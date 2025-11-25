/*
** EPITECH PROJECT, 2024
** Jetpack
** File description:
** run_server.c
*/

#include "../Include/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

static int create_world_and_resources(server_t *server)
{
    server->world = create_world(server->width, server->height);
    if (!server->world) {
        printf("Erreur: Impossible de créer le monde\n");
        return -1;
    }
    world_initialize_resources(server->world);
    srand(time(NULL));
    for (int i = 0; i < server->nb_teams; i++) {
        initialize_team_eggs(server, i);
    }
    return 0;
}

static int create_socket(server_t *server)
{
    char error_msg[MAX_MESSAGE_LENGTH];
    int opt = 1;

    server->server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server->server_sock == -1) {
        snprintf(error_msg, MAX_MESSAGE_LENGTH, "Socket failed: %s",
            strerror(errno));
        fprintf(stderr, "%s", error_msg);
        return -1;
    }
    if (setsockopt(server->server_sock, SOL_SOCKET,
        SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        return -1;
    memset(&server->server_addr, 0, sizeof(server->server_addr));
    server->server_addr.sin_family = AF_INET;
    server->server_addr.sin_port = htons(server->port);
    server->server_addr.sin_addr.s_addr = INADDR_ANY;
    return 0;
}

static int bind_and_listen(server_t *server)
{
    char error_msg[MAX_MESSAGE_LENGTH];
    struct sockaddr *addr;
    socklen_t addr_len;

    addr = (struct sockaddr*)&server->server_addr;
    addr_len = sizeof(server->server_addr);
    if (bind(server->server_sock, addr, addr_len) < 0) {
        snprintf(error_msg, MAX_MESSAGE_LENGTH, "Bind failed: %s",
            strerror(errno));
        fprintf(stderr, "%s", error_msg);
        return -1;
    }
    if (listen(server->server_sock, BACKLOG) < 0) {
        snprintf(error_msg, MAX_MESSAGE_LENGTH, "Listen failed: %s",
            strerror(errno));
        fprintf(stderr, "%s", error_msg);
        return -1;
    }
    return 0;
}

static int init_client_array(server_t *server)
{
    server->next_egg_id = 1;
    server->game_start_time = get_current_time();
    server->last_resource_regeneration = server->game_start_time;
    if (create_world_and_resources(server) != 0) {
        free(server);
        return -1;
    }
    server->clients[0].fd = server->server_sock;
    server->clients[0].events = POLLIN;
    server->clients[0].revents = 0;
    server->nb_clients = 1;
    for (int i = 1; i <= MAX_CLIENTS; i++) {
        server->clients[i].fd = -1;
        server->clients[i].events = 0;
        server->clients[i].revents = 0;
    }
    server->nb_players = 0;
    return 0;
}

server_t *server_init(int argc, char **argv, int skip)
{
    server_t *server = malloc(sizeof(server_t));

    memset(server, 0, sizeof(server_t));
    if (parse_arguments(server, argc, argv, skip) != 0) {
        free(server);
        return NULL;
    }
    if (create_socket(server) != 0) {
        free(server);
        return NULL;
    }
    if (bind_and_listen(server) != 0) {
        close(server->server_sock);
        free(server);
        return NULL;
    }
    if (init_client_array(server) != 0)
        return NULL;
    init_game_functions(server);
    return server;
}

static float calculate_delta_time(double *last_time)
{
    struct timeval tv;
    double current_time;
    float delta_time;

    gettimeofday(&tv, NULL);
    current_time = (tv.tv_sec * 1000.0) + (tv.tv_usec / 1000.0);
    delta_time = (float)(current_time - *last_time);
    return delta_time;
}

static int handle_poll(server_t *server, int poll_timeout)
{
    char error_msg[MAX_MESSAGE_LENGTH];

    if (poll(server->clients, server->nb_clients, poll_timeout) < 0) {
        snprintf(error_msg, MAX_MESSAGE_LENGTH, "Poll error: %s",
            strerror(errno));
        fprintf(stderr, "%s", error_msg);
        return -1;
    }
    return 0;
}

void process_single_client(server_t *server, int *i)
{
    if (server->clients[*i].revents & POLLIN) {
        if (!get_clients_data(server, *i)) {
            disconnect_client(server, *i, "client déconnecté");
        }
        if (server->clients[*i].fd == -1)
            (*i)--;
    }
}

static void update_server_timers(server_t *server, double *last_time)
{
    float delta_time = calculate_delta_time(last_time);
    const float frame_time = 3.0f;
    struct timeval tv;

    if (delta_time >= frame_time) {
        update_game_state(server);
        gettimeofday(&tv, NULL);
        *last_time = (tv.tv_sec * 1000.0) + (tv.tv_usec / 1000.0);
    }
}

void server_run(server_t *server)
{
    struct timeval tv;
    float frame_time = 3.0f;
    double last_time;
    float delta_time;
    int poll_timeout;

    gettimeofday(&tv, NULL);
    last_time = (tv.tv_sec * 1000.0) + (tv.tv_usec / 1000.0);
    while (1) {
        delta_time = calculate_delta_time(&last_time);
        if (delta_time < frame_time)
            poll_timeout = (int)(frame_time - delta_time);
        else
            poll_timeout = 0;
        if (handle_poll(server, poll_timeout) < 0)
            return;
        process_client_events(server);
        update_server_timers(server, &last_time);
    }
}

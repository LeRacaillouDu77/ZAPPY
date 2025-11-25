/*
** EPITECH PROJECT, 2024
** Jetpack
** File description:
** gest_clients.c
*/

#include "../Include/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static int accept_new_socket(server_t *server)
{
    int new_sock;
    char error_msg[MAX_MESSAGE_LENGTH];

    server->client_len = sizeof(server->client_addr);
    new_sock = accept(server->server_sock,
        (struct sockaddr*)&server->client_addr, &server->client_len);
    if (new_sock == -1) {
        snprintf(error_msg, MAX_MESSAGE_LENGTH, "Accept failed: %s",
            strerror(errno));
        fprintf(stderr, "%s", error_msg);
    }
    return new_sock;
}

static void init_new_client(server_t *server, int new_sock)
{
    server->clients[server->nb_clients].fd = new_sock;
    server->clients[server->nb_clients].events = POLLIN;
    server->clients[server->nb_clients].revents = 0;
    server->nb_clients++;
}

static void init_new_player(server_t *server, int new_sock)
{
    player_info_t *player;

    player = &server->players[server->nb_players];
    player->id = new_sock;
    strcpy(player->team, "");
    player->x = 0;
    player->y = 0;
    player->state = CLIENT_PENDING;
    player->type = CLIENT_TYPE_UNKNOWN;
    player->level = 1;
    player->orientation = 1;
    player->is_frozen = false;
    player->inventory = (Inventory_t){0, 0, 0, 0, 0, 0, 0};
    init_command_queue(&player->command_queue);
    server->nb_players++;
}

void accept_clients(server_t *server)
{
    int new_sock = accept_new_socket(server);

    if (new_sock == -1)
        return;
    if (server->nb_clients < MAX_CLIENTS + 1) {
        init_new_client(server, new_sock);
        init_new_player(server, new_sock);
        send_data_to_client(new_sock, "WELCOME\n");
        printf("Nouveau client connecté: ID=%d\n", new_sock);
    } else {
        close(new_sock);
        printf("Max clients reached, connection rejected\n");
    }
}

static void remove_player_from_team(server_t *server, int disconnected_fd,
    const char *team_name)
{
    int team_index = find_team(server, team_name);

    if (team_index < 0)
        return;
    server->teams[team_index].current_clients--;
    for (int t = 0; t < MAX_CLIENTS; t++) {
        if (server->teams[team_index].player_ids[t] == disconnected_fd) {
            server->teams[team_index].player_ids[t] = -1;
            break;
        }
    }
}

static void remove_player_from_list(server_t *server, int disconnected_fd)
{
    for (int j = 0; j < server->nb_players; j++) {
        if (server->players[j].id != disconnected_fd)
            continue;
        if (strlen(server->players[j].team) > 0)
            remove_player_from_team(server, disconnected_fd,
                server->players[j].team);
        destroy_command_queue(&server->players[j].command_queue);
        for (int k = j; k < server->nb_players - 1; k++) {
            server->players[k] = server->players[k + 1];
        }
        server->nb_players--;
        break;
    }
}

static void log_client_disconnection(int disconnected_fd,
    client_type_t client_type, const char *reason)
{
    if (client_type == CLIENT_TYPE_GUI) {
        printf("Déconnexion du client GUI (fd=%d)\n", disconnected_fd);
    } else if (client_type == CLIENT_TYPE_AI) {
        printf("Client déconnecté (fd=%d, raison: %s)\n",
            disconnected_fd, reason);
    }
}

static void get_client_info(server_t *server, int disconnected_fd,
    bool *was_connected, client_type_t *client_type)
{
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].id == disconnected_fd) {
            *client_type = server->players[i].type;
            *was_connected = (server->players[i].state == CLIENT_CONNECTED);
            break;
        }
    }
}

static void cleanup_client_data(server_t *server, int client_index,
    int disconnected_fd)
{
    close(disconnected_fd);
    server->clients[client_index].fd = -1;
    remove_player_from_list(server, disconnected_fd);
    for (int j = client_index; j < server->nb_clients - 1; j++)
        server->clients[j] = server->clients[j + 1];
    server->nb_clients--;
}

void disconnect_client(server_t *server, int client_index, const char *reason)
{
    int disconnected_fd;
    bool was_connected;
    client_type_t client_type;
    char message[256];

    if (client_index < 1 || client_index >= server->nb_clients)
        return;
    disconnected_fd = server->clients[client_index].fd;
    was_connected = false;
    client_type = CLIENT_TYPE_UNKNOWN;
    get_client_info(server, disconnected_fd, &was_connected, &client_type);
    log_client_disconnection(disconnected_fd, client_type, reason);
    cleanup_client_data(server, client_index, disconnected_fd);
    if (was_connected && client_type == CLIENT_TYPE_AI) {
        snprintf(message, sizeof(message), "pdi #%d\n", disconnected_fd);
        send_to_gui(server, message);
        print("📢 Notification de déconnexion envoyée aux GUIs\n");
    }
}

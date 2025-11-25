/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** server.c
*/

#include "../Include/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int debug_mode;

void server_destroy(server_t *server)
{
    if (!server)
        return;
    close(server->server_sock);
    for (int i = 0; i < MAX_CLIENTS + 1; i++) {
        if (server->clients[i].fd > 0)
            close(server->clients[i].fd);
    }
    if (server->world) {
        world_destroy(server->world);
        server->world = NULL;
    }
    free(server);
}

void process_client_events(server_t *server)
{
    if (server->clients[0].revents & POLLIN)
        accept_clients(server);
    for (int i = 1; i < server->nb_clients; i++) {
        process_single_client(server, &i);
    }
}

void print(const char *format, ...)
{
    va_list args;

    if (!debug_mode)
        return;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

static void print_teams_list(server_t *server)
{
    for (int i = 0; i < server->nb_teams; i++) {
        printf("%s", server->teams[i].name);
        if (i < server->nb_teams - 1)
            printf(", ");
    }
}

void print_server_info(server_t *server)
{
    printf("=== SERVEUR ZAPPY DÉMARRÉ 🚀 ===\n");
    printf("Port: %d\n", server->port);
    printf("Carte: %dx%d\n", server->width, server->height);
    printf("Clients par équipe: %d\n", server->clients_per_team);
    printf("Fréquence: %d\n", server->freq);
    printf("Équipes: ");
    print_teams_list(server);
    printf("\n");
    printf("Statut: En ligne ✅\n");
    printf("================================\n");
}

int main(int argc, char **argv)
{
    server_t *server = NULL;
    int skip = 0;

    debug_mode = 0;
    if (argc == 1) {
        skip = 1;
    }
    if (!skip && argc < 2) {
        printf("Usage: %s -p port -x width -y height -n team1 team2 ... "
            "-c clientsNb -f freq\n", argv[0]);
        return 84;
    }
    server = server_init(argc, argv, skip);
    if (server == NULL)
        return 84;
    print_server_info(server);
    server_run(server);
    server_destroy(server);
    return 0;
}

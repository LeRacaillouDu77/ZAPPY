/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** tna.c
*/

#include "../../../Include/server.h"

void handle_tna_command(server_t *server, int client_fd)
{
    char message[1024];

    for (int i = 0; i < server->nb_teams; i++) {
        snprintf(message, sizeof(message), "tna %s\n", server->teams[i].name);
        send_data_to_client(client_fd, message);
    }
}

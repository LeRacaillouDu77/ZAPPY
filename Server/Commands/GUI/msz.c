/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** msz.c
*/

#include "../../../Include/server.h"

void handle_msz_command(server_t *server, int client_fd)
{
    char message[1024];

    snprintf(message, sizeof(message), "msz %d %d\n",
        server->width, server->height);
    send_data_to_client(client_fd, message);
}

/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** sgt.c
*/

#include "../../../Include/server.h"

void handle_sgt_command(server_t *server, int client_fd)
{
    char response[64];

    snprintf(response, sizeof(response), "sgt %d\n", server->freq);
    send_data_to_client(client_fd, response);
}

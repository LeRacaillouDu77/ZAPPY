/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** mct.c
*/

#include "../../../Include/server.h"

static void send_row_data(server_t *server, int client_fd, int y)
{
    char message[256];
    tile_t *tile;

    for (int x = 0; x < server->width; x++) {
        tile = world_get_tile(server->world, x, y);
        if (tile) {
            format_tile_message(message, x, y, tile);
            send_data_to_client(client_fd, message);
        }
    }
}

void handle_mct_command(server_t *server, int client_fd)
{
    for (int y = 0; y < server->height; y++) {
        send_row_data(server, client_fd, y);
    }
}

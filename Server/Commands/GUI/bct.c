/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** bct.c
*/

#include "../../../Include/server.h"

void handle_bct_command(server_t *server, int client_fd, int x, int y)
{
    char message[256];
    tile_t *tile;

    if (x < 0 || x >= server->width || y < 0 || y >= server->height) {
        send_data_to_client(client_fd, "sbp\n");
        return;
    }
    tile = world_get_tile(server->world, x, y);
    if (!tile) {
        send_data_to_client(client_fd, "sbp\n");
        return;
    }
    format_tile_message(message, x, y, tile);
    send_data_to_client(client_fd, message);
}

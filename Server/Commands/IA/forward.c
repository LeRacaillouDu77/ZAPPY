/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** forward.c
*/
#include "../../../Include/server.h"

void update_position_forward(player_info_t *player)
{
    switch (player->orientation) {
        case 1:
            player->y--;
            break;
        case 2:
            player->x++;
            break;
        case 3:
            player->y++;
            break;
        case 4:
            player->x--;
            break;
    }
}

void handle_forward_command(server_t *server, int client_fd)
{
    player_info_t *player = find_player_by_fd(server, client_fd);
    char gui_message[256];
    int old_player_x = player->x;
    int old_player_y = player->y;

    if (!player) {
        send_data_to_client(client_fd, "ko\n");
        return;
    }
    update_position_forward(player);
    player->x = ((player->x % server->width)
        + server->width) % server->width;
    player->y = ((player->y % server->height)
        + server->height) % server->height;
    print("⬆️ [Joueur %d - position] (%d,%d) -> (%d,%d)\n",
        client_fd, old_player_x, old_player_y, player->x, player->y);
    snprintf(gui_message, sizeof(gui_message), "ppo #%d %d %d %d\n",
        client_fd, player->x, player->y, player->orientation);
    send_to_gui(server, gui_message);
    send_data_to_client(client_fd, "ok\n");
}

/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** left.c
*/
#include "../../../Include/server.h"

void update_position_left(player_info_t *player)
{
    player->orientation--;
    if (player->orientation < 1) {
        player->orientation = 4;
    }
}

void handle_left_command(server_t *server, int client_fd)
{
    player_info_t *player;
    int old_orientation;
    char gui_message[256];

    player = find_player_by_fd(server, client_fd);
    if (!player) {
        send_data_to_client(client_fd, "ko\n");
        return;
    }
    old_orientation = player->orientation;
    update_position_left(player);
    print("⬅️ [Joueur %d - orientation] (%d) -> (%d)\n",
        client_fd, old_orientation, player->orientation);
    snprintf(gui_message, sizeof(gui_message), "ppo #%d %d %d %d\n",
        client_fd, player->x, player->y, player->orientation);
    send_to_gui(server, gui_message);
    send_data_to_client(client_fd, "ok\n");
}

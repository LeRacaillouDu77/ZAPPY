/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** plv.c
*/

#include "../../../Include/server.h"
#include "../../../Include/player.h"

static player_info_t *find_player_by_id(server_t *server, int player_id)
{
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].id == player_id &&
            server->players[i].state == CLIENT_CONNECTED) {
            return &server->players[i];
        }
    }
    return NULL;
}

void handle_plv_gui(server_t *server, int client_fd, const char *command)
{
    player_info_t *player;
    char response[256];
    int player_id;

    if (sscanf(command, "plv #%d", &player_id) != 1) {
        send_data_to_client(client_fd, "sbp\n");
        return;
    }
    player = find_player_by_id(server, player_id);
    if (!player) {
        send_data_to_client(client_fd, "sbp\n");
        return;
    }
    snprintf(response, sizeof(response), "plv #%d %d\n",
        player_id, player->level);
    send_data_to_client(client_fd, response);
}

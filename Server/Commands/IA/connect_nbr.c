/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** connect_nbr.c
*/

#include "../../../Include/server.h"

void handle_connect_nbr_command(server_t *server, int client_fd)
{
    player_info_t *player = find_player_by_fd(server, client_fd);
    int team_index = find_team(server, player->team);
    int available_eggs = 0;
    char response[64];

    if (!player) {
        return send_data_to_client(client_fd, "ko\n");
    }
    if (team_index == -1) {
        return send_data_to_client(client_fd, "ko\n");
    }
    for (int i = 0; i < server->clients_per_team; i++) {
        if (server->teams[team_index].eggs[i].is_active)
            available_eggs++;
    }
    snprintf(response, sizeof(response), "%d\n", available_eggs);
    send_data_to_client(client_fd, response);
    print("📊 [Joueur %d - Connect_nbr] Équipe %s: %d œufs disponibles\n",
        client_fd, player->team, available_eggs);
}

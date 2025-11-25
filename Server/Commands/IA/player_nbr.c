/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** player_nbr.c
*/
#include "../../../Include/server.h"

void handle_player_nbr_command(server_t *server, int client_fd)
{
    player_info_t *player = find_player_by_fd(server, client_fd);
    int team_index;
    int connected_players = 0;
    char response[64];

    if (!player)
        return send_data_to_client(client_fd, "ko\n");
    team_index = find_team(server, player->team);
    if (team_index == -1)
        return send_data_to_client(client_fd, "ko\n");
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].state == CLIENT_CONNECTED &&
            server->players[i].type == CLIENT_TYPE_AI &&
            strcmp(server->players[i].team, player->team) == 0) {
            connected_players++;
        }
    }
    snprintf(response, sizeof(response), "%d\n", connected_players);
    send_data_to_client(client_fd, response);
}

void handle_player_pos_command(server_t *server, int client_fd)
{
    player_info_t *player = find_player_by_fd(server, client_fd);
    char response[64];

    if (!player)
        return send_data_to_client(client_fd, "ko\n");
    snprintf(response, sizeof(response), "%d,%d\n", player->x, player->y);
    send_data_to_client(client_fd, response);
}

void handle_player_orientation_command(server_t *server, int client_fd)
{
    player_info_t *player = find_player_by_fd(server, client_fd);
    char response[64];

    if (!player)
        return send_data_to_client(client_fd, "ko\n");
    snprintf(response, sizeof(response), "%d\n", player->orientation);
    send_data_to_client(client_fd, response);
}

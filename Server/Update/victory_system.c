/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** victory_system.c
*/

#include "../Include/server.h"
#include <stdio.h>
#include <string.h>

static int count_max_level_players(server_t *server, const char *team_name)
{
    int count = 0;
    const int MAX_LEVEL = 8;

    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].state == CLIENT_CONNECTED
            && server->players[i].type == CLIENT_TYPE_AI &&
            strcmp(server->players[i].team, team_name) == 0 &&
            server->players[i].level == MAX_LEVEL) {
            count++;
        }
    }
    return count;
}

static void announce_victory(server_t *server, const char *winning_team)
{
    char victory_mess[256];

    snprintf(victory_mess, sizeof(victory_mess), "seg %s\n", winning_team);
    send_to_gui(server, victory_mess);
    print("🏆 VICTOIRE ! L'équipe '%s' a gagné avec 6 joueurs au niveau 8 !\n",
        winning_team);
}

void check_victory_condition(server_t *server)
{
    const int REQUIRED_MAX_LEVEL_PLAYERS = 6;
    int max_level_players = 0;
    char *team_name = NULL;

    for (int i = 0; i < server->nb_teams; i++) {
        team_name = server->teams[i].name;
        max_level_players = count_max_level_players(server, team_name);
        if (max_level_players >= REQUIRED_MAX_LEVEL_PLAYERS) {
            announce_victory(server, team_name);
            disconnect_non_gui_clients(server);
            return;
        }
    }
}

static client_type_t get_client_type(server_t *server, int client_fd)
{
    for (int j = 0; j < server->nb_players; j++) {
        if (server->players[j].id == client_fd) {
            return server->players[j].type;
        }
    }
    return CLIENT_TYPE_UNKNOWN;
}

void disconnect_non_gui_clients(server_t *server)
{
    client_type_t client_type;

    for (int i = server->nb_clients - 1; i >= 0; i--) {
        if (server->clients[i].fd == -1) {
            continue;
        }
        client_type = get_client_type(server, server->clients[i].fd);
        if (client_type != CLIENT_TYPE_GUI) {
            disconnect_client(server, i, "Victoire !");
        }
    }
}

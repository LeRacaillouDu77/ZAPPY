/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** team_management.c
*/

#include "../Include/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void init_team_data(Team_t *team, const char *team_name,
    int clients_per_team)
{
    strncpy(team->name, team_name, MAX_NAME_LENGTH - 1);
    team->name[MAX_NAME_LENGTH - 1] = '\0';
    team->max_clients = clients_per_team;
    team->current_clients = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        team->player_ids[i] = -1;
    }
}

int add_team(server_t *server, const char *team_name)
{
    if (server->nb_teams >= MAX_TEAMS) {
        return -1;
    }
    init_team_data(&server->teams[server->nb_teams], team_name,
        server->clients_per_team);
    server->nb_teams++;
    return server->nb_teams - 1;
}

int find_team(server_t *server, const char *team_name)
{
    for (int i = 0; i < server->nb_teams; i++) {
        if (strcmp(server->teams[i].name, team_name) == 0) {
            return i;
        }
    }
    return -1;
}

int team_has_slot(server_t *server, const char *team_name)
{
    int team_index;

    team_index = find_team(server, team_name);
    if (team_index == -1) {
        return 0;
    }
    return server->teams[team_index].current_clients <
        server->teams[team_index].max_clients;
}

int add_player_to_team(server_t *server, int player_id, const char *team_name)
{
    int team_index = find_team(server, team_name);

    if (team_index == -1)
        return -1;
    if (!team_has_slot(server, team_name))
        return -1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (server->teams[team_index].player_ids[i] == -1) {
            server->teams[team_index].player_ids[i] = player_id;
            server->teams[team_index].current_clients++;
            break;
        }
    }
    return 0;
}

void send_team_info(server_t *server, int client_fd, const char *team_name)
{
    int team_index;
    int slots_available;
    char response[MAX_MESSAGE_LENGTH];

    team_index = find_team(server, team_name);
    if (team_index == -1) {
        send_data_to_client(client_fd, "ko\n");
        return;
    }
    slots_available = server->teams[team_index].max_clients -
        server->teams[team_index].current_clients;
    snprintf(response, MAX_MESSAGE_LENGTH, "%d\n%d %d\n",
        slots_available, server->width, server->height);
    send_data_to_client(client_fd, response);
}

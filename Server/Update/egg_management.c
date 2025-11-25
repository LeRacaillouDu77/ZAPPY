/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** egg_managment.c
*/

#include "../Include/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int get_next_egg_id(server_t *server)
{
    int id = server->next_egg_id;

    server->next_egg_id++;
    return id;
}

static void init_single_egg(server_t *server, Team_t *team, int index)
{
    team->eggs[index].id = get_next_egg_id(server);
    team->eggs[index].is_active = 1;
    strncpy(team->eggs[index].team, team->name, MAX_NAME_LENGTH - 1);
    team->eggs[index].team[MAX_NAME_LENGTH - 1] = '\0';
}

static void assign_egg_position_and_init(server_t *server, Team_t *team,
    int index)
{
    team->eggs[index].x = rand() % server->width;
    team->eggs[index].y = rand() % server->height;
    init_single_egg(server, team, index);
    team->nb_eggs++;
}

void initialize_team_eggs(server_t *server, int team_index)
{
    Team_t *team;

    if (team_index < 0 || team_index >= server->nb_teams)
        return;
    team = &server->teams[team_index];
    team->nb_eggs = 0;
    for (int i = 0; i < server->clients_per_team; i++) {
        assign_egg_position_and_init(server, team, i);
    }
    print("Équipe %s: %d œufs initiaux créés\n",
        team->name, team->nb_eggs);
}

static int collect_available_eggs(Team_t *team, int *available_eggs,
    int clients_per_team)
{
    int count = 0;

    for (int i = 0; i < clients_per_team; i++) {
        if (team->eggs[i].is_active) {
            available_eggs[count] = i;
            count++;
        }
    }
    return count;
}

int find_available_egg(server_t *server, const char *team_name)
{
    Team_t *team;
    int available_eggs[MAX_CLIENTS];
    int count;
    int selected_index;
    int team_index = find_team(server, team_name);

    if (team_index == -1)
        return -1;
    team = &server->teams[team_index];
    count = collect_available_eggs(team, available_eggs,
        server->clients_per_team);
    if (count == 0)
        return -1;
    selected_index = available_eggs[rand() % count];
    return team->eggs[selected_index].id;
}

static void deactivate_egg(Team_t *team, int egg_id)
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (team->eggs[i].id == egg_id && team->eggs[i].is_active) {
            team->eggs[i].is_active = 0;
            team->nb_eggs--;
            break;
        }
    }
}

static void set_ply_pos(server_t *server, int player_id, int x, int y)
{
    for (int j = 0; j < server->nb_players; j++) {
        if (server->players[j].id == player_id) {
            server->players[j].x = x;
            server->players[j].y = y;
            break;
        }
    }
}

void spawn_player_from_egg(server_t *server, int player_id, int egg_id,
    const char *team_name)
{
    int team_index = find_team(server, team_name);
    Team_t *team;
    char gui_message[256];

    if (team_index == -1)
        return;
    team = &server->teams[team_index];
    for (int i = 0; i < server->clients_per_team; i++) {
        if (team->eggs[i].id == egg_id && team->eggs[i].is_active) {
            deactivate_egg(team, egg_id);
            set_ply_pos(server, player_id, team->eggs[i].x, team->eggs[i].y);
            initialize_player_survival(server, player_id);
            print("Joueur %d apparut à (%d, %d) œuf %d équipe %s\n",
                player_id, team->eggs[i].x, team->eggs[i].y, egg_id,
                team_name);
            snprintf(gui_message, sizeof(gui_message), "ebo #%d\n", egg_id);
            send_to_gui(server, gui_message);
            break;
        }
    }
}

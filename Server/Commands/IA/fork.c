/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** fork.c
*/

#include "../../../Include/server.h"

static int validate_fork_request(server_t *server, int client_fd,
    player_info_t **player, Team_t **team)
{
    int team_index;

    *player = find_player_by_fd(server, client_fd);
    if (!*player) {
        send_data_to_client(client_fd, "ko\n");
        return 0;
    }
    team_index = find_team(server, (*player)->team);
    if (team_index == -1) {
        send_data_to_client(client_fd, "ko\n");
        return 0;
    }
    *team = &server->teams[team_index];
    return 1;
}

static int find_available_egg_slot(Team_t *team, int client_fd)
{
    int new_egg_index = -1;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!team->eggs[i].is_active) {
            new_egg_index = i;
            break;
        }
    }
    if (new_egg_index == -1) {
        print("🚫 [Joueur %d - Fork]"
            "Pas de slot libre pour un nouvel œuf\n", client_fd);
        send_data_to_client(client_fd, "ko\n");
        return -1;
    }
    return new_egg_index;
}

static void create_new_egg(server_t *server, Team_t *team,
    int egg_index, player_info_t *player)
{
    team->eggs[egg_index].id = get_next_egg_id(server);
    team->eggs[egg_index].x = player->x;
    team->eggs[egg_index].y = player->y;
    team->eggs[egg_index].is_active = 1;
    strncpy(team->eggs[egg_index].team, player->team, MAX_NAME_LENGTH - 1);
    team->eggs[egg_index].team[MAX_NAME_LENGTH - 1] = '\0';
    team->nb_eggs++;
    team->max_clients++;
}

static void send_fork_notifications(server_t *server, player_info_t *player,
    Egg_t *egg)
{
    char gui_message[256];
    int client_fd = player->id;

    print("🥚 [Joueur %d - Fork] Pond un œuf %d à (%d,%d) pour l'équipe %s\n",
        client_fd, egg->id, player->x, player->y, player->team);
    snprintf(gui_message, sizeof(gui_message), "pfk #%d\n", client_fd);
    send_to_gui(server, gui_message);
    snprintf(gui_message, sizeof(gui_message), "enw #%d #%d %d %d\n",
        egg->id, client_fd, player->x, player->y);
    send_to_gui(server, gui_message);
    send_data_to_client(client_fd, "ok\n");
}

void handle_fork_command(server_t *server, int client_fd)
{
    player_info_t *player;
    Team_t *team;
    int new_egg_index;

    if (!validate_fork_request(server, client_fd, &player, &team)) {
        return;
    }
    new_egg_index = find_available_egg_slot(team, client_fd);
    if (new_egg_index == -1) {
        return;
    }
    create_new_egg(server, team, new_egg_index, player);
    send_fork_notifications(server, player, &team->eggs[new_egg_index]);
}

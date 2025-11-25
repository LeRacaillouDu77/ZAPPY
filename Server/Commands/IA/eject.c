/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** eject.c
*/

#include "../../../Include/server.h"

static int find_players_on_tile(server_t *server, position_t pos,
    int ejector_id, int *players_to_eject)
{
    int count = 0;

    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].id != ejector_id &&
            server->players[i].x == pos.x &&
            server->players[i].y == pos.y) {
            players_to_eject[count] = i;
            count++;
        }
    }
    return count;
}

static int check_and_collect_egg(egg_collection_params_t *params,
    server_t *server, int team_idx, int egg_idx)
{
    Egg_t *egg = &server->teams[team_idx].eggs[egg_idx];

    if (egg->is_active && egg->x == params->pos.x && egg->y == params->pos.y) {
        params->eggs_to_destroy[params->count * 2] = team_idx;
        params->eggs_to_destroy[params->count * 2 + 1] = egg_idx;
        return params->count + 1;
    }
    return params->count;
}

static int find_eggs_on_tile(server_t *server, position_t pos,
    int *eggs_to_destroy)
{
    egg_collection_params_t params = {
        .pos = pos,
        .eggs_to_destroy = eggs_to_destroy,
        .count = 0
    };

    for (int team_idx = 0; team_idx < server->nb_teams; team_idx++) {
        for (int egg_idx = 0; egg_idx < server->clients_per_team; egg_idx++) {
            params.count = check_and_collect_egg(&params,
                server, team_idx, egg_idx);
        }
    }
    return params.count;
}

static position_t ejection_position(server_t *server,
    int orientation, position_t current_pos)
{
    position_t new_pos;

    new_pos.x = current_pos.x;
    new_pos.y = current_pos.y;
    switch (orientation) {
        case 1:
            new_pos.y = (new_pos.y - 1 + server->height) % server->height;
            break;
        case 2:
            new_pos.x = (new_pos.x + 1) % server->width;
            break;
        case 3:
            new_pos.y = (new_pos.y + 1) % server->height;
            break;
        case 4:
            new_pos.x = (new_pos.x - 1 + server->width) % server->width;
            break;
    }
    return new_pos;
}

static int get_opposite_direction(int orientation)
{
    switch (orientation) {
        case 1:
            return 3;
        case 2:
            return 4;
        case 3:
            return 1;
        case 4:
            return 2;
        default:
            return 1;
    }
}

static void eject_player(server_t *server, int player_idx, position_t new_pos,
    int ejector_orientation)
{
    player_info_t *player = &server->players[player_idx];
    char gui_message[256];
    char eject_message[32];
    int old_x = player->x;
    int old_y = player->y;
    int opposite_dir = get_opposite_direction(ejector_orientation);

    player->x = new_pos.x;
    player->y = new_pos.y;
    snprintf(eject_message, sizeof(eject_message),
        "eject: %d\n", opposite_dir);
    send_data_to_client(player->id, eject_message);
    snprintf(gui_message, sizeof(gui_message), "ppo #%d %d %d %d\n",
        player->id, player->x, player->y, player->orientation);
    send_to_gui(server, gui_message);
    printf("🚀 [Éjection] Joueur %d déplacé de (%d,%d) vers (%d,%d)\n",
        player->id, old_x, old_y, new_pos.x, new_pos.y);
}

static void destroy_egg(server_t *server, int team_idx, int egg_idx)
{
    Egg_t *egg = &server->teams[team_idx].eggs[egg_idx];
    char gui_message[256];
    int egg_id = egg->id;

    egg->is_active = 0;
    server->teams[team_idx].nb_eggs--;
    server->teams[team_idx].max_clients--;
    snprintf(gui_message, sizeof(gui_message), "edi #%d\n", egg_id);
    send_to_gui(server, gui_message);
    print("🥚💥 [Éjection] Œuf %d détruit à (%d,%d)\n",
        egg_id, egg->x, egg->y);
}

static void execute_ejections(server_t *server, eject_targets_t *targets,
    position_t new_pos, int ejector_orientation)
{
    int team_idx = 0;
    int egg_idx = 0;

    for (int i = 0; i < targets->players_count; i++) {
        eject_player(server, targets->players_to_eject[i], new_pos,
        ejector_orientation);
    }
    for (int i = 0; i < targets->eggs_count; i++) {
        team_idx = targets->eggs_to_destroy[i * 2];
        egg_idx = targets->eggs_to_destroy[i * 2 + 1];
        destroy_egg(server, team_idx, egg_idx);
    }
}

static void collect_eject_targets(server_t *server, position_t ejector_pos,
    int client_fd, eject_targets_t *targets)
{
    targets->players_count = find_players_on_tile(server, ejector_pos,
        client_fd,
        targets->players_to_eject);
    targets->eggs_count = find_eggs_on_tile(server, ejector_pos,
        targets->eggs_to_destroy);
}

void handle_eject_command(server_t *server, int client_fd)
{
    player_info_t *ejector = find_player_by_fd(server, client_fd);
    int players_to_eject[MAX_CLIENTS];
    int eggs_to_destroy[MAX_CLIENTS * 2];
    eject_targets_t targets;
    position_t ejector_pos = { .x = ejector->x, .y = ejector->y };
    position_t new_pos;
    char gui_message[256];

    if (!ejector)
        return send_data_to_client(client_fd, "ko\n");
    targets.players_to_eject = players_to_eject;
    targets.eggs_to_destroy = eggs_to_destroy;
    collect_eject_targets(server, ejector_pos, client_fd, &targets);
    new_pos = ejection_position(server, ejector->orientation, ejector_pos);
    execute_ejections(server, &targets, new_pos, ejector->orientation);
    snprintf(gui_message, sizeof(gui_message), "pex #%d\n", client_fd);
    send_to_gui(server, gui_message);
    send_data_to_client(client_fd, "ok\n");
    print("✅ [Éjection] Joueur %d: %d joueurs éjectés, %d œufs détruits\n",
        client_fd, targets.players_count, targets.eggs_count);
}

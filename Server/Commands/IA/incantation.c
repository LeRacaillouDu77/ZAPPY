/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** incantation.c
*/

#include "../../../Include/server.h"

static void get_participants(server_t *server, incantation_context_t context,
    participants_t *participants)
{
    participants->count = 0;
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].type == CLIENT_TYPE_AI &&
            server->players[i].state == CLIENT_CONNECTED &&
            server->players[i].x == context.x &&
            server->players[i].y == context.y &&
            server->players[i].level == context.level) {
            participants->participant_ids[participants->count] =
                server->players[i].id;
            (participants->count)++;
        }
    }
}

static void consume_resources(server_t *server, incantation_context_t context)
{
    tile_t *tile = world_get_tile(server->world, context.x, context.y);
    const int *req = level_requirements[context.level - 1];

    if (!tile)
        return;
    for (int i = 1; i < 7; i++) {
        tile->resources[i] -= req[i];
        server->world->total_resources[i] -= req[i];
    }
    print("💎 [Incantation] Ressources consommées pour niveau %d→%d\n",
        context.level, context.level + 1);
}

static void level_up_participants(server_t *server,
    participants_t *participants)
{
    player_info_t *player = NULL;
    char gui_message[256];

    for (int i = 0; i < participants->count; i++) {
        player = find_player_by_fd(server,
            participants->participant_ids[i]);
        if (player) {
            player->level++;
            print("⬆️ [Incantation] Joueur %d monte au niveau %d\n",
                player->id, player->level);
            snprintf(gui_message, sizeof(gui_message), "plv #%d %d\n",
                player->id, player->level);
            send_to_gui(server, gui_message);
        }
    }
}

static void send_gui_pie_notification(server_t *server,
    incantation_context_t context, int success)
{
    char gui_message[256];

    snprintf(gui_message, sizeof(gui_message), "pie %d %d %d\n",
        context.x, context.y, success);
    send_to_gui(server, gui_message);
    print("📺 [GUI] Fin incantation: %s", gui_message);
}

static void send_response_to_participants(participants_t *participants,
    int success, int new_level)
{
    char response[128];

    if (success) {
        snprintf(response, sizeof(response),
            "Elevation underway Current level: %d\n", new_level);
        send_data_to_client(participants->participant_ids[0], response);
    } else {
        send_data_to_client(participants->participant_ids[0], "ko\n");
    }
}

static void execute_successful_incantation(server_t *server,
    incantation_context_t context, participants_t *participants)
{
    consume_resources(server, context);
    level_up_participants(server, participants);
    send_response_to_participants(participants, 1, context.level + 1);
    send_gui_pie_notification(server, context, 1);
    print("✅ [Incantation] Succès! %d joueurs montent au niveau %d\n",
        participants->count, context.level + 1);
}

static void execute_failed_incantation(server_t *server,
    incantation_context_t context, participants_t *participants)
{
    send_response_to_participants(participants, 0, context.level);
    send_gui_pie_notification(server, context, 0);
}

void handle_incantation_command(server_t *server, int client_fd)
{
    player_info_t *init = find_player_by_fd(server, client_fd);
    int participants_array[MAX_CLIENTS];
    participants_t participants;
    incantation_context_t context;

    if (!init)
        return send_data_to_client(client_fd, "ko\n");
    context = (incantation_context_t){init->x, init->y, init->level};
    participants = (participants_t){participants_array, 0};
    print("🔮 [Joueur %d - Incantation] Tentative niveau %d→%d à (%d,%d)\n",
        client_fd, context.level, context.level + 1, context.x, context.y);
    get_participants(server, context, &participants);
    if (!verify_incantation_conditions(server, init)) {
        execute_failed_incantation(server, context, &participants);
        return send_data_to_client(client_fd, "ko\n");
    }
    if (verify_incantation_conditions(server, init) == 1)
        execute_successful_incantation(server, context, &participants);
    else
        execute_failed_incantation(server, context, &participants);
}

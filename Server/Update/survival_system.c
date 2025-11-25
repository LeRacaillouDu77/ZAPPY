/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** survival_system.c
*/

#include "../../Include/server.h"

void initialize_player_survival(server_t *server, int player_id)
{
    player_info_t *player = NULL;
    double current_time = get_current_time();

    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].id == player_id) {
            player = &server->players[i];
            break;
        }
    }
    if (!player)
        return;
    player->survival_time = INITIAL_FOOD * FOOD_SURVIVAL_TIME;
    player->last_update = current_time;
    player->inventory.n_food = INITIAL_FOOD;
}

static void send_pin(server_t *server, int player_id, player_info_t *player)
{
    char message[256];

    snprintf(message, sizeof(message), "pin #%d %d %d %d %d %d %d %d %d %d\n",
        player_id,
        player->x, player->y,
        player->inventory.n_food,
        player->inventory.n_linemate,
        player->inventory.n_deraumere,
        player->inventory.n_sibur,
        player->inventory.n_mendiane,
        player->inventory.n_phiras,
        player->inventory.n_thystame);
    send_to_gui(server, message);
}

static void handle_starvation(server_t *server,
    player_info_t *player)
{
    double original_survival_time = player->survival_time;
    double time_elapsed = get_current_time() - player->last_update;
    double game_time_elapsed = time_elapsed * server->freq;
    int original_food_cycles = 0;
    int current_food_cycles = 0;
    int food_to_consume = 0;
    int actual_consumed = 0;

    player->survival_time -= game_time_elapsed;
    player->last_update = get_current_time();
    original_food_cycles = (int)(original_survival_time / FOOD_SURVIVAL_TIME);
    current_food_cycles = (int)(player->survival_time / FOOD_SURVIVAL_TIME);
    food_to_consume = original_food_cycles - current_food_cycles;
    if (food_to_consume > 0 && player->inventory.n_food > 0) {
        actual_consumed = (food_to_consume > player->inventory.n_food) ?
            player->inventory.n_food : food_to_consume;
        player->inventory.n_food -= actual_consumed;
        send_pin(server, player->id, player);
        print("🔔 Player %d a consomme %d food\n", player->id, actual_consumed);
    }
}

void update_player_survival(server_t *server, int player_id)
{
    player_info_t *player = NULL;

    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].id == player_id &&
            server->players[i].state == CLIENT_CONNECTED &&
            server->players[i].type == CLIENT_TYPE_AI) {
            player = &server->players[i];
            break;
        }
    }
    if (!player)
        return;
    handle_starvation(server, player);
    if (player->survival_time <= 0.0 && player->inventory.n_food <= 0) {
        print("💀 Joueur %d est mort de faim\n", player_id);
        disconnect_client(server, player_id + 1, "Mort de faim");
    }
}

void check_players_survival(server_t *server)
{
    if (server == NULL)
        return;
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].state == CLIENT_CONNECTED &&
            server->players[i].type == CLIENT_TYPE_AI) {
            update_player_survival(server, server->players[i].id);
        }
    }
}

void add_food_to_survival(server_t *server, int player_id, int food_amount)
{
    player_info_t *player = NULL;
    double additional_survival_time;

    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].id == player_id &&
            server->players[i].state == CLIENT_CONNECTED &&
            server->players[i].type == CLIENT_TYPE_AI) {
            player = &server->players[i];
            break;
        }
    }
    if (!player)
        return;
    additional_survival_time = food_amount * FOOD_SURVIVAL_TIME;
    player->survival_time += additional_survival_time;
    print("🔔 Player %d gained %.2f survival time units from %d food\n",
        player_id, additional_survival_time, food_amount);
}

/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** manage.c
*/

#include "../Include/server.h"

void init_game_functions(server_t *server)
{
    server->game_functions.check_players_survival = check_players_survival;
    server->game_functions.regenerate_resources = regenerate_resources;
    server->game_functions.process_command_queues = process_command_queues;
    server->game_functions.check_victory_condition = check_victory_condition;
}

void update_game_state(server_t *server)
{
    double current_time = get_current_time();

    server->game_functions.check_players_survival(server);
    server->game_functions.regenerate_resources(server, current_time);
    server->game_functions.process_command_queues(server);
    server->game_functions.check_victory_condition(server);
}

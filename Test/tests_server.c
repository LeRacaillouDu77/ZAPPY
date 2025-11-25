/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** tests_server.c
*/

#include <criterion/criterion.h>
#include "../Include/server.h"

int debug_mode = 0;

Test(parse_arguments, valid_arguments)
{
    server_t server;
    char *argv[] = {"./zappy_server", "-p", "4242", "-x", "10", "-y", "10",
                    "-n", "team1", "team2", "-c", "3", "-f", "100"};
    int argc = 14;
    int result = parse_arguments(&server, argc, argv, 0);

    cr_assert_eq(result, 0, "Parse arguments should succeed");
    cr_assert_eq(server.port, 4242, "Port should be 4242");
    cr_assert_eq(server.width, 10, "Width should be 10");
    cr_assert_eq(server.height, 10, "Height should be 10");
    cr_assert_eq(server.clients_per_team, 3, "Clients per team should be 3");
    cr_assert_eq(server.freq, 100, "Frequency should be 100");
    cr_assert_eq(server.nb_teams, 2, "Should have 2 teams");
    cr_assert_str_eq(server.teams[0].name, "team1", "First team should be team1");
    cr_assert_str_eq(server.teams[1].name, "team2", "Second team should be team2");
}

Test(parse_default_args, invalid_run)
{
    server_t server;
    char *argv[] = {"./zappy_server"};
    int argc = 1;
    int result = parse_arguments(&server, argc, argv, 0);

    cr_assert_neq(result, 0, "Parse arguments should fail with no parameters");
}

/*
** EPITECH PROJECT, 2024
** Jetpack
** File description:
** parse_args.c
*/

#include "../Include/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void init_server_params(server_t *server, int skip)
{
    server->port = -1;
    server->width = -1;
    server->height = -1;
    server->clients_per_team = -1;
    server->freq = -1;
    server->nb_teams = 0;
    if (skip == 1) {
        server->port = 4242;
        server->width = 10;
        server->height = 10;
        server->clients_per_team = 2;
        server->freq = 10;
        server->nb_teams = 2;
    }
    strncpy(server->teams[0].name, "team1", MAX_NAME_LENGTH - 1);
    strncpy(server->teams[1].name, "team2", MAX_NAME_LENGTH - 1);
}

static int parse_port_param(server_t *server, char **argv, int *i, int argc)
{
    if (strcmp(argv[*i], "-p") == 0 && *i + 1 < argc) {
        server->port = atoi(argv[*i + 1]);
        *i += 2;
        return 1;
    }
    return 0;
}

static int parse_dimensions_params(server_t *server, char **argv, int *i,
    int argc)
{
    if (strcmp(argv[*i], "-x") == 0 && *i + 1 < argc &&
        argv[*i + 1][0] != '-') {
        server->width = atoi(argv[*i + 1]);
        *i += 2;
        return 1;
    } else if (strcmp(argv[*i], "-y") == 0 && *i + 1 < argc &&
        argv[*i + 1][0] != '-') {
        server->height = atoi(argv[*i + 1]);
        *i += 2;
        return 1;
    }
    return 0;
}

static int parse_advanced_params(server_t *server, char **argv, int *i,
    int argc)
{
    if (strcmp(argv[*i], "-c") == 0 && *i + 1 < argc &&
        argv[*i + 1][0] != '-') {
        server->clients_per_team = atoi(argv[*i + 1]);
        *i += 2;
        return 1;
    }
    if (strcmp(argv[*i], "-f") == 0 && *i + 1 < argc &&
        argv[*i + 1][0] != '-') {
        server->freq = atoi(argv[*i + 1]);
        *i += 2;
        return 1;
    }
    if (strcmp(argv[*i], "-d") == 0) {
        debug_mode = 1;
        (*i)++;
        return 1;
    }
    return 0;
}

static int parse_teams(server_t *server, char **argv, int *i, int argc)
{
    (*i)++;
    while (*i < argc && argv[*i][0] != '-' && server->nb_teams < MAX_TEAMS) {
        strncpy(server->teams[server->nb_teams].name, argv[*i],
            MAX_NAME_LENGTH - 1);
        server->teams[server->nb_teams].name[MAX_NAME_LENGTH - 1] = '\0';
        server->teams[server->nb_teams].max_clients = 0;
        server->teams[server->nb_teams].current_clients = 0;
        server->nb_teams++;
        (*i)++;
    }
    return 0;
}

static int more_checks(int width, int height)
{
    if (width != height) {
        printf("Erreur: La carte doit être un carré (largeur = hauteur)\n");
        return -1;
    }
    return 0;
}

static int validate_params(server_t *server)
{
    int total_eggs;
    int map_size;

    if (server->port == -1 || server->width == -1 ||
        server->height == -1 || server->clients_per_team <= 0 ||
        server->freq <= 0 || server->nb_teams <= 0) {
        printf("Erreur: Paramètres manquants\n");
        printf("Usage: -p port -x width -y height -n team1 team2 ... ");
        printf("-c clientsNb -f freq\n");
        return -1;
    }
    total_eggs = server->nb_teams * server->clients_per_team;
    map_size = server->width * server->height;
    if (more_checks(server->width, server->height) != 0)
        return -1;
    if (total_eggs > map_size) {
        printf("Carte trop petite pour le nombre d'équipes et d'œufs\n");
        return -1;
    }
    return 0;
}

static int parse_single_param(server_t *server, char **argv, int *i, int argc)
{
    if (parse_port_param(server, argv, i, argc))
        return 1;
    if (parse_dimensions_params(server, argv, i, argc))
        return 1;
    if (parse_advanced_params(server, argv, i, argc))
        return 1;
    return 0;
}

static int handle_argument(server_t *server, char **argv, int *i, int argc)
{
    if (strcmp(argv[*i], "-n") == 0) {
        parse_teams(server, argv, i, argc);
        return 1;
    }
    if (parse_single_param(server, argv, i, argc))
        return 1;
    (*i)++;
    return 1;
}

int parse_arguments(server_t *server, int argc, char **argv, int skip)
{
    int i = 1;

    init_server_params(server, skip);
    while (i < argc) {
        handle_argument(server, argv, &i, argc);
    }
    if (validate_params(server) != 0)
        return -1;
    for (int j = 0; j < server->nb_teams; j++) {
        server->teams[j].max_clients = server->clients_per_team;
        for (int k = 0; k < MAX_CLIENTS; k++) {
            server->teams[j].player_ids[k] = -1;
        }
    }
    return 0;
}

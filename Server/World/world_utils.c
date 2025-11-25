/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** world_utils.c
*/

#include "../../Include/world.h"
#include "../../Include/server.h"
#include <sys/time.h>
#include <math.h>

const char *resource_names[MAX_RESOURCE_TYPES] = {
    "food", "linemate", "deraumere", "sibur",
    "mendiane", "phiras", "thystame"
};

double get_current_time(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int count_connected_players(server_t *server)
{
    int connected_players = 0;

    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].state == CLIENT_CONNECTED &&
            server->players[i].type == CLIENT_TYPE_AI) {
            connected_players++;
        }
    }
    return connected_players;
}

static void normalize_coordinates(server_t *server, int *x, int *y)
{
    *x = ((*x % server->width) + server->width) % server->width;
    *y = ((*y % server->height) + server->height) % server->height;
}

static void add_players_to_content(server_t *server, int x, int y,
    char *content)
{
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].state == CLIENT_CONNECTED &&
            server->players[i].type == CLIENT_TYPE_AI &&
            server->players[i].x == x && server->players[i].y == y) {
            strcat(content, "player ");
        }
    }
}

static void add_resources_to_content(tile_t *tile, char *content)
{
    for (int r = 0; r < MAX_RESOURCE_TYPES; r++) {
        for (int count = 0; count < tile->resources[r]; count++) {
            strcat(content, resource_names[r]);
            strcat(content, " ");
        }
    }
}

static void clean_content_string(char *content)
{
    int len = strlen(content);

    if (len > 0 && content[len - 1] == ' ') {
        content[len - 1] = '\0';
    }
    if (strlen(content) == 0) {
        strcpy(content, "");
    }
}

char *generate_tile_content(server_t *server, int x, int y)
{
    tile_t *tile;
    static char content[512];

    if (!server || !server->world)
        return NULL;
    normalize_coordinates(server, &x, &y);
    tile = world_get_tile(server->world, x, y);
    if (!tile)
        return NULL;
    content[0] = '\0';
    add_players_to_content(server, x, y, content);
    add_resources_to_content(tile, content);
    clean_content_string(content);
    return content;
}

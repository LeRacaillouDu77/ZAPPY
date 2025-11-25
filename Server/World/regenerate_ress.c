/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** regenerate_ress.c
*/

#include "../../Include/world.h"
#include "../../Include/server.h"

static int count_tile_resources(tile_t *tile)
{
    int total = 0;

    for (int r = 0; r < MAX_RESOURCE_TYPES; r++) {
        total += tile->resources[r];
    }
    return total;
}

static int is_tile_already_modified(int *modified_tiles, int nb_modified,
    int tile_index)
{
    for (int i = 0; i < nb_modified; i++) {
        if (modified_tiles[i] == tile_index)
            return 1;
    }
    return 0;
}

static void add_modified_tile(int *modified_tiles, int *nb_modified,
    int tile_index, world_t *world)
{
    int max_tiles;

    max_tiles = world->width * world->height;
    if (!is_tile_already_modified(modified_tiles, *nb_modified, tile_index) &&
        *nb_modified < max_tiles) {
        modified_tiles[*nb_modified] = tile_index;
        (*nb_modified)++;
    }
}

static void place_resource_on_tile(world_t *world, position_t pos, int r,
    tile_tracking_t *tracking)
{
    int tile_index;

    world->tiles[pos.y][pos.x].resources[r]++;
    world->total_resources[r]++;
    if (tracking && tracking->modified_tiles && tracking->nb_modified) {
        tile_index = pos.y * world->width + pos.x;
        add_modified_tile(tracking->modified_tiles, tracking->nb_modified,
            tile_index, world);
    }
}

static void distribute_single_resource(world_t *world, int resource_type,
    int quantity, tile_tracking_t *tracking)
{
    int remaining;
    position_t pos;

    remaining = quantity;
    while (remaining > 0) {
        pos.x = rand() % world->width;
        pos.y = rand() % world->height;
        if (count_tile_resources(&world->tiles[pos.y][pos.x]) <
            MAX_RESOURCES_PER_TILE) {
            place_resource_on_tile(world, pos, resource_type, tracking);
        }
        remaining--;
    }
}

void distribute_resources(world_t *world, int max_qte[MAX_RESOURCE_TYPES],
    int *modified_tiles, int *nb_modified)
{
    tile_tracking_t tracking;

    tracking.modified_tiles = modified_tiles;
    tracking.nb_modified = nb_modified;
    if (modified_tiles && nb_modified) {
        *nb_modified = 0;
    }
    for (int r = 0; r < MAX_RESOURCE_TYPES; r++) {
        distribute_single_resource(world, r, max_qte[r], &tracking);
    }
}

static void send_tile_update_to_gui(server_t *server, int tile_index)
{
    char bct_message[256];
    int x;
    int y;
    tile_t *tile;

    x = tile_index % server->world->width;
    y = tile_index / server->world->width;
    tile = world_get_tile(server->world, x, y);
    if (tile) {
        snprintf(bct_message, sizeof(bct_message),
            "bct %d %d %d %d %d %d %d %d %d\n",
            x, y, tile->resources[0], tile->resources[1],
            tile->resources[2], tile->resources[3],
            tile->resources[4], tile->resources[5],
            tile->resources[6]);
        send_to_gui(server, bct_message);
    }
}

static void notify_gui_of_changes(server_t *server, int *modified_tiles,
    int nb_modified)
{
    for (int i = 0; i < nb_modified; i++) {
        send_tile_update_to_gui(server, modified_tiles[i]);
    }
    print("📡 Envoyé %d commandes bct au GUI pour les tiles modifiées\n",
        nb_modified);
}

void regenerate_resources(server_t *server, double time)
{
    int connected_players = count_connected_players(server);
    double elapsed;
    int modified_tiles[10000];
    int nb_modified;

    if (connected_players == 0)
        return;
    if (server->last_resource_regeneration == server->game_start_time) {
        server->last_resource_regeneration = time;
        print("🌱 Premier joueur connecté - début de la régénération\n");
        return;
    }
    elapsed = (time - server->last_resource_regeneration) * server->freq;
    if (elapsed >= RESOURCE_REGEN_TIME) {
        nb_modified = world_regenerate_resources(server->world,
            modified_tiles);
        notify_gui_of_changes(server, modified_tiles, nb_modified);
        server->last_resource_regeneration = time;
    }
}

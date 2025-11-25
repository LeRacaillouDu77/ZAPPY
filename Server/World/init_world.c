/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** init_world.c
*/

#include "../../Include/world.h"
#include "../../Include/server.h"

static void init_tile(tile_t *tile, int x, int y)
{
    tile->x = x;
    tile->y = y;
    tile->nb_players = 0;
    for (int r = 0; r < MAX_RESOURCE_TYPES; r++) {
        tile->resources[r] = 0;
    }
    for (int p = 0; p < MAX_RESOURCES_PER_TILE; p++) {
        tile->player_ids[p] = -1;
    }
}

static void cleanup_partial_world(world_t *world, int allocated_rows)
{
    for (int i = 0; i < allocated_rows; i++) {
        free(world->tiles[i]);
    }
    free(world->tiles);
    free(world);
}

static int allocate_world_tiles(world_t *world, int width, int height)
{
    for (int y = 0; y < height; y++) {
        world->tiles[y] = malloc(width * sizeof(tile_t));
        if (!world->tiles[y]) {
            cleanup_partial_world(world, y);
            return -1;
        }
        for (int x = 0; x < width; x++) {
            init_tile(&world->tiles[y][x], x, y);
        }
    }
    return 0;
}

world_t *create_world(int width, int height)
{
    world_t *world = malloc(sizeof(world_t));

    if (!world)
        return NULL;
    world->width = width;
    world->height = height;
    world->tiles = malloc(height * sizeof(tile_t *));
    if (!world->tiles) {
        free(world);
        return NULL;
    }
    if (allocate_world_tiles(world, width, height) == -1)
        return NULL;
    for (int r = 0; r < MAX_RESOURCE_TYPES; r++) {
        world->total_resources[r] = 0;
    }
    return world;
}

void world_destroy(world_t *world)
{
    if (!world)
        return;
    if (world->tiles) {
        for (int y = 0; y < world->height; y++) {
            free(world->tiles[y]);
        }
        free(world->tiles);
    }
    free(world);
}

static void calculate_target_quantities(world_t *world,
    int target_quantities[MAX_RESOURCE_TYPES])
{
    float resource_densities[MAX_RESOURCE_TYPES] = {
        0.5f, 0.3f, 0.15f, 0.1f, 0.1f, 0.08f, 0.05f
    };
    int total_tiles;

    total_tiles = world->width * world->height;
    for (int r = 0; r < MAX_RESOURCE_TYPES; r++) {
        target_quantities[r] = (int)(total_tiles * resource_densities[r]);
        world->total_resources[r] = 0;
    }
}

void world_initialize_resources(world_t *world)
{
    int target_quantities[MAX_RESOURCE_TYPES];

    if (!world)
        return;
    srand(time(NULL));
    calculate_target_quantities(world, target_quantities);
    distribute_resources(world, target_quantities, NULL, NULL);
    print("🌍 Monde initialisé (%dx%d) avec ressources distribuées\n",
        world->width, world->height);
}

int world_regenerate_resources(world_t *world, int *modified_tiles)
{
    int target_qty[MAX_RESOURCE_TYPES];
    int nb_modified = 0;

    if (!world || !modified_tiles)
        return 0;
    srand(time(NULL));
    calculate_target_quantities(world, target_qty);
    distribute_resources(world, target_qty, modified_tiles, &nb_modified);
    print("🌍 Régénération des ressources: %d tiles modifiées\n", nb_modified);
    return nb_modified;
}

tile_t *world_get_tile(world_t *world, int x, int y)
{
    if (!world || x < 0 || x >= world->width || y < 0 || y >= world->height)
        return NULL;
    return &world->tiles[y][x];
}

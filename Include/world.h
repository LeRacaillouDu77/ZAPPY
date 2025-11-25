/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** world.h
*/

#ifndef WORLD_H_
    #define WORLD_H_
    #define MAX_RESOURCES_PER_TILE 10

typedef enum {
    RESOURCE_FOOD = 0,      // q0: nourriture
    RESOURCE_LINEMATE,      // q1: linemate
    RESOURCE_DERAUMERE,     // q2: deraumere
    RESOURCE_SIBUR,         // q3: sibur
    RESOURCE_MENDIANE,      // q4: mendiane
    RESOURCE_PHIRAS,        // q5: phiras
    RESOURCE_THYSTAME,      // q6: thystame
    MAX_RESOURCE_TYPES
} resource_type_t;

typedef struct {
    int x;
    int y;
} position_t;

typedef struct {
    position_t pos;
    int *eggs_to_destroy;
    int count;
} egg_collection_params_t;

typedef struct {
    int *modified_tiles;
    int *nb_modified;
} tile_tracking_t;

typedef struct Tile {
    int x, y;
    int resources[MAX_RESOURCE_TYPES];
    int nb_players;
    int player_ids[MAX_RESOURCES_PER_TILE];
} tile_t;

typedef struct World {
    int width, height;
    tile_t **tiles;
    int total_resources[MAX_RESOURCE_TYPES];
} world_t;

typedef struct Server server_t;

world_t *create_world(int width, int height);
void world_destroy(world_t *world);
void world_initialize_resources(world_t *world);
int world_regenerate_resources(world_t *world, int *modified_tiles);
tile_t *world_get_tile(world_t *world, int x, int y);
char *generate_tile_content(server_t *server, int x, int y);
void distribute_resources(world_t *world,
    int target_quantities[MAX_RESOURCE_TYPES],
    int *modified_tiles, int *nb_modified);

extern const char *resource_names[MAX_RESOURCE_TYPES];

#endif

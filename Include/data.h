/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** data.h
*/

#ifndef DATA_H_
    #define DATA_H_
    #define FOOD_SURVIVAL_TIME 126.0
    #define INITIAL_FOOD 10
    #define RESOURCE_REGEN_TIME 150.0

    #include "player.h"
    #include "world.h"

typedef struct Server server_t;

extern const int level_requirements[8][7];

typedef struct {
    int x;
    int y;
    int level;
} incantation_context_t;

typedef struct {
    int *participant_ids;
    int count;
} participants_t;

typedef struct {
    server_t *server;
    player_info_t *player;
    tile_t *tile;
    int client_fd;
    int resource_type;
} gui_takeget_data_t;

typedef struct {
    int player_index;
    int team_index;
    int client_fd;
} connection_params_t;

typedef struct {
    int *players_to_eject;
    int *eggs_to_destroy;
    int players_count;
    int eggs_count;
} eject_targets_t;

int verify_incantation_conditions(server_t *server, player_info_t *initiator);

#endif /* DATA_H_ */

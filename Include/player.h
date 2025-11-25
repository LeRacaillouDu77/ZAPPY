/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** player.h
*/

#ifndef PLAYER_H_
    #define PLAYER_H_
    #include <stdbool.h>

    #define MAX_NAME_LENGTH 256

typedef struct Server server_t;

typedef enum {
    CLIENT_PENDING,
    CLIENT_CONNECTED
} client_state_t;

typedef enum {
    CLIENT_TYPE_UNKNOWN,
    CLIENT_TYPE_AI,
    CLIENT_TYPE_GUI
} client_type_t;

typedef struct Egg {
    int id;
    int x, y;
    int is_active;
    char team[MAX_NAME_LENGTH];
} Egg_t;

typedef struct Inventory {
    int n_food;
    int n_linemate;
    int n_deraumere;
    int n_sibur;
    int n_mendiane;
    int n_phiras;
    int n_thystame;
} Inventory_t;

typedef struct CommandNode {
    char command[MAX_NAME_LENGTH];
    double execution_time;
    struct CommandNode* next;
} command_node_t;

typedef struct CommandQueue {
    command_node_t *head;
    command_node_t *tail;
    int count;
    double current_execution_time;
    int is_executing;
} CommandQueue_t;

typedef struct PlayerInfo {
    int id;
    char team[MAX_NAME_LENGTH];
    int x, y;
    int orientation;
    int level;
    client_state_t state;
    client_type_t type;
    Inventory_t inventory;
    CommandQueue_t command_queue;
    double survival_time;
    double last_update;
    bool is_frozen;
} player_info_t;

// Gestion des équipes
int add_team(server_t *server, const char *team_name);
int find_team(server_t *server, const char *team_name);
int team_has_slot(server_t *server, const char *team_name);
int add_player_to_team(server_t *server, int player_id, const char *team_name);
int count_connected_players(server_t *server);

// Gestion des œufs
void initialize_team_eggs(server_t *server, int team_index);
int find_available_egg(server_t *server, const char *team_name);
void spawn_player_from_egg(server_t *server, int player_id, int egg_id,
    const char *team_name);

// Gestion des joueurs
player_info_t *find_player_by_fd(server_t *server, int client_fd);

// Gestion de la file d'attente des commandes
void init_command_queue(CommandQueue_t *queue);
void destroy_command_queue(CommandQueue_t *queue);
int add_command_to_queue(CommandQueue_t *queue, const char *command,
    double execution_time);
void remove_command_from_queue(CommandQueue_t *queue);
double get_command_delay(const char *command, int freq);
void process_command_queues(server_t *server);
void execute_ai_command(server_t *server, int client_fd, const char *command);

#endif // PLAYER_H_

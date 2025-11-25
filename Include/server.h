/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** server.h
*/

#ifndef SERVER_H_
    #define SERVER_H_
    #define MAX_CLIENTS 50
    #define MAX_TEAMS 10
    #define MAX_MESSAGE_LENGTH 1024
    #define BACKLOG 10

    #include <string.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <poll.h>
    #include <dirent.h>
    #include <errno.h>
    #include <signal.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <netdb.h>
    #include <sys/types.h>
    #include <time.h>
    #include "player.h"
    #include "data.h"
    #include <stdbool.h>
    #include "world.h"
    #include <stdarg.h>
    #include "communication.h"

extern int debug_mode;

typedef struct GameUpdateFunctions {
    void (*check_players_survival)(server_t *server);
    void (*regenerate_resources)(server_t *server, double current_time);
    void (*process_command_queues)(server_t *server);
    void (*check_victory_condition)(server_t *server);
} game_update_functions_t;

typedef struct Team {
    char name[MAX_NAME_LENGTH];
    int max_clients;
    int current_clients;
    int player_ids[MAX_CLIENTS];
    Egg_t eggs[MAX_CLIENTS];
    int nb_eggs;
} Team_t;

typedef struct Server {
    int server_sock;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    int port;

    // Configuration du jeu
    int width, height;      // Dimensions de la carte
    int clients_per_team;   // Nombre max de clients par équipe (-c)
    int freq;               // Fréquence du jeu (-f)

    // Monde du jeu
    world_t *world;

    // Système de temps
    double last_resource_regeneration;
    double game_start_time;

    Team_t teams[MAX_TEAMS];
    int nb_teams;
    int next_egg_id;

    player_info_t players[MAX_CLIENTS];
    struct pollfd clients[MAX_CLIENTS + 1];
    int nb_clients;
    int nb_players;
    game_update_functions_t game_functions;

} server_t;

// Gestion du serveur
server_t *server_init(int argc, char **argv, int skip);
void server_destroy(server_t *server);
void server_run(server_t *server);
int parse_arguments(server_t *server, int argc, char **argv, int skip);
void process_single_client(server_t *server, int *i);
void process_client_events(server_t *server);
void print(const char *format, ...);

// Gestion du protocole
void send_data_to_all_client(server_t *server, const char *message);
void send_data_to_client(int client_fd, const char *message);
void send_to_gui(server_t *server, const char *message);

// Gestion des clients
void accept_clients(server_t *server);
int get_clients_data(server_t *server, int client_index);
void handle_client_team_selection(server_t *server, int client_index,
    const char *team_name);
void disconnect_client(server_t *server, int client_index, const char *reason);
void disconnect_non_gui_clients(server_t *server);

// Gestion des messages
void handle_message(server_t *server, int client_fd, const char *message);
void process_single_message(server_t *server, int client_fd,
    const char *clean_message);
void handle_client_team_selection(server_t *server, int client_index,
    const char *team_name);
void handle_gui_connection(server_t *server, int client_index);
void send_team_info(server_t *server, int client_fd, const char *team_name);
void send_gui_initial_state(server_t *server, int client_fd);
void notify_new_player(server_t *server, int player_id);
void spawn_player_from_egg(server_t *server, int player_id, int egg_id,
    const char *team_name);
int find_available_egg(server_t *server, const char *team_name);
void initialize_team_eggs(server_t *server, int team_index);
int get_next_egg_id(server_t *server);

// Gestion du jeu
void update_game_state(server_t *server);
void check_victory_condition(server_t *server);
void init_game_functions(server_t *server);
double get_current_time(void);
void update_player_survival(server_t *server, int player_id);
void check_players_survival(server_t *server);
void regenerate_resources(server_t *server, double current_time);
void initialize_player_survival(server_t *server, int player_id);
void add_food_to_survival(server_t *server, int player_id, int food_amount);

#endif /* SERVER_H_ */

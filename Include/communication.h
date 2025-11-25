/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** communication.h
*/

#ifndef COMMUNICATION_H_
    #define COMMUNICATION_H_
    #define MAX_QUEUE_SIZE 10

typedef struct Server server_t;

typedef struct {
    const char *command;
    double base_time;
    int is_prefix;
} command_delay_t;

typedef struct {
    int front_dx;
    int front_dy;
    int right_dx;
    int right_dy;
} direction_vectors_t;

typedef enum {
    CMD_FORWARD = 1,
    CMD_RIGHT,
    CMD_LEFT,
    CMD_LOOK,
    CMD_INVENTORY,
    CMD_CONNECT_NBR,
    CMD_PLAYER_NBR,
    CMD_PLAYER_POS,
    CMD_ORIENTATION,
    CMD_FORK,
    CMD_EJECT,
    CMD_TAKE,
    CMD_SET,
    CMD_BROADCAST,
    CMD_INCANTATION,
    CMD_UNKNOWN = 0
} command_type_t;

typedef struct {
    const char *command;
    command_type_t type;
    int is_prefix;
} command_mapping_t;

typedef struct {
    server_t *server;
    char *response;
    int *first_tile;
} tile_response_data_t;

typedef struct {
    const char *command;
    int is_prefix;
    int needs_command;
    void (*handler_with_cmd)(server_t *server,
        int client_fd, const char *command);
    void (*handler_no_cmd)(server_t *server, int client_fd);
} gui_command_t;

// General functions
void handle_gui_commands(server_t *server, int client_fd, const char *command);
void handle_ai_commands(server_t *server, int client_fd, const char *command);
void execute_gui_command(server_t *server, int client_fd,
    const char *command, int i);
int is_valid_ai_command(const char *command);

// Gestion commandes IA
void handle_forward_command(server_t *server, int client_fd);
void handle_right_command(server_t *server, int client_fd);
void handle_left_command(server_t *server, int client_fd);
void handle_look_command(server_t *server, int client_fd);
void handle_inventory_command(server_t *server, int client_fd);
void handle_take_object_command(server_t *server, int fd, const char *co);
void handle_set_object_command(server_t *server, int fd, const char *co);
void handle_connect_nbr_command(server_t *server, int client_fd);
void handle_player_pos_command(server_t *server, int client_fd);
void handle_player_nbr_command(server_t *server, int client_fd);
void handle_player_orientation_command(server_t *server, int client_fd);
void handle_fork_command(server_t *server, int client_fd);
void handle_eject_command(server_t *server, int client_fd);
void handle_incantation_command(server_t *server, int client_fd);
void handle_broadcast_command(server_t *server, int client_fd,
    const char *command);

// Gestion des commandes GUI
void handle_bct_gui(server_t *server, int client_fd, const char *command);
void handle_pin_gui(server_t *server, int client_fd, const char *command);
void handle_plv_gui(server_t *server, int client_fd, const char *command);

void handle_bct_command(server_t *server, int client_fd, int x, int y);
void handle_mct_command(server_t *server, int client_fd);
void handle_tna_command(server_t *server, int client_fd);
void handle_msz_command(server_t *server, int client_fd);
void handle_sgt_command(server_t *server, int client_fd);
void handle_sst_gui(server_t *server, int client_fd, const char *command);
void handle_pin_command(server_t *server, int client_fd, int player_id);

void format_tile_message(char *message, int x, int y, tile_t *tile);

#endif /* COMMUNICATION_H_ */

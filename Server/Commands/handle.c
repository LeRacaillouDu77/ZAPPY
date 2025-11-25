/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** handle.c
*/

#include "../Include/server.h"

static const command_mapping_t command_map[] = {
    {"Forward", CMD_FORWARD, 0},
    {"Right", CMD_RIGHT, 0},
    {"Left", CMD_LEFT, 0},
    {"Look", CMD_LOOK, 0},
    {"Inventory", CMD_INVENTORY, 0},
    {"Connect_nbr", CMD_CONNECT_NBR, 0},
    {"Player_nbr", CMD_PLAYER_NBR, 0},
    {"Player_pos", CMD_PLAYER_POS, 0},
    {"Orientation", CMD_ORIENTATION, 0},
    {"Fork", CMD_FORK, 0},
    {"Eject", CMD_EJECT, 0},
    {"Take ", CMD_TAKE, 1},
    {"Set ", CMD_SET, 1},
    {"Broadcast ", CMD_BROADCAST, 1},
    {"Incantation", CMD_INCANTATION, 0},
    {NULL, CMD_UNKNOWN, 0}
};

player_info_t *find_player_by_fd(server_t *server, int client_fd)
{
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].id == client_fd) {
            return &server->players[i];
        }
    }
    return NULL;
}

void handle_ai_commands(server_t *server, int client_fd, const char *command)
{
    player_info_t *player;
    double delay;

    player = find_player_by_fd(server, client_fd);
    if (!player) {
        return send_data_to_client(client_fd, "ko\n");
    }
    if (!is_valid_ai_command(command)) {
        return send_data_to_client(client_fd, "ko\n");
    }
    delay = get_command_delay(command, server->freq);
    if (!add_command_to_queue(&player->command_queue, command, delay)) {
        send_data_to_client(client_fd, "ko\n");
        print("⚠️ File d'attente pleine pour le joueur %d, "
            "commande '%s' rejetée\n", client_fd, command);
        return;
    }
    print("📝 Commande '%s' ajoutée à la file d'attente du joueur %d "
        "(délai: %.1fms, file: %d/10)\n",
        command, client_fd, delay, player->command_queue.count);
}

static int check_command_match(const char *command,
    const command_mapping_t *mapping)
{
    if (mapping->is_prefix) {
        return strncmp(command, mapping->command,
            strlen(mapping->command)) == 0;
    }
    return strcmp(command, mapping->command) == 0;
}

static command_type_t get_command_type(const char *command)
{
    for (int i = 0; command_map[i].command != NULL; i++) {
        if (check_command_match(command, &command_map[i]))
            return command_map[i].type;
    }
    return CMD_UNKNOWN;
}

static void execute_movement_commands(server_t *server, int client_fd,
    const char *command)
{
    command_type_t cmd_type = get_command_type(command);

    switch (cmd_type) {
        case CMD_FORWARD:
            handle_forward_command(server, client_fd);
            break;
        case CMD_RIGHT:
            handle_right_command(server, client_fd);
            break;
        case CMD_LEFT:
            handle_left_command(server, client_fd);
            break;
        default:
            break;
    }
}

static void execute_info_commands(server_t *server, int client_fd,
    const char *command)
{
    command_type_t cmd_type = get_command_type(command);

    if (cmd_type == CMD_LOOK)
        return handle_look_command(server, client_fd);
    if (cmd_type == CMD_INVENTORY)
        return handle_inventory_command(server, client_fd);
    if (cmd_type == CMD_CONNECT_NBR)
        return handle_connect_nbr_command(server, client_fd);
    if (cmd_type == CMD_PLAYER_NBR)
        return handle_player_nbr_command(server, client_fd);
    if (cmd_type == CMD_PLAYER_POS)
        return handle_player_pos_command(server, client_fd);
    if (cmd_type == CMD_ORIENTATION)
        return handle_player_orientation_command(server, client_fd);
    if (cmd_type == CMD_FORK)
        return handle_fork_command(server, client_fd);
    if (cmd_type == CMD_EJECT)
        return handle_eject_command(server, client_fd);
    if (cmd_type == CMD_INCANTATION)
        return handle_incantation_command(server, client_fd);
}

static void execute_object_commands(server_t *server, int client_fd,
    const char *command)
{
    command_type_t cmd_type = get_command_type(command);

    switch (cmd_type) {
        case CMD_TAKE:
            handle_take_object_command(server, client_fd, command);
            break;
        case CMD_SET:
            handle_set_object_command(server, client_fd, command);
            break;
        default:
            break;
    }
}

void dispatch_command_execution(server_t *server, int client_fd,
    const char *command, command_type_t cmd_type)
{
    if (cmd_type == CMD_FORWARD || cmd_type == CMD_RIGHT ||
        cmd_type == CMD_LEFT)
        return execute_movement_commands(server, client_fd, command);
    if (cmd_type == CMD_LOOK || cmd_type == CMD_INVENTORY ||
        cmd_type == CMD_CONNECT_NBR || cmd_type == CMD_PLAYER_NBR ||
        cmd_type == CMD_FORK || cmd_type == CMD_EJECT ||
        cmd_type == CMD_INCANTATION || cmd_type == CMD_PLAYER_POS ||
        cmd_type == CMD_ORIENTATION)
        return execute_info_commands(server, client_fd, command);
    if (cmd_type == CMD_TAKE || cmd_type == CMD_SET)
        return execute_object_commands(server, client_fd, command);
    if (cmd_type == CMD_BROADCAST)
        return handle_broadcast_command(server, client_fd, command);
    if (cmd_type == CMD_UNKNOWN)
        send_data_to_client(client_fd, "ko\n");
}

void execute_ai_command(server_t *server, int client_fd, const char *command)
{
    command_type_t cmd_type;

    cmd_type = get_command_type(command);
    dispatch_command_execution(server, client_fd, command, cmd_type);
}

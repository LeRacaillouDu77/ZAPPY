/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** verif.c
*/

#include "../Include/server.h"

static const command_delay_t delay_map[] = {
    {"Look", 7.0, 0},
    {"Forward", 7.0, 0},
    {"Right", 7.0, 0},
    {"Left", 7.0, 0},
    {"Take ", 7.0, 1},
    {"Set ", 7.0, 1},
    {"Fork", 42.0, 0},
    {"Eject", 7.0, 0},
    {"Inventory", 1.0, 0},
    {"Connect_nbr", 1.0, 0},
    {"Player_nbr", 1.0, 0},
    {"Orientation", 1.0, 0},
    {"Player_pos", 1.0, 0},
    {"Broadcast ", 7.0, 1},
    {"Incantation", 300.0, 0},
    {NULL, 0.0, 0}
};

static const gui_command_t gui_commands[] = {
    {"bct ", 1, 1, handle_bct_gui, NULL},
    {"mct", 0, 0, NULL, handle_mct_command},
    {"tna", 0, 0, NULL, handle_tna_command},
    {"msz", 0, 0, NULL, handle_msz_command},
    {"pin ", 1, 1, handle_pin_gui, NULL},
    {"plv ", 1, 1, handle_plv_gui, NULL},
    {"sgt", 0, 0, NULL, handle_sgt_command},
    {"sst ", 1, 1, handle_sst_gui, NULL},
    {NULL, 0, 0, NULL, NULL}
};

static const char *valid_ai_commands[] = {
    "Forward",
    "Right",
    "Left",
    "Look",
    "Inventory",
    "Connect_nbr",
    "Player_nbr",
    "Player_pos",
    "Orientation",
    "Fork",
    "Eject",
    "Incantation",
    NULL
};

static const char *valid_ai_prefixes[] = {
    "Take ",
    "Set ",
    "Broadcast ",
    NULL
};

void execute_gui_command(server_t *server, int client_fd,
    const char *command, int i)
{
    if (gui_commands[i].needs_command) {
        gui_commands[i].handler_with_cmd(server, client_fd, command);
    } else {
        gui_commands[i].handler_no_cmd(server, client_fd);
    }
}

static int check_gui_command_match(const char *command, int i)
{
    if (gui_commands[i].is_prefix) {
        return strncmp(command, gui_commands[i].command,
            strlen(gui_commands[i].command)) == 0;
    } else {
        return strcmp(command, gui_commands[i].command) == 0;
    }
}

void handle_gui_commands(server_t *server, int client_fd, const char *command)
{
    for (int i = 0; gui_commands[i].command != NULL; i++) {
        if (check_gui_command_match(command, i)) {
            execute_gui_command(server, client_fd, command, i);
            return;
        }
    }
    print("Commande GUI inconnue: %s\n", command);
}

int is_valid_ai_command(const char *command)
{
    if (!command)
        return 0;
    for (int i = 0; valid_ai_commands[i] != NULL; i++) {
        if (strcmp(command, valid_ai_commands[i]) == 0)
            return 1;
    }
    for (int i = 0; valid_ai_prefixes[i] != NULL; i++) {
        if (strncmp(command, valid_ai_prefixes[i],
                strlen(valid_ai_prefixes[i])) == 0)
            return 1;
    }
    return 0;
}

static int check_delay_command_match(const char *command,
    const command_delay_t *map)
{
    if (map->is_prefix) {
        return strncmp(command, map->command, strlen(map->command)) == 0;
    }
    return strcmp(command, map->command) == 0;
}

double get_command_delay(const char *command, int freq)
{
    if (!command || freq <= 0)
        return 0.0;
    for (int i = 0; delay_map[i].command != NULL; i++) {
        if (check_delay_command_match(command, &delay_map[i]))
            return (delay_map[i].base_time / freq) * 1000.0;
    }
    return 0.0;
}

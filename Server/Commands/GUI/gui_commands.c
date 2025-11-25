/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** gui_commands.c
*/

#include "../../../Include/server.h"

void format_tile_message(char *message, int x, int y, tile_t *tile)
{
    snprintf(message, 256, "bct %d %d %d %d %d %d %d %d %d\n", x, y,
        tile->resources[0], tile->resources[1], tile->resources[2],
        tile->resources[3], tile->resources[4], tile->resources[5],
        tile->resources[6]
    );
}

void handle_bct_gui(server_t *server, int client_fd,
    const char *command)
{
    int x;
    int y;

    print("-> Commande BCT reçue du client GUI (fd=%d)\n", client_fd);
    if (sscanf(command, "bct %d %d", &x, &y) == 2) {
        handle_bct_command(server, client_fd, x, y);
    } else {
        send_data_to_client(client_fd, "sbp\n");
    }
}

void handle_pin_gui(server_t *server, int client_fd, const char *command)
{
    int player_id;

    print("-> Commande PIN reçue du client GUI (fd=%d): %s\n",
        client_fd, command);
    if (sscanf(command, "pin #%d", &player_id) == 1) {
        handle_pin_command(server, client_fd, player_id);
    } else {
        print("🚫 [GUI - PIN] Format de commande invalide: %s\n", command);
        send_data_to_client(client_fd, "sbp\n");
    }
}

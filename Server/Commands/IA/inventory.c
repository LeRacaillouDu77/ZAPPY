/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** inventory.c
*/
#include "../../../Include/server.h"

void load_inventory(player_info_t *player, char *response)
{
    sprintf(response,
        "[food: %d, linemate: %d, deraumere: %d, sibur: %d, "
        "mendiane: %d, phiras: %d, thystame: %d]\n",
        player->inventory.n_food,
        player->inventory.n_linemate,
        player->inventory.n_deraumere,
        player->inventory.n_sibur,
        player->inventory.n_mendiane,
        player->inventory.n_phiras,
        player->inventory.n_thystame
    );
}

void handle_inventory_command(server_t *server, int client_fd)
{
    player_info_t *player;
    char response[4096];

    player = find_player_by_fd(server, client_fd);
    if (!player) {
        send_data_to_client(client_fd, "ko\n");
        return;
    }
    load_inventory(player, response);
    print("🗄️ [Joueur %d - inventory] %s", client_fd, response);
    send_data_to_client(client_fd, response);
}

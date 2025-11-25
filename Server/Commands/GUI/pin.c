/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** pin.c
*/

#include "../../../Include/server.h"

static void send_pin(int client_fd, int player_id, player_info_t *player)
{
    char message[256];

    snprintf(message, sizeof(message), "pin #%d %d %d %d %d %d %d %d %d %d\n",
        player_id,
        player->x, player->y,
        player->inventory.n_food,
        player->inventory.n_linemate,
        player->inventory.n_deraumere,
        player->inventory.n_sibur,
        player->inventory.n_mendiane,
        player->inventory.n_phiras,
        player->inventory.n_thystame);
    send_data_to_client(client_fd, message);
    print("📌 [GUI - PIN] Envoi des infos du joueur #%d au GUIs (fd=%d)\n",
        player_id, client_fd);
}

void handle_pin_command(server_t *server, int client_fd, int player_id)
{
    player_info_t *player = NULL;

    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].id == player_id &&
            server->players[i].state == CLIENT_CONNECTED &&
            server->players[i].type == CLIENT_TYPE_AI) {
            player = &server->players[i];
            break;
        }
    }
    if (!player) {
        print("🚫 [GUI - PIN] Joueur #%d non trouvé\n", player_id);
        send_data_to_client(client_fd, "sbp\n");
        return;
    }
    send_pin(client_fd, player_id, player);
}

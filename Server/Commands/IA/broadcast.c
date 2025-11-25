/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** broadcast.c
*/

#include "../../../Include/server.h"
#include <math.h>

static void calcul_shortest_distance(int *dx, int *dy, int width, int height)
{
    if (abs(*dx) > width / 2) {
        *dx = *dx > 0 ? *dx - width : *dx + width;
    }
    if (abs(*dy) > height / 2) {
        *dy = *dy > 0 ? *dy - height : *dy + height;
    }
}

static int get_vertical_direction(int dy, int dx, int abs_dx, int abs_dy)
{
    int direction;

    if (dy < 0) {
        direction = 1;
    } else {
        direction = 5;
    }
    if (abs_dx > abs_dy / 3) {
        if (dx > 0) {
            direction = (direction == 1) ? 2 : 6;
        }
        if (dx < 0) {
            direction = (direction == 1) ? 8 : 4;
        }
    }
    return direction;
}

static int get_horizontal_direction(int dx, int dy, int abs_dx, int abs_dy)
{
    int direction;

    if (dx > 0) {
        direction = 3;
    } else {
        direction = 7;
    }
    if (abs_dy > abs_dx / 3) {
        if (dy < 0) {
            direction = (direction == 3) ? 2 : 8;
        }
        if (dy > 0) {
            direction = (direction == 3) ? 4 : 6;
        }
    }
    return direction;
}

static int adjust_direction(int direction, int orientation)
{
    direction = ((direction - orientation + 8) % 8);
    if (direction == 0) {
        direction = 8;
    }
    return direction;
}

int calculate_direction(server_t *server, player_info_t *sender,
    player_info_t *receiver)
{
    int abs_dx;
    int abs_dy;
    int direction;
    int dx = receiver->x - sender->x;
    int dy = receiver->y - sender->y;

    calcul_shortest_distance(&dx, &dy, server->width, server->height);
    if (dx == 0 && dy == 0) {
        return 0;
    }
    abs_dx = abs(dx);
    abs_dy = abs(dy);
    if (abs_dy > abs_dx) {
        direction = get_vertical_direction(dy, dx, abs_dx, abs_dy);
    } else {
        direction = get_horizontal_direction(dx, dy, abs_dx, abs_dy);
    }
    return adjust_direction(direction, receiver->orientation);
}

static void send_broadcast_to_player(server_t *server, player_info_t *sender,
    player_info_t *receiver, const char *message)
{
    char broadcast_message[MAX_MESSAGE_LENGTH];
    int direction = 0;

    if (sender->id == receiver->id) {
        return;
    }
    direction = calculate_direction(server, sender, receiver);
    snprintf(broadcast_message, sizeof(broadcast_message),
        "message %d, %s\n", direction, message);
    send_data_to_client(receiver->id, broadcast_message);
    print("📢 [Broadcast] Joueur %d -> Joueur %d (direction %d): %s\n",
        sender->id, receiver->id, direction, message);
}

static void send_broadcast_to_all_players(server_t *server,
    player_info_t *sender, const char *message)
{
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].type == CLIENT_TYPE_AI &&
            server->players[i].state == CLIENT_CONNECTED) {
            send_broadcast_to_player(server, sender,
                &server->players[i], message);
        }
    }
}

void handle_broadcast_command(server_t *server, int client_fd,
    const char *command)
{
    player_info_t *sender = find_player_by_fd(server, client_fd);
    char message[MAX_MESSAGE_LENGTH];
    char gui_message[MAX_MESSAGE_LENGTH + 50];

    if (!sender) {
        send_data_to_client(client_fd, "ko\n");
        return;
    }
    if (sscanf(command, "Broadcast %[^\n]", message) != 1) {
        print("🚫 [Joueur %d - Broadcast] Format de commande invalide: %s\n",
            client_fd, command);
        send_data_to_client(client_fd, "ko\n");
        return;
    }
    print("📢 [Joueur %d - Broadcast] Message: %s\n", client_fd, message);
    send_broadcast_to_all_players(server, sender, message);
    snprintf(gui_message, sizeof(gui_message),
        "pbc #%d %s\n", client_fd, message);
    send_to_gui(server, gui_message);
    send_data_to_client(client_fd, "ok\n");
}

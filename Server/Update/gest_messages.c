/*
** EPITECH PROJECT, 2024
** Jetpack
** File description:
** gest_messages.c
*/

#include "../Include/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int find_player_index(server_t *server, int player_id)
{
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].id == player_id) {
            return i;
        }
    }
    return -1;
}

void notify_new_player(server_t *server, int player_id)
{
    char message[512];
    int player_index = find_player_index(server, player_id);

    if (player_index == -1)
        return;
    snprintf(message, sizeof(message), "pnw #%d %d %d %d %d %s\n",
        player_id, server->players[player_index].x,
        server->players[player_index].y,
        server->players[player_index].orientation,
        server->players[player_index].level,
        server->players[player_index].team);
    print("📢 Notification d'un nouveau joueur\n");
    send_to_gui(server, message);
}

int get_clients_data(server_t *server, int client_index)
{
    char buffer[MAX_MESSAGE_LENGTH];
    ssize_t bytes;

    bytes = recv(server->clients[client_index].fd, buffer,
        sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
        return 0;
    }
    buffer[bytes] = '\0';
    handle_message(server, server->clients[client_index].fd, buffer);
    return 1;
}

static void process_message_part(server_t *server, int client_fd,
    const char *current_pos, size_t message_len)
{
    char clean_message[MAX_MESSAGE_LENGTH];

    if (message_len >= MAX_MESSAGE_LENGTH - 1)
        return;
    strncpy(clean_message, current_pos, message_len);
    clean_message[message_len] = '\0';
    if (strlen(clean_message) > 0) {
        process_single_message(server, client_fd, clean_message);
    }
}

static void process_remaining_message(server_t *server, int client_fd,
    const char *current_pos)
{
    char clean_message[MAX_MESSAGE_LENGTH];

    if (strlen(current_pos) > 0) {
        strncpy(clean_message, current_pos, MAX_MESSAGE_LENGTH - 1);
        clean_message[MAX_MESSAGE_LENGTH - 1] = '\0';
        process_single_message(server, client_fd, clean_message);
    }
}

void handle_message(server_t *server, int client_fd, const char *message)
{
    const char *current_pos;
    const char *newline_pos;
    size_t message_len;

    current_pos = message;
    while (*current_pos != '\0') {
        newline_pos = strchr(current_pos, '\n');
        if (newline_pos) {
            message_len = newline_pos - current_pos;
            process_message_part(server, client_fd, current_pos, message_len);
            current_pos = newline_pos + 1;
        } else {
            process_remaining_message(server, client_fd, current_pos);
            break;
        }
    }
}

static int find_client_index(server_t *server, int client_fd)
{
    for (int i = 1; i < server->nb_clients; i++) {
        if (server->clients[i].fd == client_fd) {
            return i;
        }
    }
    return -1;
}

static int find_player_by_fdd(server_t *server, int client_fd)
{
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].id == client_fd) {
            return i;
        }
    }
    return -1;
}

static void handle_client_commands(server_t *server, int client_fd,
    const char *clean_message, int player_index)
{
    if (server->players[player_index].state == CLIENT_PENDING) {
        handle_client_team_selection(server,
            find_client_index(server, client_fd), clean_message);
        return;
    }
    if (server->players[player_index].type == CLIENT_TYPE_GUI &&
        server->players[player_index].state == CLIENT_CONNECTED) {
        handle_gui_commands(server, client_fd, clean_message);
        return;
    }
    if (server->players[player_index].type == CLIENT_TYPE_AI &&
        server->players[player_index].state == CLIENT_CONNECTED) {
        handle_ai_commands(server, client_fd, clean_message);
        return;
    }
}

void process_single_message(server_t *server, int client_fd,
    const char *clean_message)
{
    int client_index;
    int player_index;

    client_index = find_client_index(server, client_fd);
    if (client_index == -1) {
        return;
    }
    player_index = find_player_by_fdd(server, client_fd);
    if (player_index == -1) {
        return;
    }
    handle_client_commands(server, client_fd, clean_message, player_index);
}

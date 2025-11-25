/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** send_data.c
*/

#include "../Include/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int handle_send_error(int client_fd)
{
    char error_msg[MAX_MESSAGE_LENGTH];

    if (errno == EAGAIN || errno == EWOULDBLOCK)
        return 0;
    snprintf(error_msg, MAX_MESSAGE_LENGTH,
        "Error sending to client %d: %s", client_fd, strerror(errno));
    fprintf(stderr, "%s", error_msg);
    return -1;
}

static int send_chunk_to_client(int client_fd, const char *message,
    size_t *total_sent, size_t len)
{
    ssize_t sent;

    sent = send(client_fd, message + *total_sent, len - *total_sent, 0);
    if (sent < 0) {
        return handle_send_error(client_fd);
    }
    *total_sent += sent;
    return 0;
}

void send_data_to_all_client(server_t *server, const char *message)
{
    if (!message)
        return;
    for (int i = 1; i < server->nb_clients; i++) {
        if (server->clients[i].fd != -1)
            send_data_to_client(server->clients[i].fd, message);
    }
}

void send_data_to_client(int client_fd, const char *message)
{
    size_t total_sent = 0;
    size_t message_len;

    if (!message || client_fd < 0)
        return;
    message_len = strlen(message);
    while (total_sent < message_len) {
        if (send_chunk_to_client(client_fd, message,
            &total_sent, message_len) == -1) {
            return;
        }
    }
}

void send_to_gui(server_t *server, const char *message)
{
    if (!message)
        return;
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].type == CLIENT_TYPE_GUI &&
            server->players[i].state == CLIENT_CONNECTED) {
            send_data_to_client(server->players[i].id, message);
        }
    }
}

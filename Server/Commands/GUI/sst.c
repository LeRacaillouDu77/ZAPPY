/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** sst.c
*/

#include "../../../Include/server.h"

void handle_sst_gui(server_t *server, int client_fd, const char *command)
{
    char response[64];
    int new_freq;

    if (sscanf(command, "sst %d", &new_freq) != 1) {
        send_data_to_client(client_fd, "sbp\n");
        return;
    }
    if (new_freq <= 0) {
        send_data_to_client(client_fd, "sbp\n");
        return;
    }
    server->freq = new_freq;
    snprintf(response, sizeof(response), "sst %d\n", server->freq);
    send_data_to_client(client_fd, response);
    print("⏱️  Fréquence temporelle modifiée par le GUI: %d\n", new_freq);
}

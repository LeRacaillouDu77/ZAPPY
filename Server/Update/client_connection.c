/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** client_connection.c
*/

#include "../Include/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int find_player_by_client_fd(server_t *server, int client_fd)
{
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].id == client_fd) {
            return i;
        }
    }
    return -1;
}

static void handle_team_rejection(server_t *server, int client_fd,
    int client_index, const char *message)
{
    print("🚫 Connexion refusée: équipe '%s' inexistante\n", message);
    send_data_to_client(client_fd, "ko\n");
    disconnect_client(server, client_index, "Team does not exist");
}

static void handle_team_full(server_t *server, int client_fd, int client_index,
    const char *message)
{
    print("🚫 Connexion refusée: équipe '%s' pleine\n", message);
    send_data_to_client(client_fd, "ko\n");
    disconnect_client(server, client_index, "No slots available");
}

static void assign_player_to_team(server_t *server, int player_index,
    int team_index, int client_fd)
{
    server->players[player_index].team[MAX_NAME_LENGTH - 1] = '\0';
    server->teams[team_index].current_clients++;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (server->teams[team_index].player_ids[i] == -1) {
            server->teams[team_index].player_ids[i] = client_fd;
            break;
        }
    }
}

static void send_connection_response(server_t *server, int team_index,
    int client_fd)
{
    int available_eggs = 0;
    char client_num_msg[64];
    char client_id_msg[64];
    char map_size[64];

    for (int i = 0; i < server->clients_per_team; i++) {
        if (server->teams[team_index].eggs[i].is_active) {
            available_eggs++;
        }
    }
    snprintf(client_num_msg, sizeof(client_num_msg), "%d\n", available_eggs);
    send_data_to_client(client_fd, client_num_msg);
    snprintf(client_id_msg, sizeof(client_id_msg), "%d\n", client_fd);
    send_data_to_client(client_fd, client_id_msg);
    snprintf(map_size, sizeof(map_size), "%d %d\n",
        server->width, server->height);
    send_data_to_client(client_fd, map_size);
}

static void finalize_player_connection(server_t *server, int player_index,
    int client_fd, const char *message)
{
    int egg_id = find_available_egg(server, message);

    spawn_player_from_egg(server, client_fd, egg_id, message);
    server->players[player_index].state = CLIENT_CONNECTED;
    notify_new_player(server, client_fd);
}

static int find_client_index_by_fd(server_t *server, int client_fd)
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (server->clients[i].fd == client_fd) {
            return i;
        }
    }
    return -1;
}

static void process_team_connection(server_t *server,
    connection_params_t *params, const char *message)
{
    int egg_id = find_available_egg(server, message);
    int client_index = find_client_index_by_fd(server, params->client_fd);

    if (egg_id == -1) {
        print("🚫 Connexion refusée: pas d'œuf dispo pour l'équipe '%s'\n",
            message);
        send_data_to_client(params->client_fd, "ko\n");
        disconnect_client(server, client_index, "No eggs available");
        return;
    }
    strncpy(server->players[params->player_index].team, message,
        MAX_NAME_LENGTH - 1);
    assign_player_to_team(server, params->player_index, params->team_index,
        params->client_fd);
    send_connection_response(server, params->team_index, params->client_fd);
    finalize_player_connection(server, params->player_index,
        params->client_fd, message);
}

void handle_client_team_selection(server_t *server, int client_index,
    const char *message)
{
    int client_fd = server->clients[client_index].fd;
    int player_index = find_player_by_client_fd(server, client_fd);
    int team_index;
    connection_params_t params;

    if (player_index == -1)
        return;
    if (strcmp(message, "GRAPHIC") == 0)
        return handle_gui_connection(server, client_index);
    server->players[player_index].type = CLIENT_TYPE_AI;
    team_index = find_team(server, message);
    if (team_index == -1)
        return handle_team_rejection(server, client_fd, client_index, message);
    if (!team_has_slot(server, message))
        return handle_team_full(server, client_fd, client_index, message);
    params = (connection_params_t){player_index, team_index, client_fd};
    process_team_connection(server, &params, message);
}

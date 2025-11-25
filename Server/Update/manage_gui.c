/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** manage_gui.c
*/

#include "../Include/server.h"

static void send_single_egg_info(int client_fd, Egg_t *egg)
{
    char message[256];

    snprintf(message, sizeof(message), "enw #%d #-1 %d %d\n",
        egg->id, egg->x, egg->y);
    send_data_to_client(client_fd, message);
}

static void send_team_eggs_to_gui(server_t *server, int client_fd, int team)
{
    for (int i = 0; i < server->clients_per_team; i++) {
        if (server->teams[team].eggs[i].is_active) {
            send_single_egg_info(client_fd, &server->teams[team].eggs[i]);
        }
    }
}

static void send_all_eggs_to_gui(server_t *server, int client_fd)
{
    for (int team = 0; team < server->nb_teams; team++) {
        send_team_eggs_to_gui(server, client_fd, team);
    }
}

static void configure_gui_player(server_t *server, int client_fd)
{
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].id == client_fd) {
            server->players[i].type = CLIENT_TYPE_GUI;
            server->players[i].state = CLIENT_CONNECTED;
            strcpy(server->players[i].team, "GRAPHIC");
            print("✅ Client GUI configuré (fd=%d)\n", client_fd);
            break;
        }
    }
}

void handle_gui_connection(server_t *server, int client_index)
{
    int client_fd;

    client_fd = server->clients[client_index].fd;
    print("🖥️  Connexion GUI détectée (fd=%d)\n", client_fd);
    print("📋 Configuration du client GUI (fd=%d)...\n", client_fd);
    configure_gui_player(server, client_fd);
    send_gui_initial_state(server, client_fd);
    print("🎮 GUI prêt à afficher le jeu (fd=%d)\n", client_fd);
}

void send_gui_initial_state(server_t *server, int client_fd)
{
    char message[1024];

    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].state == CLIENT_CONNECTED &&
            server->players[i].type == CLIENT_TYPE_AI) {
            snprintf(message, sizeof(message), "pnw #%d %d %d %d %d %s\n",
                server->players[i].id, server->players[i].x,
                server->players[i].y, server->players[i].orientation,
                server->players[i].level, server->players[i].team);
            send_data_to_client(client_fd, message);
        }
    }
    send_all_eggs_to_gui(server, client_fd);
}

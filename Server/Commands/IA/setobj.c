/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** setobj.c
*/

#include "../../../Include/server.h"

static int get_resource_type_from_name(const char *resource_name)
{
    for (int i = 0; i < MAX_RESOURCE_TYPES; i++) {
        if (strcmp(resource_name, resource_names[i]) == 0) {
            return i;
        }
    }
    return -1;
}

void remove_resource_from_inventory(player_info_t *player, int resource_type)
{
    int *inventory_ptr[] = {
        &player->inventory.n_food, &player->inventory.n_linemate,
        &player->inventory.n_deraumere, &player->inventory.n_sibur,
        &player->inventory.n_mendiane, &player->inventory.n_phiras,
        &player->inventory.n_thystame
    };

    if (resource_type >= 0 && resource_type < MAX_RESOURCE_TYPES) {
        (*inventory_ptr[resource_type])--;
    }
}

int get_inventory_resource_count(player_info_t *player, int resource_type)
{
    int *inventory_ptr[] = {
        &player->inventory.n_food, &player->inventory.n_linemate,
        &player->inventory.n_deraumere, &player->inventory.n_sibur,
        &player->inventory.n_mendiane, &player->inventory.n_phiras,
        &player->inventory.n_thystame
    };

    if (resource_type >= 0 && resource_type < MAX_RESOURCE_TYPES)
        return *inventory_ptr[resource_type];
    return 0;
}

static int validate_set_command(server_t *server, int client_fd,
    player_info_t **player, tile_t **tile)
{
    *player = find_player_by_fd(server, client_fd);
    if (!*player) {
        send_data_to_client(client_fd, "ko\n");
        return 0;
    }
    *tile = world_get_tile(server->world, (*player)->x, (*player)->y);
    if (!*tile) {
        send_data_to_client(client_fd, "ko\n");
        return 0;
    }
    return 1;
}

static int parse_resource_command(int client_fd, const char *command,
    char *resource_name)
{
    if (sscanf(command, "Set %63s", resource_name) != 1) {
        print("🚫 [Joueur %d - Set] Format de commande invalide: %s\n",
            client_fd, command);
        send_data_to_client(client_fd, "ko\n");
        return 0;
    }
    return 1;
}

static int validate_resource_availability(int client_fd, player_info_t *player,
    int resource_type, const char *resource_name)
{
    if (resource_type == -1) {
        print("🚫 [Joueur %d - Set] Ressource inconnue: %s\n",
            client_fd, resource_name);
        send_data_to_client(client_fd, "ko\n");
        return 0;
    }
    if (get_inventory_resource_count(player, resource_type) <= 0) {
        print("🚫 [Joueur %d - Set] Ressource %s non disponible\n",
            client_fd, resource_name);
        send_data_to_client(client_fd, "ko\n");
        return 0;
    }
    return 1;
}

static void send_gui_notifications(gui_takeget_data_t *data)
{
    char message[256];

    snprintf(message, sizeof(message), "pdr #%d %d\n",
        data->client_fd, data->resource_type);
    send_to_gui(data->server, message);
    snprintf(message, sizeof(message), "bct %d %d %d %d %d %d %d %d %d\n",
        data->player->x, data->player->y, data->tile->resources[0],
        data->tile->resources[1], data->tile->resources[2],
        data->tile->resources[3], data->tile->resources[4],
        data->tile->resources[5], data->tile->resources[6]);
    send_to_gui(data->server, message);
}

void handle_set_object_command(server_t *server, int client_fd,
    const char *command)
{
    player_info_t *player;
    tile_t *tile;
    int ress;
    char ress_name[64];
    gui_takeget_data_t gui_data;

    if (!validate_set_command(server, client_fd, &player, &tile))
        return;
    if (!parse_resource_command(client_fd, command, ress_name))
        return;
    ress = get_resource_type_from_name(ress_name);
    if (!validate_resource_availability(client_fd, player, ress, ress_name))
        return;
    remove_resource_from_inventory(player, ress);
    tile->resources[ress]++;
    server->world->total_resources[ress]++;
    print("📤 [Joueur %d - Set] Dépose %s\n", client_fd, ress_name);
    gui_data = (gui_takeget_data_t){server, player, tile, client_fd, ress};
    send_gui_notifications(&gui_data);
    send_data_to_client(client_fd, "ok\n");
}

/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** takeobj.c
*/

#include "../../../Include/server.h"

static int get_resource_type_from_name(const char *resource_name)
{
    for (int i = 0; i < MAX_RESOURCE_TYPES; i++) {
        if (strcmp(resource_name, resource_names[i]) == 0)
            return i;
    }
    return -1;
}

static void add_resource_to_inventory(player_info_t *player, int resource_type)
{
    int *inventory_ptr[] = {
        &player->inventory.n_food, &player->inventory.n_linemate,
        &player->inventory.n_deraumere, &player->inventory.n_sibur,
        &player->inventory.n_mendiane, &player->inventory.n_phiras,
        &player->inventory.n_thystame
    };

    if (resource_type >= 0 && resource_type < MAX_RESOURCE_TYPES)
        (*inventory_ptr[resource_type])++;
}

static int validate_take_command(server_t *server, int client_fd,
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

static int parse_take_command(int client_fd, const char *command,
    char *resource_name)
{
    if (sscanf(command, "Take %63s", resource_name) != 1) {
        print("🚫 [Joueur %d - Take] Format de commande invalide: %s\n",
            client_fd, command);
        send_data_to_client(client_fd, "ko\n");
        return 0;
    }
    return 1;
}

static int validate_resource_on_tile(int client_fd, tile_t *tile,
    int resource_type, player_info_t *player)
{
    if (resource_type == -1) {
        print("🚫 [Joueur %d - Take] Ressource inconnue\n",
            client_fd);
        send_data_to_client(client_fd, "ko\n");
        return 0;
    }
    if (tile->resources[resource_type] <= 0) {
        print("🚫 [Joueur %d - Take] Ressource non disponible à (%d,%d)\n",
            client_fd, player->x, player->y);
        send_data_to_client(client_fd, "ko\n");
        return 0;
    }
    return 1;
}

static void send_take_gui_notifications(gui_takeget_data_t *data)
{
    char message[256];

    snprintf(message, sizeof(message), "pgt #%d %d\n",
        data->client_fd, data->resource_type);
    send_to_gui(data->server, message);
    snprintf(message, sizeof(message), "bct %d %d %d %d %d %d %d %d %d\n",
        data->player->x, data->player->y, data->tile->resources[0],
        data->tile->resources[1], data->tile->resources[2],
        data->tile->resources[3], data->tile->resources[4],
        data->tile->resources[5], data->tile->resources[6]);
    send_to_gui(data->server, message);
}

static void execute_resource_take(gui_takeget_data_t *data)
{
    data->tile->resources[data->resource_type]--;
    data->server->world->total_resources[data->resource_type]--;
    add_resource_to_inventory(data->player, data->resource_type);
    if (data->resource_type == RESOURCE_FOOD) {
        add_food_to_survival(data->server, data->player->id, 1);
    }
    send_take_gui_notifications(data);
    send_data_to_client(data->client_fd, "ok\n");
}

void handle_take_object_command(server_t *server, int client_fd,
    const char *command)
{
    player_info_t *player;
    tile_t *tile;
    int ress_type;
    char resource_name[64];
    gui_takeget_data_t data;

    if (!validate_take_command(server, client_fd, &player, &tile))
        return;
    if (!parse_take_command(client_fd, command, resource_name))
        return;
    ress_type = get_resource_type_from_name(resource_name);
    if (!validate_resource_on_tile(client_fd, tile, ress_type, player))
        return;
    data = (gui_takeget_data_t){server, player, tile, client_fd, ress_type};
    print("📦 [Joueur %d - Take] Collecte %s\n", client_fd, resource_name);
    execute_resource_take(&data);
}

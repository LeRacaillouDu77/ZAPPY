/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** verif_incant.c
*/

#include "../../../Include/server.h"

const int level_requirements[8][7] = {
    {1, 1, 0, 0, 0, 0, 0}, // Niveau 1 → 2
    {2, 1, 1, 1, 0, 0, 0}, // Niveau 2 → 3
    {2, 2, 0, 1, 0, 2, 0}, // Niveau 3 → 4
    {4, 1, 1, 2, 0, 1, 0}, // Niveau 4 → 5
    {4, 1, 2, 1, 3, 0, 0}, // Niveau 5 → 6
    {6, 1, 2, 3, 0, 1, 0}, // Niveau 6 → 7
    {6, 2, 2, 2, 2, 2, 1}, // Niveau 7 → 8
    {0, 0, 0, 0, 0, 0, 0}  // Niveau 8 (maximum)
};

static int count_players_at_level_and_position(server_t *server,
    incantation_context_t context)
{
    int count = 0;

    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].type == CLIENT_TYPE_AI &&
            server->players[i].state == CLIENT_CONNECTED &&
            server->players[i].x == context.x &&
            server->players[i].y == context.y &&
            server->players[i].level == context.level) {
            count++;
        }
    }
    return count;
}

static int check_resource_requirements(server_t *server,
    incantation_context_t context)
{
    tile_t *tile = world_get_tile(server->world, context.x, context.y);
    const int *req = level_requirements[context.level - 1];

    print("🏗️ [Incantation] Ressources sur la tuile (%d,%d): "
            "food=%d, linemate=%d, deraumere=%d, sibur=%d,"
            "mendiane=%d, phiras=%d, thystame=%d\n",
            context.x, context.y, tile->resources[0],
            tile->resources[1], tile->resources[2],
            tile->resources[3], tile->resources[4],
            tile->resources[5], tile->resources[6]);
    for (int i = 1; i < 7; i++) {
        if (tile->resources[i] < req[i]) {
            print("🚫 [Incantation] Ressource %d"
                "insuffisante sur la tuile: %d/%d\n",
                i, tile->resources[i], req[i]);
            return 0;
        }
    }
    print("✅ [Incantation] Ressources suffisantes sur la tuile\n");
    return 1;
}

int verify_incantation_conditions(server_t *server, player_info_t *initiator)
{
    incantation_context_t context = {
        initiator->x, initiator->y, initiator->level
    };
    const int *req = level_requirements[context.level - 1];
    int players_present = count_players_at_level_and_position(server, context);

    if (context.level < 1 || context.level > 7)
        return 0;
    print("🔍 [Incantation] Vérification niveau %d: %d/%d joueurs présents\n",
        context.level, players_present, req[0]);
    if (players_present < req[0]) {
        print("🚫 [Incantation] Pas assez de joueurs niveau %d (%d/%d)\n",
            context.level, players_present, req[0]);
        return 0;
    }
    if (!check_resource_requirements(server, context))
        return 0;
    print("✅ [Incantation] Conditions remplies pour niveau %d→%d\n",
        context.level, context.level + 1);
    return 1;
}

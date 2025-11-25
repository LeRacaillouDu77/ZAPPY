/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** look.c
*/

#include "../../../Include/server.h"

static const direction_vectors_t orientation_vectors[] = {
    {0, 0, 0, 0},      // Index 0
    {0, -1, 1, 0},     // Nord
    {1, 0, 0, 1},      // Est
    {0, 1, -1, 0},     // Sud
    {-1, 0, 0, -1}     // Ouest
};

static direction_vectors_t get_direction_vectors(int orientation)
{
    if (orientation < 1 || orientation > 4)
        return orientation_vectors[0];
    return orientation_vectors[orientation];
}

static void normalize_tile_coords(server_t *server, int *tile_x, int *tile_y)
{
    *tile_x = ((*tile_x % server->width) + server->width) % server->width;
    *tile_y = ((*tile_y % server->height) + server->height) % server->height;
}

static void add_tile_to_response(tile_response_data_t *data,
    int tile_x, int tile_y)
{
    char *tile_content;

    tile_content = generate_tile_content(data->server, tile_x, tile_y);
    if (!*(data->first_tile)) {
        strcat(data->response, ",");
    }
    *(data->first_tile) = 0;
    if (tile_content && strlen(tile_content) > 0) {
        strcat(data->response, tile_content);
    }
}

static void process_vision_line(tile_response_data_t *data,
    player_info_t *player, direction_vectors_t vectors, int distance)
{
    int half_width;
    int offset;
    int tile_x;
    int tile_y;

    half_width = distance;
    for (offset = -half_width; offset <= half_width; offset++) {
        tile_x = player->x + distance * vectors.front_dx +
            offset * vectors.right_dx;
        tile_y = player->y + distance * vectors.front_dy +
            offset * vectors.right_dy;
        normalize_tile_coords(data->server, &tile_x, &tile_y);
        add_tile_to_response(data, tile_x, tile_y);
    }
}

static void build_vision_response(server_t *server, player_info_t *player,
    char *response)
{
    int first_tile;
    int vision_range;
    direction_vectors_t vectors;
    tile_response_data_t data;
    int distance;

    first_tile = 1;
    vision_range = player->level;
    vectors = get_direction_vectors(player->orientation);
    data.server = server;
    data.response = response;
    data.first_tile = &first_tile;
    for (distance = 0; distance <= vision_range; distance++) {
        process_vision_line(&data, player, vectors, distance);
    }
}

void handle_look_command(server_t *server, int client_fd)
{
    player_info_t *player;
    char response[8192];

    player = find_player_by_fd(server, client_fd);
    if (!player) {
        send_data_to_client(client_fd, "ko\n");
        return;
    }
    strcpy(response, "[");
    build_vision_response(server, player, response);
    strcat(response, "]\n");
    print("🔍 Joueur %d (niv %d, ori %d, pos %d %d):\n%s", client_fd,
        player->level, player->orientation, player->x, player->y, response);
    send_data_to_client(client_fd, response);
}

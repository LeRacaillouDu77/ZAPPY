/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** command_queue.c
*/

#include "../../Include/server.h"
#include <time.h>
#include <sys/time.h>

void init_command_queue(CommandQueue_t *queue)
{
    if (!queue)
        return;
    queue->head = NULL;
    queue->tail = NULL;
    queue->count = 0;
    queue->current_execution_time = 0.0;
    queue->is_executing = 0;
}

void destroy_command_queue(CommandQueue_t *queue)
{
    command_node_t *current = NULL;
    command_node_t *next = NULL;

    if (!queue)
        return;
    current = queue->head;
    while (current) {
        next = current->next;
        free(current);
        current = next;
    }
    current = NULL;
    init_command_queue(queue);
}

int add_command_to_queue(CommandQueue_t *queue, const char *command,
    double execution_time)
{
    command_node_t *new_node;

    if (!queue || !command)
        return 0;
    if (queue->count >= MAX_QUEUE_SIZE)
        return 0;
    new_node = malloc(sizeof(command_node_t));
    if (!new_node)
        return 0;
    strncpy(new_node->command, command, MAX_NAME_LENGTH - 1);
    new_node->command[MAX_NAME_LENGTH - 1] = '\0';
    new_node->execution_time = execution_time;
    new_node->next = NULL;
    if (queue->tail)
        queue->tail->next = new_node;
    else
        queue->head = new_node;
    queue->tail = new_node;
    queue->count++;
    return 1;
}

static command_node_t *get_next_command(CommandQueue_t *queue)
{
    if (!queue)
        return NULL;
    return queue->head;
}

void remove_command_from_queue(CommandQueue_t *queue)
{
    command_node_t *to_remove = NULL;

    if (!queue || !queue->head)
        return;
    to_remove = queue->head;
    queue->head = queue->head->next;
    if (!queue->head) {
        queue->tail = NULL;
    }
    free(to_remove);
    queue->count--;
}

static void start_command_execution(server_t *server, player_info_t *player,
    CommandQueue_t *queue, double current_time)
{
    command_node_t *next_cmd;
    double execution_end_time = 0.0;
    char gui_message[MAX_MESSAGE_LENGTH];

    next_cmd = get_next_command(queue);
    if (next_cmd) {
        execution_end_time = current_time + next_cmd->execution_time;
        queue->current_execution_time = execution_end_time;
        queue->is_executing = 1;
        print("🚀 Démarrage de la commande '%s' (délai: %.1fms)\n",
            next_cmd->command, next_cmd->execution_time);
        if (strcmp(next_cmd->command, "Incantation") == 0) {
            snprintf(gui_message, sizeof(gui_message), "pic %d %d %d #%d\n",
                player->x, player->y, player->level, player->id);
            send_to_gui(server, gui_message);
        }
    }
}

static void process_player_queue(server_t *server, player_info_t *player,
    double current_time)
{
    CommandQueue_t *queue;
    command_node_t *cmd_to_execute;

    queue = &player->command_queue;
    if (!queue->is_executing && queue->head) {
        start_command_execution(server, player, queue, current_time);
    }
    if (queue->is_executing && current_time >= queue->current_execution_time) {
        cmd_to_execute = get_next_command(queue);
        if (cmd_to_execute) {
            print("✅ Exécution de la commande '%s' pour le joueur %d\n",
                cmd_to_execute->command, player->id);
            execute_ai_command(server, player->id, cmd_to_execute->command);
            remove_command_from_queue(queue);
            queue->is_executing = 0;
        }
    }
}

void process_command_queues(server_t *server)
{
    double current_time;
    struct timeval tv;

    if (!server)
        return;
    gettimeofday(&tv, NULL);
    current_time = (tv.tv_sec * 1000.0) + (tv.tv_usec / 1000.0);
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].type == CLIENT_TYPE_AI &&
            server->players[i].state == CLIENT_CONNECTED) {
            process_player_queue(server, &server->players[i], current_time);
        }
    }
}

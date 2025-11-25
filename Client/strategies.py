#!/usr/bin/env python3
#
# EPITECH PROJECT, 2024
# Zappy
# File description:
# strategies.py
#
import time
import os
import sys
import json
from parser import parse_look
from manage_level_up import LEVEL_REQUIREMENTS
color = "\033[31m"
color2 = "\033[32m"
color3 = "\033[33m"
color4 = "\033[34m"
reset = "\033[0m"

def enough_resources(bot):
    if bot.inventory.get(bot.assigned_resource, 0) >= bot.total_to_collect:
        return True
    return False

def parse_position(position):
    try:
        pos_x_str, pos_y_str = position.split(',')
        pos_x = int(pos_x_str.strip())
        pos_y = int(pos_y_str.strip())
        return pos_x, pos_y
    except Exception as e:
        print(f"⚠️ [Parser] Format Message (position) invalide: {position} ({e})")
        return None, None

def parse_broadcast(broadcast):
    broadcast_message = broadcast[11:]
    if not (broadcast_message.startswith('{') and broadcast_message.endswith('}')):
        print(f"⚠️ [Parser] Format Broadcast invalide: [{broadcast}]\n")
        return ""
    return json.loads(broadcast_message)

def receive_message(bot):
    message = {}
    incoming = bot.conn.get_next_broadcast(timeout=0.1)
    if incoming:
        message = parse_broadcast(incoming)
        if message["move"]:
            bot.check_move = True
        else:
            bot.check_move = False
    else:
        message = {"team": bot.conn.team_name, "move": True}

    print(f"{color2}📢 [{bot.conn.id} - Broadcast] {message}{reset}")
    return message

def fill_message(bot, move):
    message = {
        "team": bot.conn.team_name,
        "move": move
    }
    return json.dumps(message)

def orient_bot_towards(bot, target_orientation):
    while bot.orientation != target_orientation:
        if (bot.orientation % 4) + 1 == target_orientation:
            move = "Right"
        else:
            move = "Left"
        resp = bot.conn.send_and_receive(move)
        if resp.strip() == "ok":
            bot.orientation = target_orientation if move == "Right" and (bot.orientation % 4) + 1 == target_orientation else (
                4 if bot.orientation == 1 else bot.orientation - 1 if move == "Left" else (bot.orientation + 1 if bot.orientation < 4 else 1)
            )
            print(f"↻ [{bot.conn.id}] Orientation maintenant: {bot.orientation}")
        else:
            print(f"❌ [{bot.conn.id}] Échec rotation {move}")
            break

def go_to_meeting_point(bot):
    dest_x = bot.meet_pos
    dest_y = bot.meet_pos

    while bot.pos_x != dest_x or bot.pos_y != dest_y:
        print(f"📍 [{bot.conn.id}] En route vers ({bot.pos_x},{bot.pos_y}) -> ({dest_x},{dest_y})")

        dx = dest_x - bot.pos_x
        dy = dest_y - bot.pos_y

        if dx != 0:
            target_orientation = 2 if dx > 0 else 4  # Est ou Ouest
            orient_bot_towards(bot, target_orientation)
            move = "Forward"
        elif dy != 0:
            target_orientation = 3 if dy > 0 else 1  # Sud ou Nord
            orient_bot_towards(bot, target_orientation)
            move = "Forward"
        else:
            break  # déjà arrivé

        resp = bot.conn.send_and_receive(move)
        if resp.strip() == "ok":
            if bot.orientation == 1:
                bot.pos_y -= 1
            elif bot.orientation == 2:
                bot.pos_x += 1
            elif bot.orientation == 3:
                bot.pos_y += 1
            elif bot.orientation == 4:
                bot.pos_x -= 1
            # print(f"✅ [{bot.conn.id}] Move Forward -> ({bot.pos_x},{bot.pos_y}) [ori {bot.orientation}]")
        else:
            print(f"❌ [{bot.conn.id}] Move Forward failed: {resp}")
            break

def drop_all_items(bot):
    for _ in range(bot.inventory.get(bot.assigned_resource, 0)):
        bot.conn.send_and_receive(f"Set {bot.assigned_resource}")
    return


def start_incantation(bot):
    look_resp = bot.conn.send_and_receive("Look")
    vision = parse_look(look_resp)
    tile = vision[0]

    # print(f"👀 [{bot.conn.id}] Vision actuelle Tile1: {tile}")

    resource_counts = {}
    for item in tile:
        resource_counts[item] = resource_counts.get(item, 0) + 1

    needed_items = LEVEL_REQUIREMENTS[bot.level]["resources"]

    for resource, amount_required in needed_items.items():
        count_on_tile = resource_counts.get(resource, 0)
        if count_on_tile < amount_required:
            print(f"⛔️ [{bot.conn.id}] Ressource manquante: {resource} ({count_on_tile}/{amount_required})")
            return

    bot.conn.send_and_receive(f"Broadcast {fill_message(bot, move=False)}")
    bot.conn.send_and_receive("Take food")
    bot.conn.send_and_receive("Take food")
    bot.conn.send_and_receive("Take food")
    look_resp = bot.conn.send_and_receive("Look")
    vision = parse_look(look_resp)
    players_on_tile = vision[0].count("player")
    # print(f"👥 [{bot.conn.id}] Joueurs sur case: {players_on_tile}")

    if players_on_tile < 6:
        print(f"⛔️ [{bot.conn.id}] Pas assez de joueurs pour l'incantation ({players_on_tile}).")
        bot.conn.send_and_receive(f"Broadcast {fill_message(bot, move=True)}")
        return

    bot.conn.send_and_receive("Take food")
    bot.conn.send_and_receive("Take food")
    print(f"✨ [{bot.conn.id}] Conditions remplies, incantation lancée!")
    resp = bot.conn.send_and_receive("Incantation")
    print(f"🔮 [{bot.conn.id}] Réponse incantation: {resp}")
    time.sleep(0.5)
    bot.conn.send_and_receive(f"Broadcast {fill_message(bot, move=True)}")

def incantation(bot):
    if bot.pos_x != bot.meet_pos or bot.pos_y != bot.meet_pos:
        go_to_meeting_point(bot)
        return
    drop_all_items(bot)
    if bot.conn.is_og:
        print(f"🪄  {color4}[{bot.conn.id} - Bot] Sur la case de réunion, prêt pour incantation!{reset}")
        start_incantation(bot)


# def test(bot):
#     message = {}
#     incoming = bot.conn.get_next_broadcast(timeout=0.1)
#     if incoming:
#         message = parse_broadcast(incoming)
#         print(f"📢 [{bot.conn.id} - Broadcast] {color2}{message}{reset}")

#     bot.conn.send_and_receive(f"Broadcast {json.dumps(fill_message(bot, message))}")
#     return False

def spawn(bot):
    if bot.conn.is_og:
        while int(bot.conn.send_and_receive("Player_nbr")) < 6:
            bot.conn.send_and_receive("Fork")
            pid = os.fork()
            if pid == 0:
                os.execv(sys.argv[0], sys.argv + ['-i'])
            else:
                print(f"👶 [{bot.conn.id} - Bot] Nouveau client  à rejoint")
            time.sleep(0.5)
        print(f"🎉 [{bot.conn.id} - Bot] Tous les {bot.conn.send_and_receive('Player_nbr')} clients sont là")
    return

#!/usr/bin/env python3
#
# EPITECH PROJECT, 2024
# Zappy
# File description:
# manage_collect.py
#
import random
from parser import parse_look, parse_inventory
color5 = "\033[35m"
reset = "\033[0m"

# === Fonctions de scan ===
def scan_for_item(bot, item):
    look_resp = bot.conn.send_and_receive("Look")
    vision = parse_look(look_resp)
    if not vision:
        print(f"⚠️ [{bot.conn.id} - Collect] Vision vide")
        return []

    if vision[0].count("player") > 1 and bot.pos_x % 2 == 0 and bot.pos_x != bot.meet_pos:
        eject = bot.conn.send_and_receive("Eject")
        print(f"💥 [{bot.conn.id} - Collect] Éjection d'un joueur: {eject}")

    item_positions = []
    for idx, tile_content in enumerate(vision):
        if item in tile_content:
            count = tile_content.count(item)
            item_positions.append({
                'index': idx,
                'count': count
            })
            # print(f"👁️  [{bot.conn.id} - Collect] {count} {item} trouvé(s) sur case {idx}")
    return item_positions

# === Collecte sur la case courante ===
def collect_item_on_tile(bot, item, count):
    print(f"🎯 [{bot.conn.id} - Collect] Tentative de collecte {item}")
    collected = 0
    for _ in range(count):
        resp = bot.conn.send_and_receive(f"Take {item}")
        if resp.strip() == "ok":
            collected += 1
            print(f"✅ [{bot.conn.id} - Collect] {item} collecté ! Total: {collected}")
        else:
            print(f"❌ [{bot.conn.id} - Collect] Échec: {resp}")
            break
    return collected

# === Exploration aléatoire améliorée ===
def explore_random(bot):
    import random
    
    # Initialiser le compteur d'exploration s'il n'existe pas
    if not hasattr(bot, 'exploration_counter'):
        bot.exploration_counter = 0
    if not hasattr(bot, 'last_exploration_direction'):
        bot.last_exploration_direction = None
    
    bot.exploration_counter += 1
    
    # Éviter de répéter le même pattern
    directions = ["Forward", "Right", "Left"]
    
    # Si on a exploré plusieurs fois sans succès, changer de stratégie
    if bot.exploration_counter > 3:
        # Pattern spiral pour éviter les boucles
        if bot.exploration_counter % 4 == 0:
            pattern = ["Right", "Forward", "Forward"]
        elif bot.exploration_counter % 4 == 1:
            pattern = ["Left", "Forward", "Forward", "Forward"]
        elif bot.exploration_counter % 4 == 2:
            pattern = ["Left", "Left", "Forward", "Forward"]
        else:
            pattern = ["Right", "Right", "Forward", "Forward", "Forward"]
    else:
        # Exploration simple au début
        pattern = ["Forward", "Forward", "Right", "Forward"]
    
    print(f"🗺️  [{bot.conn.id} - Collect] Exploration pattern {bot.exploration_counter}: {pattern}")
    
    for move in pattern:
        resp = bot.conn.send_and_receive(move)
        if resp.strip() == "ok":
            print(f"🗺️  [{bot.conn.id} - Collect] Déplacement {move}")
            # Mettre à jour la position du bot
            if move == "Forward":
                if bot.orientation == 1:
                    bot.pos_y -= 1
                elif bot.orientation == 2:
                    bot.pos_x += 1
                elif bot.orientation == 3:
                    bot.pos_y += 1
                elif bot.orientation == 4:
                    bot.pos_x -= 1
            elif move == "Right":
                bot.orientation = bot.orientation % 4 + 1
            elif move == "Left":
                bot.orientation = 4 if bot.orientation == 1 else bot.orientation - 1
        else:
            print(f"❌ [{bot.conn.id} - Collect] Échec {move}")
            # Si le mouvement échoue, essayer une autre direction
            alternative = random.choice(["Right", "Left"])
            alt_resp = bot.conn.send_and_receive(alternative)
            if alt_resp.strip() == "ok":
                if alternative == "Right":
                    bot.orientation = bot.orientation % 4 + 1
                else:
                    bot.orientation = 4 if bot.orientation == 1 else bot.orientation - 1

# === Fonction principale améliorée ===
def manage_collect(bot, item=None, is_critical=False):
    # Par défaut on collecte la ressource assignée
    target_item = item if item else bot.assigned_resource
    print(f"🔍 [{bot.conn.id} - Collect] Recherche de {target_item}")

    # Initialiser le compteur d'échecs s'il n'existe pas
    if not hasattr(bot, f'fail_count_{target_item}'):
        setattr(bot, f'fail_count_{target_item}', 0)
    
    item_positions = scan_for_item(bot, target_item)
    if not item_positions:
        fail_count = getattr(bot, f'fail_count_{target_item}', 0)
        fail_count += 1
        setattr(bot, f'fail_count_{target_item}', fail_count)
        
        # Si on a échoué plusieurs fois, reset le compteur d'exploration
        if fail_count > 5:
            print(f"🔄 [{bot.conn.id} - Collect] Reset exploration après {fail_count} échecs")
            if hasattr(bot, 'exploration_counter'):
                bot.exploration_counter = 0
            setattr(bot, f'fail_count_{target_item}', 0)
        
        explore_random(bot)
        return False

    # Reset du compteur d'échecs si on trouve quelque chose
    setattr(bot, f'fail_count_{target_item}', 0)
    if hasattr(bot, 'exploration_counter'):
        bot.exploration_counter = max(0, bot.exploration_counter - 1)

    for pos in item_positions:
        if pos['index'] == 0:
            collect_item_on_tile(bot, target_item, pos['count'])
            return True
        else:
            if move_towards_tile(bot, pos['index']):
                # une fois arrivé, on peut rescanner et tenter la collecte à nouveau
                look_resp = bot.conn.send_and_receive("Look")
                vision = parse_look(look_resp)
                if not vision:
                    continue
                count_on_tile = vision[0].count(target_item)
                if count_on_tile > 0:
                    collect_item_on_tile(bot, target_item, count_on_tile)
                return True

    return False

def manage_collect_food(bot, target_amount):
    print(f"🔍 [{bot.conn.id} - Survie] Démarrage collecte de nourriture (objectif: {target_amount})")

    while True:
        inventory_resp = bot.conn.send_and_receive("Inventory")
        bot.inventory = parse_inventory(inventory_resp)
        current_food = bot.inventory.get("food", 0)

        if current_food >= target_amount:
            print(f"🍗 [{bot.conn.id} - Survie] Stock de nourriture OK: {current_food}/{target_amount}")
            return True

        food_positions = scan_for_item(bot, "food")
        if not food_positions:
            print(f"🔍 [{bot.conn.id} - Survie] Aucune food en vue, exploration...")
            explore_random(bot)
            continue

        for pos in food_positions:
            if pos["index"] == 0:
                collect_item_on_tile(bot, "food", pos["count"])
            else:
                if move_towards_tile(bot, pos["index"]):
                    # Re-scan de la case après déplacement
                    look_resp = bot.conn.send_and_receive("Look")
                    vision = parse_look(look_resp)
                    if not vision:
                        print(f"⚠️ [{bot.conn.id} - Survie] Vision vide après déplacement")
                        continue
                    count = vision[0].count("food")
                    if count > 0:
                        collect_item_on_tile(bot, "food", count)

        # Re-vérification après les déplacements et collectes
        inventory_resp = bot.conn.send_and_receive("Inventory")
        bot.inventory = parse_inventory(inventory_resp)
        if bot.inventory.get("food", 0) >= target_amount:
            print(f"✅ [{bot.conn.id} - Survie] Objectif de food atteint: {bot.inventory.get('food')}")
            return True

def calculate_moves_to_tile(index, level):
    if index == 0:
        return []

    current_index = 0
    for distance in range(1, level + 2):  # vision = level + 1
        tiles_in_row = 2 * distance + 1
        row_start = current_index
        row_end = current_index + tiles_in_row

        if index >= row_start and index < row_end:
            moves = ["Forward"] * distance
            position_in_row = index - row_start
            middle = tiles_in_row // 2
            side_offset = position_in_row - middle
            if side_offset < 0:
                moves = ["Left"] * abs(side_offset) + moves
            elif side_offset > 0:
                moves = ["Right"] * abs(side_offset) + moves
            return moves

        current_index = row_end

    return []

def move_towards_tile(bot, index):
    moves = calculate_moves_to_tile(index, bot.level)
    # print(f"🚶 [{bot.conn.id} - Collect] Calcul du déplacement vers case {index}: {moves}")
    for move in moves:
        resp = bot.conn.send_and_receive(move)
        if resp.strip() != "ok":
            print(f"❌ [{bot.conn.id} - Collect] Échec du move {move}: {resp}")
            return False
        # print(f"✅ [{bot.conn.id} - Collect] Move {move} réussi")
    return True

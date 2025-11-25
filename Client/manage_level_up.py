#!/usr/bin/env python3
#
# EPITECH PROJECT, 2024
# Zappy
# File description:
# manage_level_up.py
#
import random
from parser import parse_look, parse_inventory
from manage_collect import manage_collect

LEVEL_REQUIREMENTS = {
    1: {"players": 1, "resources": {"linemate": 1}},
    2: {"players": 2, "resources": {"linemate": 1, "deraumere": 1, "sibur": 1}},
    3: {"players": 2, "resources": {"linemate": 2, "sibur": 1, "phiras": 2}},
    4: {"players": 4, "resources": {"linemate": 1, "deraumere": 1, "sibur": 2, "phiras": 1}},
    5: {"players": 4, "resources": {"linemate": 1, "deraumere": 2, "sibur": 1, "mendiane": 3}},
    6: {"players": 6, "resources": {"linemate": 1, "deraumere": 2, "sibur": 3, "phiras": 1}},
    7: {"players": 6, "resources": {"linemate": 2, "deraumere": 2, "sibur": 2, "mendiane": 2, "phiras": 2, "thystame": 1}},
}

def find_missing_resources(bot):
    missing = {}
    requirements = LEVEL_REQUIREMENTS.get(bot.level, {})
    if not requirements:
        return missing
    for res, qty in requirements["resources"].items():
        current_qty = bot.inventory.get(res, 0)
        if current_qty < qty:
            missing[res] = qty - current_qty
    return missing

def has_required_players(vision, level):
    requirements = LEVEL_REQUIREMENTS.get(level, {})
    if not requirements:
        return False
    current_tile = vision[0]
    player_count = current_tile.count("player")
    print(f"👥 [{level}] Joueurs sur la case : {player_count}/{requirements['players']}")
    return player_count >= requirements["players"]

def drop_resources(bot, level):
    requirements = LEVEL_REQUIREMENTS.get(level, {})
    for res, qty in requirements["resources"].items():
        for _ in range(qty):
            resp = bot.conn.send_and_receive(f"Set {res}")
            if resp.strip() != "ok":
                print(f"❌ [{bot.conn.id}] Erreur Set {res}: {resp}")
                return False
    return True

def manage_level_up(bot) -> bool:
    print(f"📈 [{bot.conn.id}] Tentative de montée de niveau {bot.level}")

    # Actualiser l'inventaire
    inventory_resp = bot.conn.send_and_receive("Inventory")
    bot.inventory = parse_inventory(inventory_resp)

    # Vérifier les ressources manquantes
    missing = find_missing_resources(bot)
    if missing:
        print(f"📦 [{bot.conn.id}] Ressources manquantes : {missing}")
        for res in missing:
            if manage_collect(bot, res):
                print(f"✅ [{bot.conn.id}] {res} récupéré, relancement de la vérification")
                return False  # On ressort pour rechecker l'inventaire
        print(f"🚶 [{bot.conn.id}] Exploration sans résultat pour les ressources")
        return False

    # Vérifier les joueurs présents
    look_resp = bot.conn.send_and_receive("Look")
    vision = parse_look(look_resp)
    if not vision or not has_required_players(vision, bot.level):
        print(f"👥 [{bot.conn.id}] Pas assez de joueurs présents pour incantation")
        return False

    # Poser les ressources
    if not drop_resources(bot, bot.level):
        print(f"📦 [{bot.conn.id}] Erreur lors du dépôt des ressources")
        return False

    # Lancer l'incantation
    print(f"🔮 [{bot.conn.id}] Lancement de l'incantation")
    incant_resp = bot.conn.send_and_receive("Incantation")
    print(f"🔮 [{bot.conn.id}] Réponse serveur: {incant_resp}")

    if "Elevation underway" in incant_resp.lower():
        print(f"🎉 [{bot.conn.id}] Incantation démarrée ! Passage au niveau {bot.level + 1}")
        bot.level += 1
        return True

    print(f"❌ [{bot.conn.id}] Échec incantation")
    return False

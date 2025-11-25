#!/usr/bin/env python3
#
# EPITECH PROJECT, 2024
# Zappy
# File description:
# bot.py
#
from connection_info import Connection, ConnectionClosedException
from parser import parse_inventory, parse_look
from strategies import spawn, parse_position, enough_resources, incantation, receive_message
from manage_level_up import manage_level_up, LEVEL_REQUIREMENTS
from manage_collect import manage_collect, manage_collect_food
import time
import sys
import os
color = "\033[31m"
color2 = "\033[32m"
color3 = "\033[33m"
reset = "\033[0m"

FOOD_DANGER = 10
FOOD_TARGET = 70

class ZappyBot:
    def __init__(self, host: str, port: int, team_name: str, is_og: bool):
        self.conn = Connection(host, port, team_name, is_og)
        self.inventory = {}
        self.level = 1
        self.map_width = 0
        self.map_height = 0
        self.assigned_resource = None
        self.enough_resources = False
        self.total_to_collect = 0
        self.food_target = FOOD_TARGET
        self.pos_x = 0
        self.pos_y = 0
        self.orientation = 1
        self.meet_pos = 0
        self.check_move = True

    def assign_resource(self):
        resource_map = ["linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"]
        self.assigned_resource = resource_map[self.conn.id % len(resource_map)]

        for lvl_data in LEVEL_REQUIREMENTS.values():
            res_required = lvl_data["resources"]
            if self.assigned_resource in res_required:
                self.total_to_collect += res_required[self.assigned_resource]

        print(f"🎯 [{self.conn.id} - Bot] Ressource assignée: {self.assigned_resource} = {self.total_to_collect}")

    def update_inventory(self, inventory_resp: str):
        self.inventory = parse_inventory(inventory_resp)
        print(f"📦 [{self.conn.id} - Bot] Inventaire mis à jour: {self.inventory}")

    def parse_vision(self, look_resp: str) -> list:
        return parse_look(look_resp)

    def calculate_moves_to_tile(self, tile_index: int) -> list:
        if tile_index == 0:
            return []

        moves = []
        current_idx = 0

        for distance in range(1, self.level + 2):
            tiles_in_row = 2 * distance + 1
            row_start = current_idx
            row_end = current_idx + tiles_in_row

            if tile_index >= row_start and tile_index < row_end:
                moves = ["Forward"] * distance
                position_in_row = tile_index - row_start
                middle = tiles_in_row // 2
                if position_in_row < middle:
                    moves = ["Left"] + moves
                elif position_in_row > middle:
                    moves = ["Right"] + moves
                break
            current_idx = row_end
        return moves

    def run(self) -> int:
        if not self.conn.connect():
            return 84

        self.assign_resource()

        if self.conn.available_slots > 1:
            pid = os.fork()
            if pid == 0:
                os.execv(sys.argv[0], sys.argv + ['-i'])
            else:
                print(f"🎉 [{self.conn.id} - Bot] Nouveau client [{self.conn.id + 1}] créé by OG")

        self.map_width, self.map_height = self.conn.get_map_dimensions()
        print(f"🗺️  [{self.conn.id} - Bot] Carte de taille {self.map_width}x{self.map_height}")
        try:
            spawn(self)
            self.meet_pos = sum(ord(c) for c in self.conn.team_name) % self.conn.map_width
            while True:
                inventory_resp = self.conn.send_and_receive("Inventory")
                self.inventory = parse_inventory(inventory_resp)
                nb_player_resp = self.conn.send_and_receive("Player_nbr")
                food_qty = self.inventory.get("food", 0)
                self.pos_x, self.pos_y = parse_position(self.conn.send_and_receive("Player_pos"))
                orientation_response = self.conn.send_and_receive("Orientation").strip()
                try:
                    self.orientation = int(orientation_response)
                    if self.orientation not in [0, 1, 2, 3]:
                        self.orientation = 0
                except ValueError:
                    self.orientation = 0

                message = receive_message(self)
                print(f"🍖 [{self.conn.id} - Bot] Nourriture actuelle ({food_qty})")

                if message["team"] == self.conn.team_name and message["move"] and self.check_move:
                    if food_qty < FOOD_DANGER:
                        print(f"🚨 [{self.conn.id} - Bot] FOOD CRITIQUE ({food_qty}) => MODE SURVIE")
                        manage_collect_food(self, target_amount=self.food_target)
                        continue

                    print(f"❤️  {color}[{self.conn.id} - Bot/{nb_player_resp}] {self.assigned_resource} ({self.inventory.get(self.assigned_resource, 0)}/{self.total_to_collect}) ({self.pos_x},{self.pos_y}){reset}")
                    # if test(self):
                    #     continue

                    if enough_resources(self) or self.enough_resources:
                        self.enough_resources = True
                        self.food_target = 45
                        incantation(self)
                    else:
                        # TEMP
                        print(f"{color3}[{self.conn.id} - Bot] {self.assigned_resource} {self.inventory.get(self.assigned_resource, 0)}/{self.total_to_collect}{reset}")
                        if manage_collect(self, self.assigned_resource, is_critical=False):
                            continue

        except ConnectionClosedException:
            print(f"💀 [{self.conn.id} - Bot] Connexion fermée par le serveur")
            self.conn.close()
            return 0
        except KeyboardInterrupt:
            print(f"⏹️  [{self.conn.id} - Bot] Arrêt demandé par l'utilisateur")
            self.conn.close()
            return 0
        return 0

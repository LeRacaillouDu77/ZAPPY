#!/usr/bin/env python3
#
# EPITECH PROJECT, 2024
# Zappy
# File description:
# parser.py
#

def parse_inventory(response: str) -> dict:
    resp = response.strip()
    if resp.startswith('[') and resp.endswith(']'):
        resp = resp[1:-1]
    items = resp.split(',')
    inventory = {}
    for item in items:
        item = item.strip()
        if not item:
            continue
        if ':' in item:
            parts = item.split(':')
            if len(parts) == 2:
                name = parts[0].strip()
                try:
                    quantity = int(parts[1].strip())
                except:
                    quantity = 0
                inventory[name] = quantity
        else:
            parts = item.split(' ')
            if len(parts) == 2:
                name = parts[0]
                try:
                    quantity = int(parts[1])
                except:
                    quantity = 0
                inventory[name] = quantity
    return inventory

def parse_look(response: str) -> list:
    vision = []
    resp = response.strip()
    if not resp.startswith('[') or not resp.endswith(']'):
        print(f"⚠️ [Parser] Format Look invalide: {resp}")
        return vision
    resp = resp[1:-1]
    if not resp:
        return [[]]
    tiles = resp.split(',')

    for tile in tiles:
        tile = tile.strip()
        if not tile:
            vision.append([])
        else:
            objects = tile.split(' ')
            objects = [obj.strip() for obj in objects if obj.strip()]
            vision.append(objects)

    return vision

def parse_connect_nbr(response: str) -> int:
    try:
        return int(response.strip())
    except ValueError:
        print(f"⚠️ [Parser] Format Connect_nbr invalide: {response}")
        return -1

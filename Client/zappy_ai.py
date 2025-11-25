#!/usr/bin/env python3
#
# EPITECH PROJECT, 2024
# Zappy
# File description:
# zappy_ai.py
#
import os
import sys

HERE = os.path.dirname(os.path.realpath(__file__))
CLIENT_DIR = os.path.join(HERE, "Client")
if CLIENT_DIR not in sys.path:
    sys.path.insert(0, CLIENT_DIR)

from bot import ZappyBot

def print_usage(p):
    print(f"Usage: {p} -p port -n team_name -h hostname")

def parse_arguments(args):
    port = None
    team_name = None
    host = "localhost"
    is_og = True
    i = 0
    while i < len(args):
        if args[i] == '-p' and i + 1 < len(args):
            try:
                port = int(args[i + 1])
            except:
                return None
            i += 2
        elif args[i] == '-n' and i + 1 < len(args):
            team_name = args[i + 1]
            i += 2
        elif args[i] == '-h' and i + 1 < len(args):
            host = args[i + 1]
            i += 2
        elif args[i] == '-i':
            is_og = False
            i += 1
        else:
            return None
    if port is None or team_name is None:
        return None
    return (host, port, team_name, is_og)

def main():
    args = parse_arguments(sys.argv[1:])
    if args is None:
        print_usage(sys.argv[0])
        sys.exit(84)
    host, port, team_name, is_og = args
    bot = ZappyBot(host, port, team_name, is_og)
    result = bot.run()
    if result == 84:
        print(f"[Error] Échec de connexion à {host}:{port}")
    sys.exit(result)

if __name__ == "__main__":
    main()

#!/usr/bin/env python3
#
# EPITECH PROJECT, 2024
# Zappy
# File description:
# connection_info.py
#

import socket
from queue import Queue
import threading
import time

class ConnectionClosedException(Exception):
    """Exception levée quand la connexion est fermée côté serveur"""
    pass

class Connection:
    MAX_MESSAGE_LENGTH = 1024

    def __init__(self, host, port, team_name, is_og):
        self.host = host
        self.port = port
        self.team_name = team_name
        self.is_og = is_og
        self.sock = None
        self.map_width = 0
        self.map_height = 0
        self.available_slots = 0
        self.id = -1
        self.broadcast_queue = Queue()
        self.listening_thread = None
        self.stop_listening = False
        self.send_lock = threading.Lock()  # Ajout d'un verrou pour les envois

    def connect(self):
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.connect((self.host, self.port))
            data = self.receive()
            if not data.startswith("WELCOME"):
                raise Exception(f"Attendu WELCOME, reçu: {data}")
            print(f"✅ [Connexion] {data}")
            self.send(self.team_name)
            print(f"📤 [Connexion] Équipe envoyée: {self.team_name}")

            # Recevoir CLIENT-NUM (nombre d'œufs disponibles)
            client_num_data = self.receive()
            try:
                self.available_slots = int(client_num_data.strip())
                print(f"🥚 [Connexion] CLIENT-NUM: {self.available_slots - 1} œufs disponibles")
            except ValueError:
                print(f"⚠️  [Connexion] Erreur parsing CLIENT-NUM: '{client_num_data}'")
                self.available_slots = 0

            # Recevoir l'ID du client
            client_id_data = self.receive()
            try:
                self.id = int(client_id_data.strip())
                if self.is_og:
                    print(f"🆔 [{self.id} {{OG}} - Connexion] ID du client: {self.id}")
                else:
                    print(f"🆔 [{self.id} - Connexion] ID du client: {self.id}")
            except ValueError:
                print(f"⚠️  [{self.id} - Connexion] Erreur parsing ID: '{client_id_data}'")
                self.id = -1

            # Recevoir les dimensions de la carte
            dimensions_data = self.receive()
            try:
                dimensions = dimensions_data.strip().split()
                if len(dimensions) == 2:
                    self.map_width = int(dimensions[0])
                    self.map_height = int(dimensions[1])
                    print(f"🗺️  [{self.id} - Connexion] Carte: {self.map_width}x{self.map_height}")
                else:
                    print(f"⚠️  [{self.id} - Connexion] Format dimensions invalide: '{dimensions_data}'")
            except ValueError:
                print(f"⚠️  [{self.id} - Connexion] Erreur parsing dimensions: '{dimensions_data}'")
            print(f"🎮 [{self.id} - Connexion] IA connectée avec succès à l'équipe {self.team_name}")
            return True
        except Exception as e:
            print(f"❌ [{self.id} - Connexion] Erreur: {e}")
            return False

    def send(self, command: str):
        if self.sock is None:
            print(f"[{self.id} - Error] Tried to send while not connected")
            raise ConnectionClosedException("Socket is None")
        try:
            self.sock.send(f"{command}\n".encode('utf-8'))
        except socket.error as e:
            raise ConnectionClosedException(f"Socket error while sending: {e}")
        except Exception as e:
            raise ConnectionClosedException(f"Failed to send: {e}")

    def receive(self) -> str:
        if self.sock is None:
            raise ConnectionClosedException("Socket is None")
        try:
            buffer = ""
            while True:
                data = self.sock.recv(1).decode('utf-8')
                if not data:
                    raise ConnectionClosedException("Connection closed by server")
                if data == '\n':
                    break
                buffer += data

            message = buffer.strip()
            return message
        except socket.error as e:
            raise ConnectionClosedException(f"Socket error: {e}")
        except Exception as e:
            raise ConnectionClosedException(f"Failed to receive: {e}")

    def send_and_receive(self, command: str) -> str:
        self.send(command)
        while True:
            message = self.receive()
            if not message.startswith("message "):
                return message
            else:
                self.broadcast_queue.put(message)

    def get_next_broadcast(self, timeout=None):
        try:
            return self.broadcast_queue.get(timeout=timeout)
        except:
            return None

    def close(self):
        if self.sock:
            self.sock.close()
            self.sock = None

    def get_map_dimensions(self):
        """Retourne les dimensions de la carte (largeur, hauteur)"""
        return (self.map_width, self.map_height)

    def get_available_slots(self):
        """Retourne le nombre d'œufs disponibles dans l'équipe (CLIENT-NUM)"""
        return self.available_slots

    def get_id(self):
        """Retourne l'ID du client"""
        return self.id

    def start_listening(self):
        """Démarre le thread d'écoute des messages broadcast"""
        if self.listening_thread is not None:
            print(f"[{self.id} - Warning] Listening thread already started")
            return
        self.stop_listening = False
        self.listening_thread = threading.Thread(target=self.listen_for_broadcasts)
        self.listening_thread.start()
        print(f"🛡️  [{self.id} - Connexion] Thread d'écoute des broadcasts démarré")

    def stop_listening(self):
        """Arrête le thread d'écoute des messages broadcast"""
        self.stop_listening = True
        if self.listening_thread is not None:
            self.listening_thread.join()
            self.listening_thread = None
            print(f"🛑 [{self.id} - Connexion] Thread d'écoute des broadcasts arrêté")

    def listen_for_broadcasts(self):
        while not self.stop_listening:
            try:
                message = self.receive()
                if message.startswith("message "):
                    self.broadcast_queue.put(message)
                else:
                    self.command_queue.put(message)
            except ConnectionClosedException:
                print(f"❌ [{self.id}] Connexion fermée par le serveur")
                break
            except Exception as e:
                print(f"⚠️ [{self.id}] Erreur dans le thread listen_for_broadcasts: {e}")

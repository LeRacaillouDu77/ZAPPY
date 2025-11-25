/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** tile.hpp
*/

#include <vector>
#include <map>
#include <utility>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#pragma once
#define TILE_SIZE 52

class Tile {
    public:
        int food;       // q0 - nourriture
        int linemate;   // q1 - linemate
        int deraumere;  // q2 - deraumere
        int sibur;      // q3 - sibur
        int mendiane;   // q4 - mendiane
        int phiras;     // q5 - phiras
        int thystame;   // q6 - thystame
        sf::Vector2i position;

        sf::Sprite m_camp_sprite;
        sf::Sprite m_food_sprite;
        sf::Sprite m_linemate_sprite;
        sf::Sprite m_deraumere_sprite;
        sf::Sprite m_sibur_sprite;
        sf::Sprite m_mendiane_sprite;
        sf::Sprite m_phiras_sprite;
        sf::Sprite m_thystame_sprite;

        static sf::Texture s_camp_Texture;
        static sf::Texture s_food_Texture;
        static sf::Texture s_linemate_Texture;
        static sf::Texture s_deraumere_Texture;
        static sf::Texture s_sibur_Texture;
        static sf::Texture s_mendiane_Texture;
        static sf::Texture s_phiras_Texture;
        static sf::Texture s_thystame_Texture;
        static bool s_texturesLoaded;

        sf::Vector2i m_tile_position;

        Tile();
        Tile(int q0, int q1, int q2, int q3, int q4, int q5, int q6);
        Tile(const Tile& other);
        Tile& operator=(const Tile& other);
        static void loadSharedTextures();
        void initializeSprites();
        void sprite_from_file(sf::Sprite &sprite, sf::Texture &texture, const std::string& filename);
        void setPosition(int x, int y);
        sf::Vector2i getPosition() const;
};

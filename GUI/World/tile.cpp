/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** tile.cpp
*/

#include "tile.hpp"
#include <iostream>

// Définition des textures statiques partagées
sf::Texture Tile::s_camp_Texture;
sf::Texture Tile::s_food_Texture;
sf::Texture Tile::s_linemate_Texture;
sf::Texture Tile::s_deraumere_Texture;
sf::Texture Tile::s_sibur_Texture;
sf::Texture Tile::s_mendiane_Texture;
sf::Texture Tile::s_phiras_Texture;
sf::Texture Tile::s_thystame_Texture;
bool Tile::s_texturesLoaded = false;

void Tile::loadSharedTextures()
{
    if (s_texturesLoaded) return;
    
    s_camp_Texture.loadFromFile("Assets/MAIN_VILLAGE/Camp.png");
    s_food_Texture.loadFromFile("Assets/Items/food_sprite.png");
    s_linemate_Texture.loadFromFile("Assets/Items/linemate_sprite.png");
    s_deraumere_Texture.loadFromFile("Assets/Items/deraumere_sprite.png");
    s_sibur_Texture.loadFromFile("Assets/Items/sibur_sprite.png");
    s_mendiane_Texture.loadFromFile("Assets/Items/mendiane_sprite.png");
    s_phiras_Texture.loadFromFile("Assets/Items/phiras_sprite.png");
    s_thystame_Texture.loadFromFile("Assets/Items/thystame_sprite.png");
    
    s_texturesLoaded = true;
}

void Tile::initializeSprites()
{
    loadSharedTextures();

    m_camp_sprite.setTexture(s_camp_Texture);
    m_food_sprite.setTexture(s_food_Texture);
    m_linemate_sprite.setTexture(s_linemate_Texture);
    m_deraumere_sprite.setTexture(s_deraumere_Texture);
    m_sibur_sprite.setTexture(s_sibur_Texture);
    m_mendiane_sprite.setTexture(s_mendiane_Texture);
    m_phiras_sprite.setTexture(s_phiras_Texture);
    m_thystame_sprite.setTexture(s_thystame_Texture);
}

void Tile::sprite_from_file(sf::Sprite &sprite, sf::Texture &texture, const std::string& filename)
{
    if (texture.loadFromFile(filename)) {
        sprite.setTexture(texture);
    } else {
        std::cerr << "Erreur de chargement de la texture: " << filename << std::endl;
    }
}

Tile::Tile() : food(0), linemate(0), deraumere(0), sibur(0), mendiane(0), phiras(0), thystame(0)
{
    initializeSprites();
}

Tile::Tile(int q0, int q1, int q2, int q3, int q4, int q5, int q6)
    : food(q0), linemate(q1), deraumere(q2), sibur(q3), mendiane(q4), phiras(q5), thystame(q6) {
    initializeSprites();
}

Tile::Tile(const Tile& other)
    : food(other.food), linemate(other.linemate), deraumere(other.deraumere),
      sibur(other.sibur), mendiane(other.mendiane), phiras(other.phiras),
      thystame(other.thystame), position(other.position), m_tile_position(other.m_tile_position) {
    initializeSprites();
}

Tile& Tile::operator=(const Tile& other) {
    if (this != &other) {
        food = other.food;
        linemate = other.linemate;
        deraumere = other.deraumere;
        sibur = other.sibur;
        mendiane = other.mendiane;
        phiras = other.phiras;
        thystame = other.thystame;
        position = other.position;
        m_tile_position = other.m_tile_position;
        if (m_camp_sprite.getTexture() == nullptr) {
            initializeSprites();
        }
    }
    return *this;
}

void Tile::setPosition(int x, int y)
{
    position = sf::Vector2i(x, y);
}

sf::Vector2i Tile::getPosition() const
{
    return position;
}
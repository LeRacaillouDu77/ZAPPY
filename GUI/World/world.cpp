/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** world.cpp
*/

#include "world.hpp"
#include <stdexcept>

void World::setSize(int width, int height)
{
    m_width = width;
    m_height = height;
    m_tiles.clear();
}

void World::updateTile(int x, int y, int q0, int q1, int q2, int q3, int q4, int q5, int q6)
{
    if (!isValidPosition(x, y)) {
        throw std::out_of_range("Position invalide: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }
    Tile tile(q0, q1, q2, q3, q4, q5, q6);
    m_tiles[std::make_pair(x, y)] = tile;
}

void World::updateTile(int x, int y, const Tile& tile)
{
    if (!isValidPosition(x, y)) {
        throw std::out_of_range("Position invalide: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }
    m_tiles[std::make_pair(x, y)] = tile;
}

Tile& World::getTile(int x, int y)
{
    if (!isValidPosition(x, y)) {
        throw std::out_of_range("Position invalide: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }
    auto it = m_tiles.find(std::make_pair(x, y));
    if (it == m_tiles.end()) {
        static Tile emptyTile;
        return emptyTile;
    }
    return it->second;
}

bool World::hasTile(int x, int y) const
{
    if (!isValidPosition(x, y)) {
        return false;
    }
    return m_tiles.find(std::make_pair(x, y)) != m_tiles.end();
}

void World::clear()
{
    m_tiles.clear();
    m_width = 0;
    m_height = 0;
}

bool World::isValidPosition(int x, int y) const
{
    return (x >= 0 && y >= 0);
}

size_t World::getTileCount() const {
    return m_tiles.size();
}

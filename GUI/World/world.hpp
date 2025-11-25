/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** World.hpp
*/

#pragma once

#include <vector>
#include <map>
#include <utility>
#include "tile.hpp"

class World {
    private:
        int m_width;
        int m_height;
        std::map<std::pair<int, int>, Tile> m_tiles;

    public:
        World() : m_width(0), m_height(0) {}
        World(int width, int height) : m_width(width), m_height(height) {}
        ~World() {}

        void setSize(int width, int height);
        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }
        void updateTile(int x, int y, int q0, int q1, int q2, int q3, int q4, int q5, int q6);
        void updateTile(int x, int y, const Tile& tile);
        Tile& getTile(int x, int y);
        bool hasTile(int x, int y) const;
        const std::map<std::pair<int, int>, Tile>& getAllTiles() const { return m_tiles; }
        void clear();
        bool isValidPosition(int x, int y) const;
        size_t getTileCount() const;
};

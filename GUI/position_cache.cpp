/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** position_cache.cpp - Système de cache des positions pour optimiser les performances
*/

#include "gui.hpp"
#include <cmath>

void ZappyGUI::updateRenderCache()
{
    sf::Vector2u currentWindowSize = m_window->getSize();
    sf::Vector2u currentTextureSize = game_bk_texture.getSize();
    int currentWorldWidth = m_world.getWidth();
    int currentWorldHeight = m_world.getHeight();
    if (m_renderCache.isValid &&
        m_renderCache.lastWindowSize == currentWindowSize &&
        m_renderCache.lastTextureSize == currentTextureSize &&
        m_renderCache.lastWorldWidth == currentWorldWidth &&
        m_renderCache.lastWorldHeight == currentWorldHeight) {
        return;
    }
    m_renderCache.scaleX = static_cast<float>(currentWindowSize.x) / currentTextureSize.x;
    m_renderCache.scaleY = static_cast<float>(currentWindowSize.y) / currentTextureSize.y;
    m_renderCache.TOP_X = 360.f * m_renderCache.scaleX;
    m_renderCache.TOP_Y = 62.f * m_renderCache.scaleY;
    m_renderCache.TILE_WIDTH = (570.f * m_renderCache.scaleX) / currentWorldWidth;
    m_renderCache.TILE_HEIGHT = (410.f * m_renderCache.scaleY) / currentWorldHeight;
    m_renderCache.START_X = m_renderCache.TOP_X;
    m_renderCache.START_Y = m_renderCache.TOP_Y;
    m_renderCache.tilePositions.clear();
    for (int gridX = 0; gridX < currentWorldWidth; gridX++) {
        for (int gridY = 0; gridY < currentWorldHeight; gridY++) {
            float screenX = m_renderCache.START_X + (gridX - gridY) * (m_renderCache.TILE_WIDTH / 2.f);
            float screenY = m_renderCache.START_Y + (gridX + gridY) * (m_renderCache.TILE_HEIGHT / 2.f);
            screenY += m_renderCache.TILE_HEIGHT * 0.3f;
            m_renderCache.tilePositions[{gridX, gridY}] = sf::Vector2f(screenX, screenY);
        }
    }
    m_renderCache.lastWindowSize = currentWindowSize;
    m_renderCache.lastTextureSize = currentTextureSize;
    m_renderCache.lastWorldWidth = currentWorldWidth;
    m_renderCache.lastWorldHeight = currentWorldHeight;
    m_renderCache.isValid = true;
}

void ZappyGUI::invalidateRenderCache()
{
    m_renderCache.isValid = false;
    m_renderCache.tilePositions.clear();
}

sf::Vector2f ZappyGUI::getCachedTilePosition(int x, int y)
{
    updateRenderCache();
    auto it = m_renderCache.tilePositions.find({x, y});
    if (it != m_renderCache.tilePositions.end()) {
        return it->second;
    }
    float screenX = m_renderCache.START_X + (x - y) * (m_renderCache.TILE_WIDTH / 2.f);
    float screenY = m_renderCache.START_Y + (x + y) * (m_renderCache.TILE_HEIGHT / 2.f);
    screenY += m_renderCache.TILE_HEIGHT * 0.3f;
    sf::Vector2f position(screenX, screenY);
    m_renderCache.tilePositions[{x, y}] = position;
    return position;
}

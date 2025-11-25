/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** culling.cpp - Optimisation des performances par élimination des éléments hors écran
*/

#include "gui.hpp"
#include <cmath>

sf::FloatRect ZappyGUI::getVisibleBounds() const
{
    sf::View currentView = m_camera.getView();
    sf::Vector2f center = currentView.getCenter();
    sf::Vector2f size = currentView.getSize();

    float margin = 70.0f;
    float left = center.x - (size.x / 2.0f) - margin;
    float top = center.y - (size.y / 2.0f) - margin;
    float width = size.x + (margin * 2.0f);
    float height = size.y + (margin * 2.0f);
    return sf::FloatRect(left, top, width, height);
}

bool ZappyGUI::isPositionVisible(int x, int y) const
{
    if (m_showGameEndScreen) {
        return true;
    }
    if (!m_world.isValidPosition(x, y) || game_bk_texture.getSize().x == 0) {
        return false;
    }
    sf::Vector2f screenPos = getPlayerBaseTilePosition(x, y);
    sf::FloatRect visibleBounds = getVisibleBounds();
    return visibleBounds.contains(screenPos);
}

bool ZappyGUI::isSpriteVisible(const sf::Vector2f& position, const sf::Vector2f& size) const
{
    sf::FloatRect visibleBounds = getVisibleBounds();
    sf::FloatRect spriteRect(position.x - size.x / 2.0f, position.y - size.y / 2.0f, size.x, size.y);
    return visibleBounds.intersects(spriteRect);
}

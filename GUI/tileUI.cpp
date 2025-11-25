/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** tileUI.cpp
*/

#include "gui.hpp"

void ZappyGUI::initializeTileStatsUI()
{
    m_tileStatsBackground.setSize(sf::Vector2f(450, 350));
    m_tileStatsBackground.setFillColor(sf::Color(0, 0, 0, 200));
    m_tileStatsBackground.setOutlineColor(sf::Color::White);
    m_tileStatsBackground.setOutlineThickness(2);
    m_tileStatsTitle.setFont(m_font);
    m_tileStatsTitle.setString("Informations de la Tile");
    m_tileStatsTitle.setCharacterSize(24);
    m_tileStatsTitle.setFillColor(sf::Color::White);
    m_tilePositionText.setFont(m_font);
    m_tilePositionText.setCharacterSize(18);
    m_tilePositionText.setFillColor(sf::Color::Cyan);
    m_tileResourcesText.setFont(m_font);
    m_tileResourcesText.setCharacterSize(16);
    m_tileResourcesText.setFillColor(sf::Color::Green);
    m_tileCloseButtonText.setFont(m_font);
    m_tileCloseButtonText.setString("Fermer (Echap)");
    m_tileCloseButtonText.setCharacterSize(16);
    m_tileCloseButtonText.setFillColor(sf::Color::Red);
}

void ZappyGUI::renderTileStatsOverlay()
{
    if (!m_showTileStats || m_selectedTileX == -1 || m_selectedTileY == -1)
        return;
    if (!m_world.hasTile(m_selectedTileX, m_selectedTileY)) {
        m_showTileStats = false;
        return;
    }
    const Tile& selectedTile = m_world.getTile(m_selectedTileX, m_selectedTileY);
    sf::View currentView = m_window->getView();
    m_window->setView(m_window->getDefaultView());
    float overlayX = (WINDOW_WIDTH - m_tileStatsBackground.getSize().x) / 2.0f;
    float overlayY = (WINDOW_HEIGHT - m_tileStatsBackground.getSize().y) / 2.0f;
    m_tileStatsBackground.setPosition(overlayX, overlayY);
    m_tileStatsTitle.setPosition(overlayX + 20, overlayY + 20);
    m_tilePositionText.setString("Position: (" + std::to_string(m_selectedTileX) + ", " + std::to_string(m_selectedTileY) + ")");
    m_tilePositionText.setPosition(overlayX + 20, overlayY + 60);
    std::string resourcesText = "Ressources disponibles:\n\n";
    resourcesText += "   Nourriture: " + std::to_string(selectedTile.food) + "\n";
    resourcesText += "   Linemate: " + std::to_string(selectedTile.linemate) + "\n";
    resourcesText += "   Deraumere: " + std::to_string(selectedTile.deraumere) + "\n";
    resourcesText += "   Sibur: " + std::to_string(selectedTile.sibur) + "\n";
    resourcesText += "   Mendiane: " + std::to_string(selectedTile.mendiane) + "\n";
    resourcesText += "   Phiras: " + std::to_string(selectedTile.phiras) + "\n";
    resourcesText += "   Thystame: " + std::to_string(selectedTile.thystame) + "\n\n";
    int totalResources = selectedTile.food + selectedTile.linemate + selectedTile.deraumere +
        selectedTile.sibur + selectedTile.mendiane + selectedTile.phiras + selectedTile.thystame;
    resourcesText += "Total: " + std::to_string(totalResources) + " ressources";
    m_tileResourcesText.setString(resourcesText);
    m_tileResourcesText.setPosition(overlayX + 20, overlayY + 100);
    m_tileCloseButtonText.setPosition(overlayX + 20, overlayY + 310);
    m_window->draw(m_tileStatsBackground);
    m_window->draw(m_tileStatsTitle);
    m_window->draw(m_tilePositionText);
    m_window->draw(m_tileResourcesText);
    m_window->draw(m_tileCloseButtonText);
    m_window->setView(currentView);
}

void ZappyGUI::handleTileClick(sf::Vector2f mousePos)
{
    sf::Vector2f worldPos = m_window->mapPixelToCoords(sf::Vector2i(mousePos.x, mousePos.y), m_camera.getView());
    for (int x = 0; x < m_world.getWidth(); x++) {
        for (int y = 0; y < m_world.getHeight(); y++) {
            if (m_world.hasTile(x, y)) {
                sf::Vector2f tilePos = getPlayerBaseTilePosition(x, y);
                if (game_bk_texture.getSize().x > 0) {
                    float scaleX = static_cast<float>(m_window->getSize().x) / game_bk_texture.getSize().x;
                    float scaleY = static_cast<float>(m_window->getSize().y) / game_bk_texture.getSize().y;
                    const int NUM_CAMPS_X = m_world.getWidth();
                    const int NUM_CAMPS_Y = m_world.getHeight();
                    float TILE_WIDTH = (570.f * scaleX) / NUM_CAMPS_X;
                    float TILE_HEIGHT = (410.f * scaleY) / NUM_CAMPS_Y;
                    float adjustedTilePosX = tilePos.x - 10.0f;
                    float adjustedTilePosY = tilePos.y - 15.0f;
                    sf::FloatRect tileBounds(
                        adjustedTilePosX - TILE_WIDTH / 2.0f,
                        adjustedTilePosY - TILE_HEIGHT / 2.0f,
                        TILE_WIDTH,
                        TILE_HEIGHT
                    );
                    if (tileBounds.contains(worldPos)) {
                        m_selectedTileX = x;
                        m_selectedTileY = y;
                        m_showTileStats = true;
                        m_showPlayerStats = false;
                        return;
                    }
                }
            }
        }
    }
}

/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** playerUI.cpp
*/

#include "gui.hpp"

void ZappyGUI::initializePlayerStatsUI()
{
    m_playerStatsBackground.setSize(sf::Vector2f(400, 300));
    m_playerStatsBackground.setFillColor(sf::Color(0, 0, 0, 200));
    m_playerStatsBackground.setOutlineColor(sf::Color::White);
    m_playerStatsBackground.setOutlineThickness(2);
    m_playerStatsTitle.setFont(m_font);
    m_playerStatsTitle.setString("Informations du Joueur");
    m_playerStatsTitle.setCharacterSize(24);
    m_playerStatsTitle.setFillColor(sf::Color::White);
    m_playerTeamText.setFont(m_font);
    m_playerTeamText.setCharacterSize(18);
    m_playerTeamText.setFillColor(sf::Color::Cyan);
    m_playerLevelText.setFont(m_font);
    m_playerLevelText.setCharacterSize(18);
    m_playerLevelText.setFillColor(sf::Color::Yellow);
    m_playerInventoryText.setFont(m_font);
    m_playerInventoryText.setCharacterSize(16);
    m_playerInventoryText.setFillColor(sf::Color::Green);
    m_closeButtonText.setFont(m_font);
    m_closeButtonText.setString("Fermer (Echap)");
    m_closeButtonText.setCharacterSize(16);
    m_closeButtonText.setFillColor(sf::Color::Red);
}

void ZappyGUI::renderPlayerStatsOverlay()
{
    if (!m_showPlayerStats || m_selectedPlayerId == -1)
        return;
    const Player* selectedPlayer = getPlayerById(m_selectedPlayerId);
    if (!selectedPlayer) {
        m_showPlayerStats = false;
        return;
    }
    sf::View currentView = m_window->getView();
    m_window->setView(m_window->getDefaultView());
    float overlayX = (WINDOW_WIDTH - m_playerStatsBackground.getSize().x) / 2.0f;
    float overlayY = (WINDOW_HEIGHT - m_playerStatsBackground.getSize().y) / 2.0f;
    m_playerStatsBackground.setPosition(overlayX, overlayY);
    m_playerStatsTitle.setPosition(overlayX + 20, overlayY + 20);
    m_playerTeamText.setString("Equipe: " + selectedPlayer->teamName + " (ID: " + std::to_string(selectedPlayer->id) + ")");
    m_playerTeamText.setPosition(overlayX + 20, overlayY + 60);
    m_playerLevelText.setString("Niveau: " + std::to_string(selectedPlayer->level));
    m_playerLevelText.setPosition(overlayX + 20, overlayY + 90);
    std::string inventoryText = "Inventaire:\n\n";
    inventoryText += "  Nourriture: " + std::to_string(selectedPlayer->inventory.food) + "\n";
    inventoryText += "  Linemate: " + std::to_string(selectedPlayer->inventory.linemate) + "\n";
    inventoryText += "  Deraumere: " + std::to_string(selectedPlayer->inventory.deraumere) + "\n";
    inventoryText += "  Sibur: " + std::to_string(selectedPlayer->inventory.sibur) + "\n";
    inventoryText += "  Mendiane: " + std::to_string(selectedPlayer->inventory.mendiane) + "\n";
    inventoryText += "  Phiras: " + std::to_string(selectedPlayer->inventory.phiras) + "\n";
    inventoryText += "  Thystame: " + std::to_string(selectedPlayer->inventory.thystame);
    m_playerInventoryText.setString(inventoryText);
    m_playerInventoryText.setPosition(overlayX + 20, overlayY + 120);
    m_closeButtonText.setPosition(overlayX + 20, overlayY + 275);
    m_window->draw(m_playerStatsBackground);
    m_window->draw(m_playerStatsTitle);
    m_window->draw(m_playerTeamText);
    m_window->draw(m_playerLevelText);
    m_window->draw(m_playerInventoryText);
    m_window->draw(m_closeButtonText);
    m_window->setView(currentView);
}

void ZappyGUI::handlePlayerClick(sf::Vector2f mousePos)
{
    sf::Vector2f worldPos = m_window->mapPixelToCoords(sf::Vector2i(mousePos.x, mousePos.y), m_camera.getView());
    for (const auto& player : m_players) {
        sf::Vector2f playerPos = getPlayerBaseTilePosition(player.x, player.y);
        float clickRadius = 30.0f;
        sf::FloatRect playerBounds(
            playerPos.x - clickRadius,
            playerPos.y - clickRadius,
            clickRadius * 2,
            clickRadius * 2
        );
        if (playerBounds.contains(worldPos)) {
            m_selectedPlayerId = player.id;
            m_showPlayerStats = true;
            m_showTileStats = false;
            return;
        }
    }
    handleTileClick(mousePos);
}

sf::Vector2f ZappyGUI::getPlayerScreenPosition(const Player& player) const
{
    if (game_bk_texture.getSize().x == 0)
        return sf::Vector2f(0, 0);
    float scaleX = static_cast<float>(m_window->getSize().x) / game_bk_texture.getSize().x;
    float scaleY = static_cast<float>(m_window->getSize().y) / game_bk_texture.getSize().y;
    const float TOP_X = 360.f * scaleX;
    const float TOP_Y = 62.f * scaleY;
    const int NUM_CAMPS_X = m_world.getWidth();
    const int NUM_CAMPS_Y = m_world.getHeight();
    float TILE_WIDTH = (570.f * scaleX) / NUM_CAMPS_X;
    float TILE_HEIGHT = (410.f * scaleY) / NUM_CAMPS_Y;
    float playerScreenX = TOP_X + (player.x - player.y) * (TILE_WIDTH / 2.f);
    float playerScreenY = TOP_Y + (player.x + player.y) * (TILE_HEIGHT / 2.f);

    return sf::Vector2f(playerScreenX, playerScreenY);
}

/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** animation.cpp
*/

#include "gui.hpp"
#include <cstring>
#include <unistd.h>
#include <sstream>
#include <map>
#include <cmath>
#include <math.h>

void ZappyGUI::renderExplosions()
{
    m_interactionManager.updateInteractions();
    auto activeExpulsions = m_interactionManager.getActiveExpulsions();
    if (activeExpulsions.empty() || game_bk_texture.getSize().x == 0)
        return;
    float scaleX = static_cast<float>(m_window->getSize().x) / game_bk_texture.getSize().x;
    float scaleY = static_cast<float>(m_window->getSize().y) / game_bk_texture.getSize().y;
    const int NUM_CAMPS_X = m_world.getWidth();
    const int NUM_CAMPS_Y = m_world.getHeight();
    float TILE_WIDTH = (570.f * scaleX) / NUM_CAMPS_X;
    float TILE_HEIGHT = (410.f * scaleY) / NUM_CAMPS_Y;
    float explosionScale = std::min(TILE_WIDTH / 494.0f, TILE_HEIGHT / 505.0f) * 0.8f;
    for (const auto& expulsion : activeExpulsions) {
        int playerX, playerY;
        const Player* player = getPlayerById(expulsion.getExpellingPlayerId());
        if (player != nullptr) {
            playerX = player->x;
            playerY = player->y;
        } else {
            playerX = expulsion.getPlayerX();
            playerY = expulsion.getPlayerY();
        }
        if (!isPositionVisible(playerX, playerY)) {
            continue;
        }
        sf::Vector2f explosionPos = getPlayerBaseTilePosition(playerX, playerY);
        float explosionWidth = 494.0f * explosionScale;
        float explosionHeight = 505.0f * explosionScale;
        float explosionX = explosionPos.x - (explosionWidth / 2.0f);
        float explosionY = explosionPos.y - (explosionHeight / 2.0f);
        m_explosionSprite.setPosition(explosionX, explosionY);
        m_explosionSprite.setScale(explosionScale, explosionScale);
        double elapsedTime = expulsion.getElapsedTime();
        double maxDisplayTime = m_interactionManager.getExpulsionDisplayTime();
        float alpha = 255.0f * (1.0f - static_cast<float>(elapsedTime / maxDisplayTime));
        alpha = std::max(0.0f, std::min(255.0f, alpha));
        m_explosionSprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
        m_window->draw(m_explosionSprite);
    }
}

void ZappyGUI::renderBroadcastBeams()
{
    auto activeBeams = m_interactionManager.getActiveBroadcastBeams();
    if (activeBeams.empty() || game_bk_texture.getSize().x == 0) {
        return;
    }
    float scaleX = static_cast<float>(m_window->getSize().x) / game_bk_texture.getSize().x;
    float scaleY = static_cast<float>(m_window->getSize().y) / game_bk_texture.getSize().y;
    const int NUM_CAMPS_X = m_world.getWidth();
    const int NUM_CAMPS_Y = m_world.getHeight();
    float TILE_WIDTH = (570.f * scaleX) / NUM_CAMPS_X;
    float TILE_HEIGHT = (410.f * scaleY) / NUM_CAMPS_Y;
    float broadcastScale = std::min(TILE_WIDTH / m_broadcastTexture.getSize().x, TILE_HEIGHT / m_broadcastTexture.getSize().y) * 0.8f;
    for (const auto& beam : activeBeams) {
        const Player* sender = getPlayerById(beam.getSenderId());
        if (sender == nullptr) {
            continue;
        }
        if (!isPositionVisible(sender->x, sender->y)) {
            continue;
        }
        sf::Vector2f broadcastPos = getPlayerBaseTilePosition(sender->x, sender->y);
        float broadcastWidth = m_broadcastTexture.getSize().x * broadcastScale;
        float broadcastHeight = m_broadcastTexture.getSize().y * broadcastScale;
        float playerSpriteHeight = 50.0f * broadcastScale;
        float broadcastX = broadcastPos.x - (broadcastWidth / 2.0f);
        float broadcastY = broadcastPos.y - playerSpriteHeight - (broadcastHeight / 2.0f);
        m_broadcastSprite.setPosition(broadcastX, broadcastY);
        m_broadcastSprite.setScale(broadcastScale, broadcastScale);
        double elapsedTime = beam.getElapsedTime();
        double maxDisplayTime = 3.0;
        float alpha = 255.0f * (1.0f - static_cast<float>(elapsedTime / maxDisplayTime));
        alpha = std::max(0.0f, std::min(255.0f, alpha));
        m_broadcastSprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
        m_window->draw(m_broadcastSprite);
    }
}

void ZappyGUI::renderPlayers()
{
    if (game_bk_texture.getSize().x == 0 || m_players.empty()) {
        return;
    }
    std::map<std::pair<int, int>, std::vector<Player*>> playersPerTile;
    for (auto& player : m_players) {
        if (isPositionVisible(player.x, player.y)) {
            std::pair<int, int> tilePos = {player.x, player.y};
            playersPerTile[tilePos].push_back(&player);
        }
    }
    float scaleX = static_cast<float>(m_window->getSize().x) / game_bk_texture.getSize().x;
    float scaleY = static_cast<float>(m_window->getSize().y) / game_bk_texture.getSize().y;
    const int NUM_CAMPS_X = m_world.getWidth();
    const int NUM_CAMPS_Y = m_world.getHeight();
    float TILE_WIDTH = (570.f * scaleX) / NUM_CAMPS_X;
    float TILE_HEIGHT = (410.f * scaleY) / NUM_CAMPS_Y;
    for (const auto& tilePlayers : playersPerTile) {
        sf::Vector2f baseTilePos = getPlayerBaseTilePosition(tilePlayers.first.first, tilePlayers.first.second);
        for (size_t i = 0; i < tilePlayers.second.size(); ++i) {
            Player* player = tilePlayers.second[i];
            float spriteScale = std::min(TILE_WIDTH / 66.0f, TILE_HEIGHT / 50.0f) * 0.5f;
            sf::Vector2f playerPos = calculatePlayerOffset(baseTilePos, i, tilePlayers.second.size(), spriteScale);
            player->sprite.setScale(spriteScale, spriteScale);
            float spriteWidth = 66.0f * spriteScale;
            float spriteHeight = 50.0f * spriteScale;
            player->sprite.setPosition(playerPos.x - spriteWidth/2, playerPos.y - spriteHeight/2);
            player->sprite.setPlayerLevel(player->level);
            if (!player->sprite.isAnimating()) {
                player->sprite.startAnimation();
            }
            player->sprite.draw(*m_window);
        }
    }
}

void ZappyGUI::renderDeaths()
{
    auto activeDeaths = m_interactionManager.getActiveDeaths();
    if (activeDeaths.empty() || game_bk_texture.getSize().x == 0) {
        return;
    }
    float scaleX = static_cast<float>(m_window->getSize().x) / game_bk_texture.getSize().x;
    float scaleY = static_cast<float>(m_window->getSize().y) / game_bk_texture.getSize().y;
    const int NUM_CAMPS_X = m_world.getWidth();
    const int NUM_CAMPS_Y = m_world.getHeight();
    float TILE_WIDTH = (570.f * scaleX) / NUM_CAMPS_X;
    float TILE_HEIGHT = (410.f * scaleY) / NUM_CAMPS_Y;
    float skullScale = std::min(TILE_WIDTH / m_skullTexture.getSize().x, TILE_HEIGHT / m_skullTexture.getSize().y) * 0.6f;
    for (const auto& death : activeDeaths) {
        if (!isPositionVisible(death.getPlayerX(), death.getPlayerY())) {
            continue;
        }
        sf::Vector2f deathPos = getPlayerBaseTilePosition(death.getPlayerX(), death.getPlayerY());
        float skullWidth = m_skullTexture.getSize().x * skullScale;
        float skullHeight = m_skullTexture.getSize().y * skullScale;
        float skullX = deathPos.x - (skullWidth / 2.0f);
        float skullY = deathPos.y - (skullHeight / 2.0f);
        m_skullSprite.setPosition(skullX, skullY);
        m_skullSprite.setScale(skullScale, skullScale);
        double elapsedTime = death.getElapsedTime();
        double maxDisplayTime = 3.0;
        float alpha = 255.0f * (1.0f - static_cast<float>(elapsedTime / maxDisplayTime));
        alpha = std::max(0.0f, std::min(255.0f, alpha));
        m_skullSprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
        m_window->draw(m_skullSprite);
    }
}

void ZappyGUI::RenderTiles()
{
    updateRenderCache();
    game_bk_sprite.setScale(m_renderCache.scaleX, m_renderCache.scaleY);
    game_bk_sprite.setPosition(0.f, 0.f);
    m_window->draw(game_bk_sprite);
    const int NUM_CAMPS_X = m_world.getWidth();
    const int NUM_CAMPS_Y = m_world.getHeight();
    float CAMPFIRE_SCALE_X = m_renderCache.TILE_WIDTH / m_campfireTexture.getSize().x;
    float CAMPFIRE_SCALE_Y = m_renderCache.TILE_HEIGHT / m_campfireTexture.getSize().y;
    for (int gridX = 0; gridX < NUM_CAMPS_X; gridX++) {
        for (int gridY = 0; gridY < NUM_CAMPS_Y; gridY++) {
            if (!isPositionVisible(gridX, gridY)) {
                continue;
            }
            sf::Vector2f cachedPos = getCachedTilePosition(gridX, gridY);
            float screenX = cachedPos.x;
            float screenY = cachedPos.y - m_renderCache.TILE_HEIGHT * 0.3f;
            float campHalfWidth = (m_campfireTexture.getSize().x * CAMPFIRE_SCALE_X) / 2.0f;
            float campHalfHeight = (m_campfireTexture.getSize().y * CAMPFIRE_SCALE_Y) / 2.0f;
            float baseTileSize = 50.0f;
            float currentTileSize = std::max(m_renderCache.TILE_WIDTH, m_renderCache.TILE_HEIGHT);
            float sizeRatio = currentTileSize / baseTileSize;
            float maxOffset = campHalfHeight * 0.8f;
            float campfire_offset_y = maxOffset * std::min(1.0f, (sizeRatio - 1.0f) * 0.6f);
            campfire_offset_y = std::max(0.0f, campfire_offset_y);
            float campX = screenX - campHalfWidth;
            float campY = screenY - campHalfHeight + campfire_offset_y;
            m_campfireSprite.setScale(CAMPFIRE_SCALE_X, CAMPFIRE_SCALE_Y);
            m_campfireSprite.setPosition(campX, campY);
            m_window->draw(m_campfireSprite);
            draw_tile(m_world.getTile(gridX, gridY), sf::Vector2f(campX, campY), sf::Vector2f(campHalfWidth, campHalfHeight));
        }
    }
}

void ZappyGUI::renderEggs()
{
    if (game_bk_texture.getSize().x == 0)
        return;
    float scaleX = static_cast<float>(m_window->getSize().x) / game_bk_texture.getSize().x;
    float scaleY = static_cast<float>(m_window->getSize().y) / game_bk_texture.getSize().y;
    const int NUM_CAMPS_X = m_world.getWidth();
    const int NUM_CAMPS_Y = m_world.getHeight();
    float TILE_WIDTH = (570.f * scaleX) / NUM_CAMPS_X;
    float TILE_HEIGHT = (410.f * scaleY) / NUM_CAMPS_Y;
    float eggScale = std::min(TILE_WIDTH / m_eggTexture.getSize().x, TILE_HEIGHT / m_eggTexture.getSize().y) * 0.8f;
    sf::View currentView = m_window->getView();
    m_window->setView(m_camera.getView());
    std::map<std::pair<int, int>, std::vector<const Egg*>> eggsPerTile;
    for (const auto& egg : m_eggs) {
        if (egg.isActive && isPositionVisible(egg.x, egg.y)) {
            std::pair<int, int> tilePos = {egg.x, egg.y};
            eggsPerTile[tilePos].push_back(&egg);
        }
    }
    for (const auto& tileEggs : eggsPerTile) {
        sf::Vector2f baseTilePos = getEggBaseTilePosition(tileEggs.first.first, tileEggs.first.second);
        for (size_t i = 0; i < tileEggs.second.size(); ++i) {
            sf::Vector2f eggPos = calculateEggOffset(baseTilePos, i, tileEggs.second.size());
            sf::Sprite eggSprite = m_eggSprite;
            eggSprite.setScale(eggScale, eggScale);
            eggSprite.setPosition(eggPos.x - (eggSprite.getGlobalBounds().width / 2),
                                 eggPos.y - (eggSprite.getGlobalBounds().height / 2));
            m_window->draw(eggSprite);
        }
    }
    m_window->setView(currentView);
}

void ZappyGUI::renderIncantations()
{
    auto activeIncantations = m_interactionManager.getActiveIncantations();
    if (activeIncantations.empty() || game_bk_texture.getSize().x == 0) {
        return;
    }
    float scaleX = static_cast<float>(m_window->getSize().x) / game_bk_texture.getSize().x;
    float scaleY = static_cast<float>(m_window->getSize().y) / game_bk_texture.getSize().y;
    const int NUM_CAMPS_X = m_world.getWidth();
    const int NUM_CAMPS_Y = m_world.getHeight();
    float TILE_WIDTH = (570.f * scaleX) / NUM_CAMPS_X;
    float TILE_HEIGHT = (410.f * scaleY) / NUM_CAMPS_Y;
    float baseScale = std::min(TILE_WIDTH / m_incantationTexture.getSize().x, TILE_HEIGHT / m_incantationTexture.getSize().y) * 0.8f;
    for (const auto& incantation : activeIncantations) {
        const Player* player = getPlayerById(incantation.getPlayerId());
        if (player == nullptr) {
            continue;
        }
        if (!isPositionVisible(player->x, player->y)) {
            continue;
        }
        sf::Vector2f campPos = getPlayerBaseTilePosition(player->x, player->y);
        float incantationWidth = m_incantationTexture.getSize().x * baseScale;
        float incantationHeight = m_incantationTexture.getSize().y * baseScale;
        double elapsedTime = incantation.getElapsedTime();
        float animationDuration = getIncantationAnimationDuration();
        float scale = baseScale;
        if (elapsedTime < animationDuration) {
            float progress = elapsedTime / animationDuration;
            float scaleMultiplier = 1.0f + 1.5f * sin(progress * M_PI);
            scale = baseScale * scaleMultiplier;
        } else {
            float pulse = 1.0f + 0.1f * sin((elapsedTime - animationDuration) * 3.0f);
            scale = baseScale * pulse;
        }
        float incantationX = campPos.x - (incantationWidth * scale / baseScale) / 2.0f;
        float incantationY = campPos.y - (incantationHeight * scale / baseScale) / 2.0f;
        m_incantationSprite.setPosition(incantationX, incantationY);
        m_incantationSprite.setScale(scale, scale);
        sf::Uint8 alpha = 255;
        if (elapsedTime < animationDuration) {
            float fadeProgress = elapsedTime / animationDuration;
            alpha = static_cast<sf::Uint8>(180 + (75 * fadeProgress));
        }
        m_incantationSprite.setColor(sf::Color(255, 255, 255, alpha));
        m_window->draw(m_incantationSprite);
    }
}

void ZappyGUI::renderIncantationFailures()
{
    auto activeFailures = m_interactionManager.getActiveIncantationFailures();
    if (activeFailures.empty() || game_bk_texture.getSize().x == 0) {
        return;
    }
    float scaleX = static_cast<float>(m_window->getSize().x) / game_bk_texture.getSize().x;
    float scaleY = static_cast<float>(m_window->getSize().y) / game_bk_texture.getSize().y;
    const int NUM_CAMPS_X = m_world.getWidth();
    const int NUM_CAMPS_Y = m_world.getHeight();
    float TILE_WIDTH = (570.f * scaleX) / NUM_CAMPS_X;
    float TILE_HEIGHT = (410.f * scaleY) / NUM_CAMPS_Y;
    float nopeScale = std::min(TILE_WIDTH / m_nopeTexture.getSize().x, TILE_HEIGHT / m_nopeTexture.getSize().y) * 0.6f;
    for (const auto& failure : activeFailures) {
        if (!isPositionVisible(failure.getPlayerX(), failure.getPlayerY())) {
            continue;
        }
        sf::Vector2f failurePos = getPlayerBaseTilePosition(failure.getPlayerX(), failure.getPlayerY());
        float nopeWidth = m_nopeTexture.getSize().x * nopeScale;
        float nopeHeight = m_nopeTexture.getSize().y * nopeScale;
        float nopeX = failurePos.x - (nopeWidth / 2.0f);
        float nopeY = failurePos.y - (nopeHeight / 2.0f);
        m_nopeSprite.setPosition(nopeX, nopeY);
        m_nopeSprite.setScale(nopeScale, nopeScale);
        double elapsedTime = failure.getElapsedTime();
        double maxDisplayTime = 3.0;
        float alpha = 255.0f * (1.0f - static_cast<float>(elapsedTime / maxDisplayTime));
        alpha = std::max(0.0f, std::min(255.0f, alpha));
        m_nopeSprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
        m_window->draw(m_nopeSprite);
    }
}
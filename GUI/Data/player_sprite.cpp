/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** player_sprite.cpp
*/

#include "player_sprite.hpp"
#include <iostream>

PlayerSprite::PlayerSprite()
    : m_currentFrame(0), m_animationTime(0.0f), m_frameTime(0.15f),
      m_isAnimating(false), m_playerLevel(1), m_teamColor(sf::Color::White) {
    resetAnimation();
}

PlayerSprite::~PlayerSprite() {}

bool PlayerSprite::loadTextures()
{
    bool allLoaded = true;

    for (int level = 1; level <= 8; ++level) {
        std::string filename = "Assets/Game/playerlvl_" + std::to_string(level) + ".png";
        if (!m_textures[level - 1].loadFromFile(filename)) {
            std::cerr << "Erreur: Impossible de charger " << filename << std::endl;
            allLoaded = false;
        }
    }
    if (allLoaded) {
        m_sprite.setTexture(m_textures[0]);
        setFrame(0);
    }
    return allLoaded;
}

void PlayerSprite::update(float deltaTime)
{
    if (!m_isAnimating) {
        return;
    }
    m_animationTime += deltaTime;
    if (m_animationTime >= m_frameTime) {
        m_currentFrame = (m_currentFrame + 1) % FRAME_COUNT;
        setFrame(m_currentFrame);
        m_animationTime = 0.0f;
    }
}

void PlayerSprite::setFrame(int frame)
{
    if (frame < 0 || frame >= FRAME_COUNT) {
        return;
    }
    m_currentFrame = frame;
    int frameX = frame * FRAME_WIDTH;
    sf::IntRect frameRect(frameX, 0, FRAME_WIDTH, FRAME_HEIGHT);
    m_sprite.setTextureRect(frameRect);
}

void PlayerSprite::startAnimation() {
    m_isAnimating = true;
    m_animationTime = 0.0f;
}

void PlayerSprite::stopAnimation() {
    m_isAnimating = false;
    setFrame(0);
}

void PlayerSprite::resetAnimation() {
    m_currentFrame = 0;
    m_animationTime = 0.0f;
    m_isAnimating = false;
    setFrame(0);
}

void PlayerSprite::setPlayerLevel(int level)
{
    if (level < 1 || level > 8) {
        std::cerr << "Niveau de joueur invalide: " << level << " (doit être entre 1 et 8)" << std::endl;
        return;
    }
    m_playerLevel = level;
    m_sprite.setTexture(m_textures[level - 1]);
    setFrame(m_currentFrame);
}

void PlayerSprite::setTeamColor(const sf::Color& color) {
    m_teamColor = color;
    m_sprite.setColor(color);
}

void PlayerSprite::setPosition(float x, float y) {
    m_sprite.setPosition(x, y);
}

void PlayerSprite::setScale(float scaleX, float scaleY) {
    m_sprite.setScale(scaleX, scaleY);
}

void PlayerSprite::draw(sf::RenderWindow& window) {
    window.draw(m_sprite);
}

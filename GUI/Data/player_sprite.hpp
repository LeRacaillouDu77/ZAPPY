/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** player_sprite.hpp
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class PlayerSprite {
    private:
        sf::Texture m_textures[8];
        sf::Sprite m_sprite;
        static const int FRAME_COUNT = 6;
        static const int FRAME_WIDTH = 64;
        static const int FRAME_HEIGHT = 50;
        int m_currentFrame;
        float m_animationTime;
        float m_frameTime;
        bool m_isAnimating;
        int m_playerLevel;
        sf::Color m_teamColor;
    public:
        PlayerSprite();
        ~PlayerSprite();
        bool loadTextures();
        void update(float deltaTime);
        void setFrame(int frame);
        void startAnimation();
        void stopAnimation();
        void resetAnimation();

        void setPlayerLevel(int level);
        void setTeamColor(const sf::Color& color);
        void setPosition(float x, float y);
        void setScale(float scaleX, float scaleY);

        // Getters
        sf::Sprite& getSprite() { return m_sprite; }
        const sf::Sprite& getSprite() const { return m_sprite; }
        bool isAnimating() const { return m_isAnimating; }
        int getCurrentFrame() const { return m_currentFrame; }

        void draw(sf::RenderWindow& window);
};

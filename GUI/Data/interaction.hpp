/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** interaction.hpp
*/

#pragma once

#include <string>
#include <chrono>
#include <vector>

class PlayerExpulsion {
    private:
        int m_PlayerId;
        int m_playerX, m_playerY;
        std::chrono::system_clock::time_point m_timestamp;
        bool m_isActive;

    public:
        PlayerExpulsion();
        PlayerExpulsion(int expellingPlayerId, int playerX = 0, int playerY = 0);
        // Getters
        int getExpellingPlayerId() const { return m_PlayerId; }
        int getPlayerX() const { return m_playerX; }
        int getPlayerY() const { return m_playerY; }
        bool isActive() const { return m_isActive; }

        void setActive(bool active);
        double getElapsedTime() const;
};

class PlayerDeath {
    private:
        int m_playerId;
        int m_playerX, m_playerY;
        std::chrono::system_clock::time_point m_timestamp;
        bool m_isActive;
    public:
        PlayerDeath();
        PlayerDeath(int playerId, int playerX, int playerY);
        // Getters
        int getPlayerId() const { return m_playerId; }
        int getPlayerX() const { return m_playerX; }
        int getPlayerY() const { return m_playerY; }
        bool isActive() const { return m_isActive; }
        // Setters
        void setActive(bool active);

        // Méthodes utilitaires
        double getElapsedTime() const;
};

class PlayerIncantation {
    private:
        int m_playerId;
        int m_playerX, m_playerY;
        std::chrono::system_clock::time_point m_timestamp;
        bool m_isActive;

    public:
        PlayerIncantation();
        PlayerIncantation(int playerId, int playerX, int playerY);
        // Getters
        int getPlayerId() const { return m_playerId; }
        int getPlayerX() const { return m_playerX; }
        int getPlayerY() const { return m_playerY; }
        bool isActive() const { return m_isActive; }

        void setActive(bool active);
        double getElapsedTime() const;
};

class IncantationFailure {
    private:
        int m_playerX, m_playerY;
        std::chrono::system_clock::time_point m_timestamp;
        bool m_isActive;

    public:
        IncantationFailure();
        IncantationFailure(int playerX, int playerY);
        // Getters
        int getPlayerX() const { return m_playerX; }
        int getPlayerY() const { return m_playerY; }
        bool isActive() const { return m_isActive; }

        void setActive(bool active);
        double getElapsedTime() const;
};

class BroadcastBeam {
    private:
        int m_senderId;
        float m_posX, m_posY;
        std::chrono::system_clock::time_point m_timestamp;
        bool m_isActive;

    public:
        BroadcastBeam();
        BroadcastBeam(int senderId, float posX, float posY);
        // Getters
        int getSenderId() const { return m_senderId; }
        float getPosX() const { return m_posX; }
        float getPosY() const { return m_posY; }
        bool isActive() const { return m_isActive; }

        void setActive(bool active);
        double getElapsedTime() const;
};

class InteractionManager {
    private:
        std::vector<PlayerExpulsion> m_expulsions;
        std::vector<BroadcastBeam> m_broadcastBeams;
        std::vector<PlayerDeath> m_deaths;
        std::vector<PlayerIncantation> m_incantations;
        std::vector<IncantationFailure> m_incantationFailures;
        double m_expulsionDisplayTime;
        double m_broadcastDisplayTime;
        double m_deathDisplayTime;
        double m_incantationDisplayTime;
        double m_incantationFailureDisplayTime;

    public:
        InteractionManager();

        void addExpulsion(int expellingPlayerId, int playerX, int playerY);
        std::vector<PlayerExpulsion> getActiveExpulsions() const;

        void addBroadcastBeam(int senderId, float posX, float posY);
        std::vector<BroadcastBeam> getActiveBroadcastBeams() const;

        void addIncantation(int playerId, int playerX, int playerY);
        void endIncantation(int playerId);
        std::vector<PlayerIncantation> getActiveIncantations() const;

        void addIncantationFailure(int playerX, int playerY);
        std::vector<IncantationFailure> getActiveIncantationFailures() const;

        void addDeath(int playerId, int playerX, int playerY);
        std::vector<PlayerDeath> getActiveDeaths() const;

        void updateInteractions();
        
        // Nouvelle méthode pour adapter la durée d'affichage des incantations
        void setIncantationDisplayTime(double duration);

        double getExpulsionDisplayTime() const { return m_expulsionDisplayTime; }
};


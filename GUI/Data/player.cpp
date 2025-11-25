/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** data.cpp
*/

#include "gui.hpp"
#include "Menu/Menu.hpp"
#include "Settings/Setting.hpp"
#include "Themes/Theme.hpp"
#include <cstring>
#include <unistd.h>
#include <sstream>
#include <map>
#include <cmath>
#include <algorithm>

void ZappyGUI::get_world_data(const std::string& message)
{
    std::istringstream iss(message);
    std::string command;

    iss >> command;
    if (command == "msz") {
        int width, height;
        if (iss >> width >> height) {
            m_world.setSize(width, height);
            std::cout << "[GUI DEBUG] 🗺️  Taille du monde mise à jour: " << width << "x" << height << std::endl;
        } else {
            std::cerr << "[GUI ERROR] Format invalide pour msz: " << message << std::endl;
        }
    }
    else if (command == "bct") {
        int x, y, q0, q1, q2, q3, q4, q5, q6;
        if (iss >> x >> y >> q0 >> q1 >> q2 >> q3 >> q4 >> q5 >> q6) {
            try {
                m_world.updateTile(x, y, q0, q1, q2, q3, q4, q5, q6);
            } catch (const std::exception& e) {
                std::cerr << "[GUI ERROR] Erreur lors de la mise à jour de la case: " << e.what() << std::endl;
            }
        } else {
            std::cerr << "[GUI ERROR] Format invalide pour bct: " << message << std::endl;
        }
    }
}

void ZappyGUI::get_team_data(const std::string& message)
{
    std::istringstream iss(message);
    std::string command;

    iss >> command;
    if (command == "tna") {
        std::string teamName;
        if (iss >> teamName && !teamName.empty()) {
            bool teamExists = false;
            for (const auto& team : m_teams) {
                if (team.name == teamName) {
                    teamExists = true;
                    break;
                }
            }
            if (!teamExists) {
                m_teams.emplace_back(teamName);
                std::cout << "[GUI DEBUG] 👥 Nouvelle équipe ajoutée: " << teamName
                          << " (Total: " << m_teams.size() << " équipes)" << std::endl;
            } else {
                std::cout << "[GUI DEBUG] 👥 Équipe déjà connue: " << teamName << std::endl;
            }
        } else {
            std::cout << "[GUI DEBUG] 📋 Commande tna reçue (début de liste des équipes)" << std::endl;
        }
    }
}

void ZappyGUI::get_player_data(const std::string& message)
{
    std::istringstream iss(message);
    std::string command;

    iss >> command;
    if (command == "pnw") {
        handle_player_new(message);
    } else if (command == "ppo") {
        handle_player_position(message);
    }
}

void ZappyGUI::handle_player_new(const std::string& message)
{
    std::istringstream iss(message);
    std::string command, playerIdStr;
    int x, y, orientation, level;
    std::string teamName;

    if (!(iss >> command >> playerIdStr >> x >> y >> orientation >> level >> teamName)) {
        std::cerr << "[GUI ERROR] Format invalide pour pnw: " << message << std::endl;
        return;
    }

    int playerId = parse_player_id(playerIdStr, "pnw");
    if (playerId == -1) return;

    bool playerExists = update_existing_player(playerId, x, y, orientation, level, teamName);
    if (!playerExists) {
        create_new_player(playerId, x, y, orientation, level, teamName);
    }
}

void ZappyGUI::handle_player_position(const std::string& message)
{
    std::istringstream iss(message);
    std::string command, playerIdStr;
    int x, y, orientation;

    if (!(iss >> command >> playerIdStr >> x >> y >> orientation)) {
        std::cerr << "[GUI ERROR] Format invalide pour ppo: " << message << std::endl;
        return;
    }

    int playerId = parse_player_id(playerIdStr, "ppo");
    if (playerId == -1) return;

    bool playerFound = false;
    for (auto& player : m_players) {
        if (player.id == playerId) {
            player.x = x;
            player.y = y;
            player.orientation = orientation;
            playerFound = true;
            std::cout << "[GUI DEBUG] 📍 Position joueur #" << playerId << " mise à jour: pos(" 
                      << x << "," << y << ") orient=" << orientation << std::endl;
            break;
        }
    }
    if (!playerFound) {
        std::cout << "[GUI DEBUG] ⚠️  Joueur #" << playerId << " non trouvé pour mise à jour position" << std::endl;
    }
}

int ZappyGUI::parse_player_id(const std::string& playerIdStr, const std::string& command)
{
    if (playerIdStr.length() <= 1 || playerIdStr[0] != '#') {
        std::cerr << "[GUI ERROR] Format ID joueur invalide dans " << command << ": " << playerIdStr << std::endl;
        return -1;
    }

    try {
        return std::stoi(playerIdStr.substr(1));
    } catch (const std::exception& e) {
        std::cerr << "[GUI ERROR] ID joueur invalide dans " << command << ": " << playerIdStr << std::endl;
        return -1;
    }
}

bool ZappyGUI::update_existing_player(int playerId, int x, int y, int orientation, int level, const std::string& teamName)
{
    for (auto& player : m_players) {
        if (player.id == playerId) {
            player.x = x;
            player.y = y;
            player.orientation = orientation;
            player.level = level;
            player.teamName = teamName;
            std::cout << "[GUI DEBUG] 🔄 Joueur #" << playerId << " mis à jour: pos("
                      << x << "," << y << ") orient=" << orientation
                      << " level=" << level << " team=" << teamName << std::endl;
            return true;
        }
    }
    return false;
}

void ZappyGUI::create_new_player(int playerId, int x, int y, int orientation, int level, const std::string& teamName)
{
    m_players.emplace_back(playerId, x, y, orientation, level, teamName);
    Player& newPlayer = m_players.back();
    if (!newPlayer.sprite.loadTextures()) {
        std::cerr << "[GUI ERROR] Erreur lors du chargement des textures pour le joueur #" << playerId << std::endl;
    } else {
        newPlayer.sprite.setPlayerLevel(level);
        newPlayer.sprite.startAnimation();
    }
    std::cout << "[GUI DEBUG] 🆕 Nouveau joueur #" << playerId << " ajouté: pos("
              << x << "," << y << ") orient=" << orientation
              << " level=" << level << " team=" << teamName << std::endl;
    Team* team = const_cast<Team*>(getTeamByName(teamName));
    if (team) {
        team->playerIds.insert(playerId);
        std::cout << "[GUI DEBUG] 👥 Joueur #" << playerId << " ajouté à l'équipe " << teamName << std::endl;
    } else {
        std::cout << "[GUI DEBUG] ⚠️  Équipe " << teamName << " non trouvée pour le joueur #" << playerId << std::endl;
    }
}

void ZappyGUI::get_player_level_data(const std::string& message)
{
    std::istringstream iss(message);
    std::string command, playerIdStr;
    int level;

    if (!(iss >> command >> playerIdStr >> level)) {
        std::cerr << "[GUI ERROR] Format invalide pour plv: " << message << std::endl;
        return;
    }
    int playerId = -1;
    if (playerIdStr.length() > 1 && playerIdStr[0] == '#') {
        try {
            playerId = std::stoi(playerIdStr.substr(1));
        } catch (const std::exception& e) {
            std::cerr << "[GUI ERROR] ID joueur invalide dans plv: " << playerIdStr << std::endl;
            return;
        }
    } else {
        std::cerr << "[GUI ERROR] Format ID joueur invalide dans plv: " << playerIdStr << std::endl;
        return;
    }
    bool playerFound = false;
    for (auto& player : m_players) {
        if (player.id == playerId) {
            int oldLevel = player.level;
            player.level = level;
            playerFound = true;
            std::cout << "[GUI INFO] 📊 Niveau joueur #" << playerId << " mis à jour: "
                      << oldLevel << " → " << level << std::endl;
            break;
        }
    }
    if (!playerFound) {
        std::cout << "[GUI WARNING] ⚠️  Joueur #" << playerId << " non trouvé pour mise à jour niveau" << std::endl;
    }
}

void ZappyGUI::get_player_inventory_data(const std::string& message)
{
    std::istringstream iss(message);
    std::string command, playerIdStr;
    int x, y, food, linemate, deraumere, sibur, mendiane, phiras, thystame;

    if (!(iss >> command >> playerIdStr >> x >> y >> food >> linemate >> deraumere >> sibur >> mendiane >> phiras >> thystame)) {
        std::cerr << "[GUI ERROR] Format invalide pour pin: " << message << std::endl;
        return;
    }
    int playerId = -1;
    if (playerIdStr.length() > 1 && playerIdStr[0] == '#') {
        try {
            playerId = std::stoi(playerIdStr.substr(1));
        } catch (const std::exception& e) {
            std::cerr << "[GUI ERROR] ID joueur invalide dans pin: " << playerIdStr << std::endl;
            return;
        }
    } else {
        std::cerr << "[GUI ERROR] Format ID joueur invalide dans pin: " << playerIdStr << std::endl;
        return;
    }
    bool playerFound = false;
    for (auto& player : m_players) {
        if (player.id == playerId) {
            bool positionChanged = (player.x != x || player.y != y);
            player.x = x;
            player.y = y;
            player.inventory = PlayerInventory(food, linemate, deraumere, sibur, mendiane, phiras, thystame);
            playerFound = true;
            std::cout << "[GUI INFO] 🎒 Inventaire joueur #" << playerId << " mis à jour:" << std::endl;
            std::cout << "  Position: (" << x << "," << y << ")" << (positionChanged ? " [CHANGÉE]" : "") << std::endl;
            std::cout << "  Inventaire: food=" << food << " linemate=" << linemate << " deraumere=" << deraumere
                      << " sibur=" << sibur << " mendiane=" << mendiane << " phiras=" << phiras
                      << " thystame=" << thystame << std::endl;
            break;
        }
    }
    if (!playerFound) {
        std::cout << "[GUI WARNING] ⚠️  Joueur #" << playerId << " non trouvé pour mise à jour inventaire" << std::endl;
    }
}

void ZappyGUI::remove_player(int playerId)
{
    auto it = std::remove_if(m_players.begin(), m_players.end(),
        [playerId](const Player& player) {
            return player.id == playerId;
        });
    if (it != m_players.end()) {
        m_players.erase(it, m_players.end());
        for (auto& team : m_teams) {
            auto teamIt = team.playerIds.find(playerId);
            if (teamIt != team.playerIds.end()) {
                team.playerIds.erase(teamIt);
                std::cout << "[GUI DEBUG] 👥 Joueur #" << playerId << " retiré de l'équipe " << team.name << std::endl;
                break;
            }
        }
    }
}

const Team* ZappyGUI::getTeamByName(const std::string& name) const
{
    for (const auto& team : m_teams) {
        if (team.name == name) {
            return &team;
        }
    }
    return nullptr;
}

const Player* ZappyGUI::getPlayerById(int playerId) const
{
    for (const auto& player : m_players) {
        if (player.id == playerId) {
            return &player;
        }
    }
    return nullptr;
}

sf::Vector2f ZappyGUI::getPlayerBaseTilePosition(int x, int y) const
{
    if (game_bk_texture.getSize().x == 0)
        return sf::Vector2f(0, 0);
    return const_cast<ZappyGUI*>(this)->getCachedTilePosition(x, y);
}

sf::Vector2f ZappyGUI::calculatePlayerOffset(sf::Vector2f basePos, size_t playerIndex, size_t totalPlayers, float spriteScale) const
{
    if (totalPlayers == 1) {
        return basePos;
    }
    const float BASE_OFFSET_RADIUS = 20.0f;
    const float BASE_OFFSET_SPACING = 15.0f;
    float OFFSET_RADIUS = BASE_OFFSET_RADIUS * spriteScale;
    float OFFSET_SPACING = BASE_OFFSET_SPACING * spriteScale;
    if (totalPlayers <= 4) {
        float angle = (2.0f * M_PI * playerIndex) / totalPlayers;
        float offsetX = OFFSET_RADIUS * cos(angle);
        float offsetY = OFFSET_RADIUS * sin(angle);
        return sf::Vector2f(basePos.x + offsetX, basePos.y + offsetY);
    } else {
        int cols = static_cast<int>(ceil(sqrt(totalPlayers)));
        int row = playerIndex / cols;
        int col = playerIndex % cols;
        float totalWidth = (cols - 1) * OFFSET_SPACING;
        float totalHeight = ((totalPlayers - 1) / cols) * OFFSET_SPACING;
        float offsetX = col * OFFSET_SPACING - totalWidth / 2.0f;
        float offsetY = row * OFFSET_SPACING - totalHeight / 2.0f;
        return sf::Vector2f(basePos.x + offsetX, basePos.y + offsetY);
    }
}

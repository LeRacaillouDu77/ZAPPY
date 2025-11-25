/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** network.cpp
*/

#include "network.hpp"
#include "../gui.hpp"
#include <cstring>
#include <unistd.h>
#include <sstream>

Network::Network()
    : m_port(0), m_connected(false), m_serverFrequency(100),
      m_isLoading(false), m_expectedTiles(0), m_receivedTiles(0),
      m_gui(nullptr), m_world(nullptr), m_interactionManager(nullptr)
{
}

Network::~Network()
{
    disconnect();
}

bool Network::connectToServer(const std::string& hostname, int port)
{
    m_hostname = hostname;
    m_port = port;
    m_socket.setBlocking(true);
    sf::Socket::Status status = m_socket.connect(m_hostname, m_port);

    if (status == sf::Socket::Done) {
        std::string authMessage = "GRAPHIC\n";
        std::cout << "[GUI DEBUG] Envoi du message d'authentification: '" << authMessage.substr(0, authMessage.length()-1) << "'" << std::endl;
        if (m_socket.send(authMessage.c_str(), authMessage.length()) != sf::Socket::Done) {
            std::cerr << "Erreur: Impossible d'envoyer l'authentification GRAPHIC" << std::endl;
            return false;
        }
        m_socket.setBlocking(false);

        m_connected = true;
        std::string statusMessage = "Connecte au serveur " + m_hostname + ":" + std::to_string(m_port) + " (GRAPHIC)";
        updateConnectionStatus(true, statusMessage);
        std::cout << "Connexion réussie au serveur " << m_hostname << ":" << m_port << " avec authentification GRAPHIC" << std::endl;
        sendMessage("sgt");
        return true;
    } else {
        m_connected = false;
        std::string statusMessage = "Échec de connexion au serveur " + m_hostname + ":" + std::to_string(m_port);
        updateConnectionStatus(false, statusMessage);
        std::cerr << "Erreur: Impossible de se connecter au serveur" << std::endl;
        return false;
    }
}

void Network::disconnect()
{
    if (m_socket.getLocalPort() != 0) {
        m_socket.disconnect();
    }
    m_connected = false;
    updateConnectionStatus(false, "Déconnecté");
}

bool Network::isConnected() const
{
    return m_connected;
}

void Network::sendMessage(const std::string& message)
{
    if (m_connected && m_socket.getLocalPort() != 0) {
        std::string messageWithNewline = message + "\n";
        if (sendDataComplete(messageWithNewline)) {
            std::cout << "[GUI DEBUG] Message envoyé au serveur: '" << message << "'" << std::endl;
        } else {
            std::cout << "[GUI DEBUG] Erreur lors de l'envoi du message au serveur" << std::endl;
        }
    }
}

void Network::update()
{
    if (m_connected) {
        char buffer[1024];
        std::size_t received;

        sf::Socket::Status status = m_socket.receive(buffer, sizeof(buffer) - 1, received);

        if (status == sf::Socket::Done && received > 0) {
            buffer[received] = '\0';
            m_receiveBuffer += std::string(buffer);
            std::string::size_type pos = 0;
            std::string::size_type newlinePos = 0;

            while ((newlinePos = m_receiveBuffer.find('\n', pos)) != std::string::npos) {
                std::string completeMessage = m_receiveBuffer.substr(pos, newlinePos - pos);
                if (!completeMessage.empty()) {
                    processServerMessage(completeMessage);
                }
                pos = newlinePos + 1;
            }
            if (pos < m_receiveBuffer.length()) {
                m_receiveBuffer = m_receiveBuffer.substr(pos);
            } else {
                m_receiveBuffer.clear();
            }
        } else if (status == sf::Socket::Disconnected) {
            std::cout << "[GUI DEBUG] Connexion fermée par le serveur" << std::endl;
            m_connected = false;
            updateConnectionStatus(false, "Connexion fermée par le serveur");
        } else if (status == sf::Socket::Error) {
            std::cout << "[GUI DEBUG] Erreur de réception" << std::endl;
        }
    }
}

void Network::setGuiReference(ZappyGUI* gui)
{
    m_gui = gui;
}

void Network::setWorldReference(World* world)
{
    m_world = world;
}

void Network::setInteractionManagerReference(InteractionManager* interactionManager)
{
    m_interactionManager = interactionManager;
}

void Network::setServerFrequency(int frequency)
{
    m_serverFrequency = frequency;
}

const std::string& Network::getReceiveBuffer() const
{
    return m_receiveBuffer;
}

int Network::getServerFrequency() const
{
    return m_serverFrequency;
}

void Network::setOnConnectionStatusChanged(std::function<void(bool, const std::string&)> callback)
{
    m_onConnectionStatusChanged = callback;
}

void Network::setOnLoadingStatusChanged(std::function<void(bool, int, int)> callback)
{
    m_onLoadingStatusChanged = callback;
}

bool Network::sendDataComplete(const std::string& data)
{
    std::size_t totalSent = 0;
    std::size_t dataSize = data.length();

    while (totalSent < dataSize) {
        std::size_t sent;
        sf::Socket::Status status = m_socket.send(
            data.c_str() + totalSent,
            dataSize - totalSent,
            sent
        );

        if (status == sf::Socket::Done || status == sf::Socket::Partial) {
            totalSent += sent;
        } else if (status == sf::Socket::Disconnected) {
            std::cout << "[GUI DEBUG] Connexion fermée par le serveur" << std::endl;
            m_connected = false;
            return false;
        } else {
            std::cout << "[GUI DEBUG] Erreur réseau lors de l'envoi" << std::endl;
            return false;
        }
    }
    return true;
}

void Network::updateConnectionStatus(bool connected, const std::string& statusMessage)
{
    if (m_onConnectionStatusChanged) {
        m_onConnectionStatusChanged(connected, statusMessage);
    }
}

void Network::updateLoadingStatus(bool loading, int received, int expected)
{
    if (m_onLoadingStatusChanged) {
        m_onLoadingStatusChanged(loading, received, expected);
    }
}

void Network::processServerMessage(const std::string& message)
{
    if (message.empty() || !m_gui)
        return;
    if (message == "WELCOME") {
        processWelcomeMessage();
    } else if (message.substr(0, 3) == "msz") {
        m_gui->get_world_data(message);
        if (m_world) {
            m_expectedTiles = m_world->getWidth() * m_world->getHeight();
            updateLoadingStatus(m_isLoading, m_receivedTiles, m_expectedTiles);
            std::cout << "[GUI DEBUG] 📏 Taille du monde reçue, " << m_expectedTiles << " tiles attendues" << std::endl;
        }
    } else if (message.substr(0, 3) == "bct") {
        m_gui->get_world_data(message);
        m_receivedTiles++;
        updateLoadingStatus(m_isLoading, m_receivedTiles, m_expectedTiles);
        if (m_receivedTiles >= m_expectedTiles && m_isLoading) {
            m_isLoading = false;
            updateLoadingStatus(false, m_receivedTiles, m_expectedTiles);
            std::cout << "[GUI DEBUG] ✅ Chargement terminé ! Toutes les tiles reçues (" << m_receivedTiles << "/" << m_expectedTiles << ")" << std::endl;
        }
    } else if (message.substr(0, 3) == "mct") {
        std::cout << "[GUI DEBUG] 🌍 Début de la carte complète" << std::endl;
    } else if (message.substr(0, 3) == "tna") {
        m_gui->get_team_data(message);
    } else if (message.substr(0, 3) == "pnw") {
        m_gui->get_player_data(message);
        std::istringstream iss(message);
        std::string command, player_id;
        if (iss >> command >> player_id) {
            sendMessage("pin " + player_id);
        }
    } else if (message.substr(0, 3) == "ppo") {
        m_gui->get_player_data(message);
    } else if (message.substr(0, 3) == "plv") {
        m_gui->get_player_level_data(message);
    } else if (message.substr(0, 3) == "pin") {
        m_gui->get_player_inventory_data(message);
    } else if (message.substr(0, 3) == "pex") {
        std::istringstream iss(message);
        std::string command, playerIdStr;
        if (iss >> command >> playerIdStr && !playerIdStr.empty() && playerIdStr[0] == '#') {
            int playerId = std::stoi(playerIdStr.substr(1));
            const Player* expellingPlayer = m_gui->getPlayerById(playerId);
            if (expellingPlayer != nullptr && m_interactionManager)
                m_interactionManager->addExpulsion(playerId, expellingPlayer->x, expellingPlayer->y);
        }
    } else if (message.substr(0, 3) == "pbc") {
        processPBCCommand(message);
    } else if (message.substr(0, 3) == "pic") {
        processPICCommand(message);
    } else if (message.substr(0, 3) == "pie") {
        processPIECommand(message);
    } else if (message.substr(0, 3) == "pfk") {
        m_gui->get_egg_data(message);
    } else if (message.substr(0, 3) == "pdr") {
        processPDRCommand(message);
    } else if (message.substr(0, 3) == "pgt") {
        processPGTCommand(message);
    } else if (message.substr(0, 3) == "pdi") {
        std::istringstream iss(message);
        std::string command, playerIdStr;
        if (iss >> command >> playerIdStr && !playerIdStr.empty() && playerIdStr[0] == '#') {
            int playerId = std::stoi(playerIdStr.substr(1));
            const Player* dyingPlayer = m_gui->getPlayerById(playerId);
            if (dyingPlayer != nullptr && m_interactionManager) {
                m_interactionManager->addDeath(playerId, dyingPlayer->x, dyingPlayer->y);
                m_gui->remove_player(playerId);
            }
            std::cout << "[GUI DEBUG] 💀 Mort du joueur " << playerId << ": " << message << std::endl;
        }
    } else if (message.substr(0, 3) == "enw") {
        m_gui->get_egg_data(message);
    } else if (message.substr(0, 3) == "ebo") {
        m_gui->get_egg_data(message);
    } else if (message.substr(0, 3) == "edi") {
        m_gui->get_egg_data(message);
    } else if (message.substr(0, 3) == "sgt") {
        processSGTCommand(message);
    } else if (message.substr(0, 3) == "sst") {
        processSSTCommand(message);
    } else if (message.substr(0, 3) == "seg") {
        std::cout << "[GUI DEBUG] 🏆 Fin de partie: " << message << std::endl;
        std::istringstream iss(message);
        std::string command, winningTeam;
        if (iss >> command >> winningTeam && !winningTeam.empty()) {
            std::cout << "[GUI INFO] 🎉 Équipe gagnante: " << winningTeam << std::endl;
            if (m_gui) {
                m_gui->showGameEndScreen(winningTeam);
            }
        } else {
            std::cout << "[GUI ERROR] ❌ Format invalide pour commande seg: " << message << std::endl;
        }
    } else if (message.substr(0, 3) == "smg") {
        std::cout << "[GUI DEBUG] 💬 Message du serveur: " << message << std::endl;
    } else if (message.substr(0, 3) == "suc") {
        std::cout << "[GUI DEBUG] ❌ Commande inconnue" << std::endl;
    } else if (message.substr(0, 3) == "sbp") {
        std::cout << "[GUI DEBUG] ⚠️  Paramètre de commande invalide" << std::endl;
    } else {
        std::cout << "[GUI DEBUG] ❓ Message non reconnu: " << message << std::endl;
    }
}

void Network::processPBCCommand(const std::string& message)
{
    if (!m_gui || !m_interactionManager)
        return;
    std::istringstream iss(message);
    std::string command, playerIdStr;
    if (iss >> command >> playerIdStr && !playerIdStr.empty() && playerIdStr[0] == '#') {
        try {
            int senderId = std::stoi(playerIdStr.substr(1));
            const Player* sender = m_gui->getPlayerById(senderId);
            if (sender != nullptr) {
                if (m_gui->getGameBackgroundTexture().getSize().x > 0) {
                    float scaleX = static_cast<float>(m_gui->getWindow()->getSize().x) / m_gui->getGameBackgroundTexture().getSize().x;
                    float scaleY = static_cast<float>(m_gui->getWindow()->getSize().y) / m_gui->getGameBackgroundTexture().getSize().y;
                    const float TOP_X = 360.f * scaleX;
                    const float TOP_Y = 62.f * scaleY;
                    const int NUM_CAMPS_X = m_world->getWidth();
                    const int NUM_CAMPS_Y = m_world->getHeight();
                    float TILE_WIDTH = (570.f * scaleX) / NUM_CAMPS_X;
                    float TILE_HEIGHT = (410.f * scaleY) / NUM_CAMPS_Y;
                    float senderScreenX = TOP_X + (sender->x - sender->y) * (TILE_WIDTH / 2.f);
                    float senderScreenY = TOP_Y + (sender->x + sender->y) * (TILE_HEIGHT / 2.f);
                    m_interactionManager->addBroadcastBeam(senderId, senderScreenX, senderScreenY);
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Impossible de parser l'ID du joueur dans pbc: " << playerIdStr << std::endl;
        }
    }
}

void Network::processPDRCommand(const std::string& message)
{
    if (!m_gui || !m_world)
        return;
    std::istringstream iss(message);
    std::string command, playerIdStr;
    int resourceType;

    if (iss >> command >> playerIdStr >> resourceType && !playerIdStr.empty() && playerIdStr[0] == '#') {
        try {
            int playerId = std::stoi(playerIdStr.substr(1));
            const Player* player = m_gui->getPlayerById(playerId);
            if (player != nullptr) {
                if (m_world->hasTile(player->x, player->y)) {
                    Tile& currentTile = m_world->getTile(player->x, player->y);
                    switch (resourceType) {
                        case 0: currentTile.food++; break;
                        case 1: currentTile.linemate++; break;
                        case 2: currentTile.deraumere++; break;
                        case 3: currentTile.sibur++; break;
                        case 4: currentTile.mendiane++; break;
                        case 5: currentTile.phiras++; break;
                        case 6: currentTile.thystame++; break;
                        default:
                            std::cerr << "[GUI ERROR] Type de ressource invalide pour pdr: " << resourceType << std::endl;
                            break;
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "[GUI ERROR] Impossible de parser l'ID du joueur dans pdr: " << playerIdStr << std::endl;
        }
    }
}

void Network::processPGTCommand(const std::string& message)
{
    if (!m_gui || !m_world)
        return;
    std::istringstream iss(message);
    std::string command, playerIdStr;
    int resourceType;

    if (iss >> command >> playerIdStr >> resourceType && !playerIdStr.empty() && playerIdStr[0] == '#') {
        try {
            int playerId = std::stoi(playerIdStr.substr(1));
            const Player* player = m_gui->getPlayerById(playerId);
            if (player != nullptr) {
                if (m_world->hasTile(player->x, player->y)) {
                    Tile& currentTile = m_world->getTile(player->x, player->y);
                    switch (resourceType) {
                        case 0: if (currentTile.food > 0) currentTile.food--; break;
                        case 1: if (currentTile.linemate > 0) currentTile.linemate--; break;
                        case 2: if (currentTile.deraumere > 0) currentTile.deraumere--; break;
                        case 3: if (currentTile.sibur > 0) currentTile.sibur--; break;
                        case 4: if (currentTile.mendiane > 0) currentTile.mendiane--; break;
                        case 5: if (currentTile.phiras > 0) currentTile.phiras--; break;
                        case 6: if (currentTile.thystame > 0) currentTile.thystame--; break;
                        default: break;
                    }
                }
                sendMessage("pin " + playerIdStr);
            }
        } catch (const std::exception& e) {
            std::cerr << "[GUI ERROR] Impossible de parser l'ID du joueur dans pgt: " << playerIdStr << std::endl;
        }
    }
}

void Network::processPICCommand(const std::string& message)
{
    if (!m_gui || !m_interactionManager)
        return;
    std::istringstream iss(message);
    std::string command, playerIdStr;
    int x, y, level;

    if (iss >> command >> x >> y >> level >> playerIdStr && !playerIdStr.empty() && playerIdStr[0] == '#') {
        int playerId = std::stoi(playerIdStr.substr(1));
        const Player* player = m_gui->getPlayerById(playerId);
        if (player != nullptr) {
            std::cout << "[GUI DEBUG] 🔮 Début d'incantation du joueur " << playerId
                << " à la position (" << x << "," << y << ") niveau " << level << std::endl;
            m_interactionManager->addIncantation(playerId, x, y);
            auto activeIncantations = m_interactionManager->getActiveIncantations();
            std::cout << "[GUI DEBUG] 📊 Nombre d'incantations actives après ajout: " << activeIncantations.size() << std::endl;
            for (const auto& inc : activeIncantations) {
                std::cout << "[GUI DEBUG] 📍 Incantation active - Joueur " << inc.getPlayerId() 
                          << " à (" << inc.getPlayerX() << "," << inc.getPlayerY() << ")" << std::endl;
            }
        } else {
            std::cout << "[GUI DEBUG] ❌ Joueur " << playerId << " non trouvé pour PIC" << std::endl;
        }
    }
}

void Network::processPIECommand(const std::string& message)
{
    if (!m_interactionManager)
        return;
    std::istringstream iss(message);
    std::string command;
    int x, y, result;

    if (iss >> command >> x >> y >> result) {
        if (result == 1) {
            auto activeIncantations = m_interactionManager->getActiveIncantations();
            for (const auto& incantation : activeIncantations) {
                if (incantation.getPlayerX() == x && incantation.getPlayerY() == y && incantation.isActive()) {
                    m_interactionManager->endIncantation(incantation.getPlayerId());
                    std::cout << "[GUI DEBUG] 🎉 Incantation du joueur " << incantation.getPlayerId()
                              << " terminée avec succès" << std::endl;
                    break;
                }
            }
        } else {
            m_interactionManager->addIncantationFailure(x, y);
            auto activeIncantations = m_interactionManager->getActiveIncantations();
            for (const auto& incantation : activeIncantations) {
                if (incantation.getPlayerX() == x && incantation.getPlayerY() == y && incantation.isActive()) {
                    m_interactionManager->endIncantation(incantation.getPlayerId());
                    std::cout << "[GUI DEBUG] 💥 Incantation du joueur " << incantation.getPlayerId()
                              << " terminée en échec" << std::endl;
                    break;
                }
            }
        }
    }
}

void Network::processSGTCommand(const std::string& message)
{
    if (!m_gui || !m_interactionManager)
        return;
    std::istringstream iss(message);
    std::string command;
    int timeUnit;

    if (iss >> command >> timeUnit) {
        m_serverFrequency = timeUnit;
        float incantationDuration = m_gui->getIncantationAnimationDuration();
        m_interactionManager->setIncantationDisplayTime(incantationDuration);
        std::cout << "[GUI DEBUG] ⏰ Unité de temps reçue: " << timeUnit << std::endl;
        std::cout << "[GUI DEBUG] 🎬 Durée d'animation d'incantation adaptée: "
                  << incantationDuration << "s" << std::endl;
    } else {
        std::cout << "[GUI ERROR] ❌ Format invalide pour commande sgt: " << message << std::endl;
    }
}

void Network::processSSTCommand(const std::string& message)
{
    if (!m_gui || !m_interactionManager)
        return;
    std::istringstream iss(message);
    std::string command;
    int newTimeUnit;

    if (iss >> command >> newTimeUnit) {
        m_serverFrequency = newTimeUnit;
        float incantationDuration = m_gui->getIncantationAnimationDuration();
        m_interactionManager->setIncantationDisplayTime(incantationDuration);
        std::cout << "[GUI DEBUG] ⏱️  Fréquence serveur modifiée: " << newTimeUnit << std::endl;
        std::cout << "[GUI DEBUG] 🎬 Durée d'animation d'incantation adaptée: "
                  << incantationDuration << "s" << std::endl;
    } else {
        std::cout << "[GUI ERROR] ❌ Format invalide pour commande sst: " << message << std::endl;
    }
}

void Network::processWelcomeMessage()
{
    if (!m_gui)
        return;
    m_isLoading = true;
    m_receivedTiles = 0;
    m_expectedTiles = 0;
    updateLoadingStatus(true, 0, 0);
    sendMessage("tna");
    sendMessage("msz");
    sendMessage("mct");
    std::cout << "[GUI DEBUG] 🎉 Bienvenue sur Zappy ! Chargement en cours..." << std::endl;
}
/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** network.hpp
*/

#pragma once
#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <functional>

// Forward declarations
class ZappyGUI;
class World;
class InteractionManager;

class INetwork {
public:
    virtual ~INetwork() = default;
    virtual bool connectToServer(const std::string& hostname, int port) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    virtual void sendMessage(const std::string& message) = 0;
    virtual void update() = 0;
    virtual void processServerMessage(const std::string& message) = 0;
};

class Network : public INetwork {
    public:
        Network();
        ~Network();
        bool connectToServer(const std::string& hostname, int port) override;
        void disconnect() override;
        bool isConnected() const override;
        void sendMessage(const std::string& message) override;
        void update() override;
        void processServerMessage(const std::string& message) override;
        void setGuiReference(ZappyGUI* gui);
        void setWorldReference(World* world);
        void setInteractionManagerReference(InteractionManager* interactionManager);
        void setServerFrequency(int frequency);
        const std::string& getReceiveBuffer() const;
        int getServerFrequency() const;
        void setOnConnectionStatusChanged(std::function<void(bool, const std::string&)> callback);
        void setOnLoadingStatusChanged(std::function<void(bool, int, int)> callback);
    private:
        sf::TcpSocket m_socket;
        std::string m_receiveBuffer;
        std::string m_hostname;
        int m_port;
        bool m_connected;
        int m_serverFrequency;
        bool m_isLoading;
        int m_expectedTiles;
        int m_receivedTiles;

        ZappyGUI* m_gui;
        World* m_world;
        InteractionManager* m_interactionManager;

        std::function<void(bool, const std::string&)> m_onConnectionStatusChanged;
        std::function<void(bool, int, int)> m_onLoadingStatusChanged;

        void processPBCCommand(const std::string& message);
        void processPDRCommand(const std::string& message);
        void processPGTCommand(const std::string& message);
        void processPICCommand(const std::string& message);
        void processPIECommand(const std::string& message);
        void processSGTCommand(const std::string& message);
        void processSSTCommand(const std::string& message);
        void processWelcomeMessage();
        bool sendDataComplete(const std::string& data);
        void updateConnectionStatus(bool connected, const std::string& statusMessage);
        void updateLoadingStatus(bool loading, int received, int expected);
};
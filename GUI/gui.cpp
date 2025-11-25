/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** gui.cpp
*/

#include "gui.hpp"
#include "Menu/Menu.hpp"
#include "Settings/Setting.hpp"
#include "Themes/Theme.hpp"
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <sstream>

ZappyGUI::ZappyGUI() : m_port(0), m_running(false)
{
    initializeWindow();
    loadResources();
    m_network = std::make_unique<Network>();
    m_network->setGuiReference(this);
    m_network->setWorldReference(&m_world);
    m_network->setInteractionManagerReference(&m_interactionManager);
    m_network->setOnConnectionStatusChanged([this](bool connected, const std::string& message) {
        onConnectionStatusChanged(connected, message);
    });
    m_network->setOnLoadingStatusChanged([this](bool loading, int received, int expected) {
        onLoadingStatusChanged(loading, received, expected);
    });
    m_menu = std::make_unique<Menu>(getWindow(), getFont(), 200.f, "Assets/MAIN_VILLAGE/basic_music.ogg");
    m_theme = std::make_unique<Theme>(getWindow(), getFont());
    m_setting = std::make_unique<Setting>(getWindow(), getFont(), &m_currentVolume);
    m_loadingScreen = std::make_unique<LoadingScreen>(getWindow(), &getFont());
    m_gameEndScreen = std::make_unique<GameEndScreen>(getWindow(), getFont());
    m_camera = Camera(sf::Vector2f(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f), 1.0f, sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    m_window->setView(m_camera.getView());
    m_menuButtonText.setFont(m_font);
    m_menuButtonText.setString("Menu");
    m_menuButtonText.setCharacterSize(33);
    m_menuButtonText.setFillColor(sf::Color::White);
    m_menuButtonText.setPosition(m_camera.getPosition().x, m_camera.getPosition().y);
    m_menuInfoButtonText.setFont(m_font);
    m_menuInfoButtonText.setString("Infos");
    m_menuInfoButtonText.setCharacterSize(33);
    m_menuInfoButtonText.setFillColor(sf::Color::White);
    m_menuInfoButtonText.setPosition(m_camera.getPosition().x + m_camera.getSize().x * 0.9, m_camera.getPosition().y + m_camera.getSize().y * 0.9);
    initializePlayerStatsUI();
    initializeTileStatsUI();
    m_icon.loadFromFile(ICON_PATH);
    m_window->setIcon(m_icon.getSize().x, m_icon.getSize().y, m_icon.getPixelsPtr());
}

ZappyGUI::~ZappyGUI()
{
    if (m_network) {
        m_network->disconnect();
    }
}

void ZappyGUI::initializeWindow()
{
    m_window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
        "Zappy GUI",
        sf::Style::Titlebar | sf::Style::Close
    );
    m_window->setFramerateLimit(60);
}

void ZappyGUI::loadResources()
{
    m_font.loadFromFile("Assets/supercell-magic.ttf");
    m_welcomeText.setFont(m_font);
    m_welcomeText.setString("Bienvenue sur Zappy");
    m_welcomeText.setCharacterSize(48);
    m_welcomeText.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = m_welcomeText.getLocalBounds();
    m_welcomeText.setPosition(
        (WINDOW_WIDTH - textBounds.width) / 2.0f,
        (WINDOW_HEIGHT - textBounds.height) / 2.0f - 50
    );

    m_statusText.setFont(m_font);
    m_statusText.setCharacterSize(24);
    m_statusText.setFillColor(sf::Color::Green);
    m_statusText.setPosition(50, WINDOW_HEIGHT - 100);

    if (!m_campfireTexture.loadFromFile("Assets/MAIN_VILLAGE/Camp.png")) {
        std::cerr << "Erreur de chargement de l'image Campfire" << std::endl;
    } else {
        m_campfireSprite.setTexture(m_campfireTexture);
        m_campfireSprite.setRotation(0.f);
    }
    m_explosionTexture.loadFromFile("Assets/Interaction/boom.png");
    m_explosionSprite.setTexture(m_explosionTexture);
    m_explosionSprite.setScale(0.15f, 0.15f);
    m_skullTexture.loadFromFile("Assets/Game/skull.png");
    m_skullSprite.setTexture(m_skullTexture);
    m_skullSprite.setScale(0.1f, 0.1f);    m_broadcastTexture.loadFromFile("Assets/Game/broadcast.png");
    m_broadcastSprite.setTexture(m_broadcastTexture);
    m_broadcastSprite.setScale(0.15f, 0.15f);
    m_incantationTexture.loadFromFile("Assets/Interaction/incant.png");
    m_incantationSprite.setTexture(m_incantationTexture);
    m_incantationSprite.setScale(0.4f, 0.4f);
    m_nopeTexture.loadFromFile("Assets/Game/nope.png");
    m_nopeSprite.setTexture(m_nopeTexture);
    m_nopeSprite.setScale(0.4f, 0.4f);
    m_eggTexture.loadFromFile("Assets/Game/egg.png");
    m_eggSprite.setTexture(m_eggTexture);
    m_eggSprite.setScale(0.15f, 0.15f);
    m_inGameMenu = std::make_unique<InGameMenu>(m_window.get(), m_font);
    m_inGameMenu->setVolumePointer(&m_currentVolume);
    m_inGameMenu->setCommandSender([this](const std::string& msg) {
        this->sendMessageToServer(msg);
    });
    m_inGameInfoMenu = std::make_unique<InGameInfoMenu>(m_window.get(), m_font);
}

bool ZappyGUI::parseArguments(int argc, char **argv)
{
    if (argc != 5) {
        return false;
    }

    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-p") == 0) {
            m_port = std::atoi(argv[i + 1]);
            if (m_port <= 0 || m_port > 65535) {
                std::cerr << "Erreur: Port invalide" << std::endl;
                return false;
            }
        } else if (strcmp(argv[i], "-h") == 0) {
            m_hostname = argv[i + 1];
        } else {
            return false;
        }
    }

    if (m_port == 0 || m_hostname.empty()) {
        return false;
    }

    return true;
}

bool ZappyGUI::connectToServer()
{
    if (!m_network) {
        std::cerr << "Erreur: Network non initialisé" << std::endl;
        return false;
    }
    return m_network->connectToServer(m_hostname, m_port);
}

void ZappyGUI::displayUsage(const std::string &programName)
{
    std::cout << "USAGE: " << programName << " -p port -h machine" << std::endl;
    std::cout << "option description" << std::endl;
    std::cout << "-p port        port number" << std::endl;
    std::cout << "-h machine     hostname of the server" << std::endl;
}

void ZappyGUI::loadSprite(const std::string& game_image_File)
{
    if (game_bk_texture.loadFromFile(game_image_File)) {
        game_bk_sprite.setTexture(game_bk_texture);
    }
}

void ZappyGUI::run() {
    m_running = true;
    m_currentTheme = getCurrentTheme();

    if (m_currentTheme == Themes_enum::THEME_BASE) {

        loadSprite("Assets/MAIN_VILLAGE/game_map.jpg");
        m_menu->changeTheme("Assets/MAIN_VILLAGE/basic_music.ogg");
    } else if (m_currentTheme == Themes_enum::THEME_NIGHT) {
        loadSprite("Assets/NIGHT_VILLAGE/game_map.png");
        m_menu->changeTheme("Assets/NIGHT_VILLAGE/night_music.ogg");
    } else if (m_currentTheme == Themes_enum::THEME_CAPITAL) {
        loadSprite("Assets/CAPITAL_VILLAGE/game_map.jpg");
        m_menu->changeTheme("Assets/CAPITAL_VILLAGE/capital_music.ogg");
    } else if (m_currentTheme == Themes_enum::THEME_WAR) {
        loadSprite("Assets/WAR_VILLAGE/game_map.jpg");
        m_menu->changeTheme("Assets/WAR_VILLAGE/war_music.ogg");
    }
    while (m_running && m_window->isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void ZappyGUI::handleEvents() {
    sf::Event event;
    while (m_window->pollEvent(event)) {
        sf::Vector2f mousePos = static_cast<sf::Vector2f>(
            sf::Mouse::getPosition(*m_window));
        if (m_menuButtonText.getGlobalBounds().contains(mousePos)) {
            m_menuButtonText.setFillColor(sf::Color::Green);
        } else {
            m_menuButtonText.setFillColor(sf::Color::White);
        }
        if (m_menuInfoButtonText.getGlobalBounds().contains(mousePos)) {
            m_menuInfoButtonText.setFillColor(sf::Color::Green);
        } else {
            m_menuInfoButtonText.setFillColor(sf::Color::White);
        }
        switch (event.type) {
            case sf::Event::Closed:
                m_running = false;
                m_window->close();
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape) {
                    if (m_showPlayerStats) {
                        m_showPlayerStats = false;
                        m_selectedPlayerId = -1;
                    } else if (m_showTileStats) {
                        m_showTileStats = false;
                        m_selectedTileX = -1;
                        m_selectedTileY = -1;
                    } else {
                        m_running = false;
                        m_window->close();
                    }
                }
                if (event.key.code == sf::Keyboard::Add || event.key.code == sf::Keyboard::P) {
                    m_camera.setZoom(m_camera.getZoom() * 1.1f);
                    m_window->setView(m_camera.getView());
                }
                if (event.key.code == sf::Keyboard::Subtract || event.key.code == sf::Keyboard::O) {
                    m_camera.setZoom(m_camera.getZoom() * 0.9f);
                    m_window->setView(m_camera.getView());
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) {
                    m_camera.move(sf::Vector2f(0, -10 / m_camera.getZoom()));
                    m_window->setView(m_camera.getView());
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
                    m_camera.move(sf::Vector2f(0, 10 / m_camera.getZoom()));
                    m_window->setView(m_camera.getView());
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
                    m_camera.move(sf::Vector2f(-10 / m_camera.getZoom(), 0));
                    m_window->setView(m_camera.getView());
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                    m_camera.move(sf::Vector2f(10 / m_camera.getZoom(), 0));
                    m_window->setView(m_camera.getView());
                }
                break;

            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = static_cast<sf::Vector2f>(
                        sf::Mouse::getPosition(*m_window)
                    );
                    if (m_menuButtonText.getGlobalBounds().contains(mousePos)) {
                        m_menuButtonText.setFillColor(sf::Color::Green);
                        m_showInGameMenu = true;
                    }
                    if (m_menuInfoButtonText.getGlobalBounds().contains(mousePos)) {
                        m_menuInfoButtonText.setFillColor(sf::Color::Green);
                        m_showInGameInfoMenu = true;
                    }
                    handlePlayerClick(mousePos);
                }
                break;
            default:
                break;
        }
    }
}

void ZappyGUI::update()
{
    if (m_connected) {
        float deltaTime = m_animationClock.restart().asSeconds();
        updatePlayerAnimations(deltaTime);
        if (m_network) {
            m_network->update();
        }
    }
}

void ZappyGUI::render()
{
    m_window->clear(sf::Color::Black);

    if (m_connected) {
        if (m_isLoading) {
            float deltaTime = m_loadingClock.restart().asSeconds();
            m_loadingScreen->update(deltaTime);
            m_loadingScreen->render();
            return;
        }
        if (game_bk_texture.getSize().x > 0) {
            RenderTiles();
            sf::View base_view = m_window->getView();
            renderEggs();
            renderPlayers();
            renderExplosions();
            renderBroadcastBeams();
            renderIncantations();
            renderIncantationFailures();
            renderDeaths();
            m_window->setView(m_window->getDefaultView());
            m_menuButtonText.setPosition(10, 10);
            m_window->draw(m_menuButtonText);
            m_menuInfoButtonText.setPosition(m_camera.getSize().x * 0.9, m_camera.getSize().y * 0.01);
            m_window->draw(m_menuInfoButtonText);
            m_window->setView(base_view);
        } else {
            displayWelcomeScreen();
        }
    } else {
        displayConnectionError();
    }
    renderPlayerStatsOverlay();
    renderTileStatsOverlay();
    if (m_showInGameMenu) {
        InGameMenuAction action = m_inGameMenu->show();
        if (action == INGAME_RESUME) {
            m_showInGameMenu = false;
            m_window->setView(m_camera.getView());
        }
        if (action == INGAME_BACK_TO_MENU) {
            m_showInGameMenu = false;
            if (m_network) {
                m_network->disconnect();
            }
            m_running = false;
        }
    } else if (m_showInGameInfoMenu) {
        update_info_page();
        InGameInfoMenuAction action = m_inGameInfoMenu->show();
        if (action == INGAME_INFO_RESUME) {
            m_showInGameInfoMenu = false;
            m_window->setView(m_camera.getView());
        }
    } else if (m_showGameEndScreen) {
        GameEndAction action = m_gameEndScreen->show();
        if (action == GAME_END_BACK_TO_MENU) {
            m_showGameEndScreen = false;
            if (m_network) {
                m_network->disconnect();
            }
            m_running = false;
        }
        if (action == GAME_END_CLOSE) {
            m_running = false;
            m_window->close();
        }
    }
    m_window->display();
}

void ZappyGUI::updatePlayerAnimations(float deltaTime)
{
    for (auto& player : m_players) {
        player.sprite.update(deltaTime);
    }
}

void ZappyGUI::displayWelcomeScreen()
{
    m_window->draw(m_welcomeText);
    m_window->draw(m_statusText);
}

void ZappyGUI::displayConnectionError()
{
    m_window->draw(m_statusText);

    sf::Text errorText;
    errorText.setFont(m_font);
    errorText.setString("Erreur de connexion");
    errorText.setCharacterSize(36);
    errorText.setFillColor(sf::Color::Red);

    sf::FloatRect bounds = errorText.getLocalBounds();
    errorText.setPosition(
        (WINDOW_WIDTH - bounds.width) / 2.0f,
        (WINDOW_HEIGHT - bounds.height) / 2.0f
    );
    m_window->draw(errorText);
}

void ZappyGUI::draw_tile(Tile& tile, sf::Vector2f tile_pos, sf::Vector2f tile_half_size)
{
    tile.setPosition(tile_pos.x, tile_pos.y);

    if (tile.food > 0) {
        tile.m_food_sprite.setPosition(tile_pos.x + tile_half_size.x, tile_pos.y + tile_half_size.y);
        tile.m_food_sprite.setScale(tile_half_size.x / tile.m_food_sprite.getTexture()->getSize().x * 0.3,
            tile_half_size.y / tile.m_food_sprite.getTexture()->getSize().y * 0.4);
        m_window->draw(tile.m_food_sprite);
    }
    if (tile.linemate > 0) {
        tile.m_linemate_sprite.setPosition(tile_pos.x + tile_half_size.x - tile_half_size.x / 3, tile_pos.y + tile_half_size.y);
        tile.m_linemate_sprite.setScale(tile_half_size.x / tile.m_linemate_sprite.getTexture()->getSize().x * 0.5,
            tile_half_size.y / tile.m_linemate_sprite.getTexture()->getSize().y * 0.5);
        m_window->draw(tile.m_linemate_sprite);
    }
    if (tile.deraumere > 0) {
        tile.m_deraumere_sprite.setPosition(tile_pos.x + tile_half_size.x  - tile_half_size.x / 2, tile_pos.y + tile_half_size.y);
        tile.m_deraumere_sprite.setScale(tile_half_size.x / tile.m_deraumere_sprite.getTexture()->getSize().x * 0.5,
            tile_half_size.y / tile.m_deraumere_sprite.getTexture()->getSize().y * 0.5);
        m_window->draw(tile.m_deraumere_sprite);
    }
    if (tile.sibur > 0) {
        tile.m_sibur_sprite.setPosition(tile_pos.x + tile_half_size.x + tile_half_size.x / 3, tile_pos.y + tile_half_size.y);
        tile.m_sibur_sprite.setScale(tile_half_size.x / tile.m_sibur_sprite.getTexture()->getSize().x * 0.5,
            tile_half_size.y / tile.m_sibur_sprite.getTexture()->getSize().y * 0.5);
        m_window->draw(tile.m_sibur_sprite);
    }
    if (tile.mendiane > 0) {
        tile.m_mendiane_sprite.setPosition(tile_pos.x + tile_half_size.x + tile_half_size.x / 2.5, tile_pos.y + tile_half_size.y);
        tile.m_mendiane_sprite.setScale(tile_half_size.x / tile.m_mendiane_sprite.getTexture()->getSize().x * 0.4,
            tile_half_size.y / tile.m_mendiane_sprite.getTexture()->getSize().y * 0.4);
        m_window->draw(tile.m_mendiane_sprite);
    }
    if (tile.phiras > 0) {
        tile.m_phiras_sprite.setPosition(tile_pos.x + tile_half_size.x + tile_half_size.x / 2, tile_pos.y + tile_half_size.y);
        tile.m_phiras_sprite.setScale(tile_half_size.x / tile.m_phiras_sprite.getTexture()->getSize().x * 0.25,
            tile_half_size.y / tile.m_phiras_sprite.getTexture()->getSize().y * 0.25);
        m_window->draw(tile.m_phiras_sprite);
    }
    if (tile.thystame > 0) {
        tile.m_thystame_sprite.setPosition(tile_pos.x + tile_half_size.x - tile_half_size.x / 1.5, tile_pos.y + tile_half_size.y);
        tile.m_thystame_sprite.setScale(tile_half_size.x / tile.m_thystame_sprite.getTexture()->getSize().x * 0.25,
            tile_half_size.y / tile.m_thystame_sprite.getTexture()->getSize().y * 0.25);
        m_window->draw(tile.m_thystame_sprite);
    }
}

void ZappyGUI::showGameEndScreen(const std::string& winningTeam)
{
    m_winningTeam = winningTeam;
    m_showGameEndScreen = true;
    m_showInGameMenu = false;
    m_showInGameInfoMenu = false;

    m_window->setView(m_window->getDefaultView());

    if (m_gameEndScreen) {
        m_gameEndScreen->setWinningTeam(winningTeam);
    }
}

int main(int argc, char **argv)
{
    ZappyGUI gui;

    while (true) {
        gui.setCurrentAction(gui.getMenu()->show());
        if (gui.getCurrentAction() == ACTION_EXIT) {
            break;
        }
        if (gui.getCurrentAction() == ACTION_SETTINGS) {
            gui.getSetting()->show();
            continue;
        }
        if (gui.getCurrentAction() == ACTION_START) {
            if (!gui.parseArguments(argc, argv)) {
                gui.displayUsage(argv[0]);
                return 84;
            }
            if (!gui.connectToServer()) {
                std::cerr << "Erreur: Impossible de se connecter au serveur" << std::endl;
                return 84;
            }
            gui.run();
            continue;
        }
        if (gui.getCurrentAction() == ACTION_CHANGE_THEME) {
            gui.getTheme()->show();
            continue;
        }
    }
    return 0;
}

void ZappyGUI::update_info_page()
{
    int item1 = 0;
    int item2 = 0;
    int item3 = 0;
    int item4 = 0;
    int item5 = 0;
    int item6 = 0;
    int item7 = 0;

    int level1 = 0;
    int level2 = 0;
    int level3 = 0;
    int level4 = 0;
    int level5 = 0;
    int level6 = 0;
    int level7 = 0;
    int level8 = 0;

    std::map<std::pair<int, int>, Tile> all_tiles = this->m_world.getAllTiles();

    for (const auto& tile_pair : all_tiles) {
        const Tile& tile = tile_pair.second;
        item1 += tile.food;
        item2 += tile.linemate;
        item3 += tile.deraumere;
        item4 += tile.sibur;
        item5 += tile.mendiane;
        item6 += tile.phiras;
        item7 += tile.thystame;
    }

    for (const auto& player : this->m_players) {
        player.level == 1 ? level1++ :
        player.level == 2 ? level2++ :
        player.level == 3 ? level3++ :
        player.level == 4 ? level4++ :
        player.level == 5 ? level5++ :
        player.level == 6 ? level6++ :
        player.level == 7 ? level7++ : level8++;

        item1 += player.inventory.food;
        item2 += player.inventory.linemate;
        item3 += player.inventory.deraumere;
        item4 += player.inventory.sibur;
        item5 += player.inventory.mendiane;
        item6 += player.inventory.phiras;
        item7 += player.inventory.thystame;
    }

    m_inGameInfoMenu->setquantity(m_inGameInfoMenu->m_level1, level1);
    m_inGameInfoMenu->setquantity(m_inGameInfoMenu->m_level2, level2);
    m_inGameInfoMenu->setquantity(m_inGameInfoMenu->m_level3, level3);
    m_inGameInfoMenu->setquantity(m_inGameInfoMenu->m_level4, level4);
    m_inGameInfoMenu->setquantity(m_inGameInfoMenu->m_level5, level5);
    m_inGameInfoMenu->setquantity(m_inGameInfoMenu->m_level6, level6);
    m_inGameInfoMenu->setquantity(m_inGameInfoMenu->m_level7, level7);
    m_inGameInfoMenu->setquantity(m_inGameInfoMenu->m_level8, level8);

    m_inGameInfoMenu->setquantity(m_inGameInfoMenu->m_food, item1);
    m_inGameInfoMenu->setquantity(m_inGameInfoMenu->m_linemate, item2);
    m_inGameInfoMenu->setquantity(m_inGameInfoMenu->m_deraumere, item3);
    m_inGameInfoMenu->setquantity(m_inGameInfoMenu->m_sibur, item4);
    m_inGameInfoMenu->setquantity(m_inGameInfoMenu->m_mendiane, item5);
    m_inGameInfoMenu->setquantity(m_inGameInfoMenu->m_phiras, item6);
    m_inGameInfoMenu->setquantity(m_inGameInfoMenu->m_thystame, item7);
    m_inGameInfoMenu->updateTeamsAndMapInfo(m_teams, m_players, m_world.getWidth(), m_world.getHeight());
}

/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** setting.cpp
*/

#include "Settings/Setting.hpp"
#include <thread>
#include <chrono>
#include <algorithm>
#include "../gui.hpp"
#include <cstdlib>

Setting::Setting(sf::RenderWindow* renderWindow, sf::Font& fontRef, int* sharedVolume)
    : window(renderWindow), font(&fontRef), m_sharedVolume(sharedVolume)
{

	// Load background texture and initialize basic settings
	if (backgroundTexture.loadFromFile("Assets/MAIN_VILLAGE/ZAPPY.png")) {
		backgroundSprite.setTexture(backgroundTexture);
		volumeLabel.setFont(*font);
		volumeLabel.setString("VOLUME");
		volumeLabel.setCharacterSize(36);
		volumeLabel.setFillColor(sf::Color::White);
	}
	// Get system volume based on OS
#if defined(__APPLE__)
	{
		const char *cmd = "osascript -e 'output volume of (get volume settings)'";
		FILE* pipe = popen(cmd, "r");
		if (pipe) {
			char buf[8] = {0};
			if (fgets(buf, sizeof(buf), pipe))
				*m_sharedVolume = std::atoi(buf);
			pclose(pipe);
		}
	}
#elif defined(__linux__)
	{
		const char *cmd = "amixer get Master | grep -o '[0-9]\\{1,3\\}%'";
		FILE* pipe = popen(cmd, "r");
		if (pipe) {
			char buf[8] = {0};
			if (fgets(buf, sizeof(buf), pipe))
				*m_sharedVolume = std::atoi(buf);
			pclose(pipe);
		}
	}
#endif
	*m_sharedVolume = std::max(0, std::min(*m_sharedVolume, 100));

	// Initialize volume display text
	volumeValueText.setFont(*font);
	volumeValueText.setCharacterSize(32);
	volumeValueText.setFillColor(sf::Color::White);
	// Initialisation des flèches
	m_leftArrow.setFont(*font);
	m_leftArrow.setString(">");
	m_leftArrow.setCharacterSize(32);
	m_leftArrow.setFillColor(SelectedBtnColor);

	m_rightArrow.setFont(*font);
	m_rightArrow.setString("<");
	m_rightArrow.setCharacterSize(32);
	m_rightArrow.setFillColor(SelectedBtnColor);

	// Initialize window label
	windowLabel.setFont(*font);
	windowLabel.setString("WINDOW");
	windowLabel.setCharacterSize(36);
	windowLabel.setFillColor(sf::Color::White);
	
	// Get initial window size
	auto sz = window->getSize();
	m_winW = sz.x;
	m_winH = sz.y;

	updateLayout();
}


void Setting::show()
{
	while (window->isOpen()) {
		// Update window dimensions
		auto sz = window->getSize();
		m_winW = sz.x;
		m_winH = sz.y;
		window->setView(sf::View(sf::FloatRect(0, 0, (float)m_winW, (float)m_winH)));

		// Update UI state
		updateLayout();
		updateHoverStates();

		// Handle user input
		if (handleInput())
			break;

		// Render frame
		draw();
		window->display();
	}
}

void Setting::updateHoverStates()
{
	sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*window));

	// Update hover states for all buttons
	volUpButton.hover = volUpButton.shape.getGlobalBounds().contains(mousePos);
	volDownButton.hover = volDownButton.shape.getGlobalBounds().contains(mousePos);
	res1080Button.hover = res1080Button.shape.getGlobalBounds().contains(mousePos);
	res720Button.hover = res720Button.shape.getGlobalBounds().contains(mousePos);
	res900Button.hover = res900Button.shape.getGlobalBounds().contains(mousePos);
	backButton.hover = backButton.shape.getGlobalBounds().contains(mousePos);
}

bool Setting::handleInput()
{
	sf::Event event;
	while (window->pollEvent(event)) {
		// Handle window close
		if (event.type == sf::Event::Closed) {
			window->close();
			return true;
		}

		// Handle escape key
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
			return true;
		}
		// Navigation clavier
		if (event.type == sf::Event::KeyPressed) {
		    m_keyboardMode = true;
		    if (event.key.code == sf::Keyboard::Up) {
		        // Section précédente
		        if (m_currentSection == 0) {
		            m_currentSection = m_buttonsPerSection.size() - 1;
		        } else {
		            m_currentSection--;
		        }
		        m_currentButtonInSection = 0; // Reset à 0 dans la nouvelle section
		    }
		    else if (event.key.code == sf::Keyboard::Down) {
		        // Section suivante
		        m_currentSection = (m_currentSection + 1) % m_buttonsPerSection.size();
		        m_currentButtonInSection = 0; // Reset à 0 dans la nouvelle section
		    }
		    else if (event.key.code == sf::Keyboard::Left) {
		        if (m_currentSection == 0) {
		            // Section Volume : diminuer
		            *m_sharedVolume = std::max(*m_sharedVolume - 10, 0);
		            setSystemVolume(*m_sharedVolume);
		            volumeValueText.setString(std::to_string(*m_sharedVolume));
		        } else {
		            // Navigation normale
		            if (m_currentButtonInSection == 0) {
		                m_currentButtonInSection = m_buttonsPerSection[m_currentSection] - 1;
		            } else {
		                m_currentButtonInSection--;
		            }
		        }
		    }
		    else if (event.key.code == sf::Keyboard::Right) {
		        if (m_currentSection == 0) {
		            // Section Volume : augmenter
		            *m_sharedVolume = std::min(*m_sharedVolume + 10, 100);
		            setSystemVolume(*m_sharedVolume);
		            volumeValueText.setString(std::to_string(*m_sharedVolume));
		        } else {
		            // Navigation normale
		            m_currentButtonInSection = (m_currentButtonInSection + 1) % m_buttonsPerSection[m_currentSection];
		        }
		    }
		    else if (event.key.code == sf::Keyboard::Return) {
		        // Sélection avec Entrée
		        return handleKeyboardSelection();
		    }
		}
		// Désactiver le mode clavier si souris utilisée
		if (event.type == sf::Event::MouseMoved) {
		    m_keyboardMode = false;
		}

		// Handle mouse clicks
		if (event.type == sf::Event::MouseButtonPressed &&
			event.mouseButton.button == sf::Mouse::Left) {

			int mouseX = event.mouseButton.x;
			int mouseY = event.mouseButton.y;

			// Volume Up button
			if (volUpButton.shape.getGlobalBounds().contains(mouseX, mouseY)) {
				*m_sharedVolume = std::min(*m_sharedVolume + 10, 100);
				setSystemVolume(*m_sharedVolume);
				volumeValueText.setString(std::to_string(*m_sharedVolume));
				volUpButton.shape.setFillColor(ClickBtnColor);
				draw(); 
				window->display();
				std::this_thread::sleep_for(std::chrono::milliseconds(80));
			}
			// Volume Down button
			else if (volDownButton.shape.getGlobalBounds().contains(mouseX, mouseY)) {
				*m_sharedVolume = std::max(*m_sharedVolume - 10, 0);
				setSystemVolume(*m_sharedVolume);
				volumeValueText.setString(std::to_string(*m_sharedVolume));
				volDownButton.shape.setFillColor(ClickBtnColor);
				draw(); 
				window->display();
				std::this_thread::sleep_for(std::chrono::milliseconds(80));
			}
			// Resolution 1920x1080 button
			else if (res1080Button.shape.getGlobalBounds().contains(mouseX, mouseY)) {
				window->create(sf::VideoMode(1920, 1080), "Zappy GUI", sf::Style::Titlebar | sf::Style::Close);
				m_winW = 1920; 
				m_winH = 1080;
				res1080Button.shape.setFillColor(ClickBtnColor);
				draw(); 
				window->display();
				std::this_thread::sleep_for(std::chrono::milliseconds(80));
			}
			// Resolution 1280x720 button
			else if (res720Button.shape.getGlobalBounds().contains(mouseX, mouseY)) {
				window->create(sf::VideoMode(1280, 720), "Zappy GUI", sf::Style::Titlebar | sf::Style::Close);
				m_winW = 1280; 
				m_winH = 720;
				res720Button.shape.setFillColor(ClickBtnColor);
				draw(); 
				window->display();
				std::this_thread::sleep_for(std::chrono::milliseconds(80));
			}
			// Resolution 1440x900 button
			else if (res900Button.shape.getGlobalBounds().contains(mouseX, mouseY)) {
				window->create(sf::VideoMode(1440, 900), "Zappy GUI", sf::Style::Titlebar | sf::Style::Close);
				m_winW = 1440; 
				m_winH = 900;
				res900Button.shape.setFillColor(ClickBtnColor);
				draw(); 
				window->display();
				std::this_thread::sleep_for(std::chrono::milliseconds(80));
			}
			// Back button
			else if (backButton.shape.getGlobalBounds().contains(mouseX, mouseY)) {
				backButton.shape.setFillColor(ClickBtnColor);
				draw(); 
				window->display();
				std::this_thread::sleep_for(std::chrono::milliseconds(80));
				return true;
			}
		}
	}
	return false;
}

void Setting::drawSelectionArrows()
{
    if (m_currentSection == 0) {
        // Section Volume - afficher les flèches autour de la valeur du volume
        sf::FloatRect volumeBounds = volumeValueText.getGlobalBounds();
        
        // Flèche gauche
        m_leftArrow.setPosition(
            volumeBounds.left - 50.f,
            volumeBounds.top + volumeBounds.height / 2.f - m_leftArrow.getLocalBounds().height / 2.f
        );
        window->draw(m_leftArrow);
        
        // Flèche droite
        m_rightArrow.setPosition(
            volumeBounds.left + volumeBounds.width + 30.f,
            volumeBounds.top + volumeBounds.height / 2.f - m_rightArrow.getLocalBounds().height / 2.f
        );
        window->draw(m_rightArrow);
    }
    else if (m_currentSection == 1) {
        // Section Résolutions - afficher les flèches autour du bouton sélectionné
        Button* selectedButton = nullptr;
        
        switch (m_currentButtonInSection) {
            case 0:
                selectedButton = &res1080Button;
                break;
            case 1:
                selectedButton = &res720Button;
                break;
            case 2:
                selectedButton = &res900Button;
                break;
        }
        
        if (selectedButton) {
            sf::FloatRect buttonBounds = selectedButton->shape.getGlobalBounds();
            
            // Flèche gauche
            m_leftArrow.setPosition(
                buttonBounds.left - 40.f,
                buttonBounds.top + buttonBounds.height / 2.f - m_leftArrow.getLocalBounds().height / 2.f
            );
            window->draw(m_leftArrow);
            
            // Flèche droite
            m_rightArrow.setPosition(
                buttonBounds.left + buttonBounds.width + 20.f,
                buttonBounds.top + buttonBounds.height / 2.f - m_rightArrow.getLocalBounds().height / 2.f
            );
            window->draw(m_rightArrow);
        }
    }
    else if (m_currentSection == 2) {
        // Section Back - afficher les flèches autour du bouton Back
        sf::FloatRect buttonBounds = backButton.shape.getGlobalBounds();
        
        // Flèche gauche
        m_leftArrow.setPosition(
            buttonBounds.left - 40.f,
            buttonBounds.top + buttonBounds.height / 2.f - m_leftArrow.getLocalBounds().height / 2.f
        );
        window->draw(m_leftArrow);
        
        // Flèche droite
        m_rightArrow.setPosition(
            buttonBounds.left + buttonBounds.width + 20.f,
            buttonBounds.top + buttonBounds.height / 2.f - m_rightArrow.getLocalBounds().height / 2.f
        );
        window->draw(m_rightArrow);
    }
}

void Setting::draw()
{
	window->clear(sf::Color::Black);
	
	// Draw background
	if (backgroundTexture.getSize().x > 0) {
		float scaleX = float(m_winW) / backgroundTexture.getSize().x;
		float scaleY = float(m_winH) / backgroundTexture.getSize().y;
		backgroundSprite.setScale(scaleX, scaleY);
		backgroundSprite.setPosition(0.f, 0.f);
		window->draw(backgroundSprite);
	}
	
	// Draw volume section
	window->draw(volumeLabel);
	window->draw(volumeValueText);
	
	// Draw volume buttons with shadows
	{
		sf::RectangleShape shadow = volUpButton.shape;
		shadow.move(shadowOffset);
		shadow.setFillColor(ShadowColor);
		window->draw(shadow);
	}
	window->draw(volUpButton.shape);
	window->draw(volUpButton.text);

	{
		sf::RectangleShape shadow = volDownButton.shape;
		shadow.move(shadowOffset);
		shadow.setFillColor(ShadowColor);
		window->draw(shadow);
	}
	window->draw(volDownButton.shape);
	window->draw(volDownButton.text);
	
	// Draw window section
	window->draw(windowLabel);

	// Draw resolution buttons with shadows
	{
		sf::RectangleShape shadow = res1080Button.shape;
		shadow.move(shadowOffset);
		shadow.setFillColor(ShadowColor);
		window->draw(shadow);
	}
	window->draw(res1080Button.shape);
	window->draw(res1080Button.text);

	{
		sf::RectangleShape shadow = res720Button.shape;
		shadow.move(shadowOffset);
		shadow.setFillColor(ShadowColor);
		window->draw(shadow);
	}
	window->draw(res720Button.shape);
	window->draw(res720Button.text);

	{
		sf::RectangleShape shadow = res900Button.shape;
		shadow.move(shadowOffset);
		shadow.setFillColor(ShadowColor);
		window->draw(shadow);
	}
	window->draw(res900Button.shape);
	window->draw(res900Button.text);
	
	// Draw back button with shadow
	{
		sf::RectangleShape shadow = backButton.shape;
		shadow.move(shadowOffset);
		shadow.setFillColor(ShadowColor);
		window->draw(shadow);
	}
	window->draw(backButton.shape);
	if (m_keyboardMode) {
		drawSelectionArrows();
	}
	window->draw(backButton.text);
}

void Setting::updateLayout()
{
	// Volume label (titre)
	{
		sf::FloatRect bounds = volumeLabel.getLocalBounds();
		float x = (m_winW - bounds.width) / 2.f;
		float y = m_winH * 0.1f;
		volumeLabel.setPosition(x, y);
	}

	// Volume value
	{
		volumeValueText.setString(std::to_string(*m_sharedVolume));
		sf::FloatRect bounds = volumeValueText.getLocalBounds();
		float x = (m_winW - bounds.width) / 2.f;
		float y = m_winH * 0.17f;
		volumeValueText.setPosition(x, y);
	}

	// Volume + button
	{
		float width = m_winW * 0.15f;
		float height = m_winH * 0.07f;
		float centerX = m_winW * 0.35f;
		float centerY = m_winH * 0.27f;
		configureButton(volUpButton, "Vol +", centerX, centerY, width, height);
	}

	// Volume - button
	{
		float width = m_winW * 0.15f;
		float height = m_winH * 0.07f;
		float centerX = m_winW * 0.65f;
		float centerY = m_winH * 0.27f;
		configureButton(volDownButton, "Vol -", centerX, centerY, width, height);
	}

	// Window label
	{
		sf::FloatRect bounds = windowLabel.getLocalBounds();
		float x = (m_winW - bounds.width) / 2.f;
		float y = m_winH * 0.43f;
		windowLabel.setPosition(x, y);
	}

	// 1920x1080
	{
		float width = m_winW * 0.15f;
		float height = m_winH * 0.07f;
		float centerX = m_winW * 0.25f;
		float centerY = m_winH * 0.55f;
		configureButton(res1080Button, "1920x1080", centerX, centerY, width, height);
	}

	// 1280x720
	{
		float width = m_winW * 0.15f;
		float height = m_winH * 0.07f;
		float centerX = m_winW * 0.50f;
		float centerY = m_winH * 0.55f;
		configureButton(res720Button, "1280x720", centerX, centerY, width, height);
	}

	// 1440x900
	{
		float width = m_winW * 0.15f;
		float height = m_winH * 0.07f;
		float centerX = m_winW * 0.75f;
		float centerY = m_winH * 0.55f;
		configureButton(res900Button, "1440x900", centerX, centerY, width, height);
	}

	// Back button (en bas de l'écran, toujours visible)
	{
		float width = m_winW * 0.15f;
		float height = m_winH * 0.07f;
		float centerX = m_winW / 2.f;
		float centerY = m_winH * 0.85f;
		configureButton(backButton, "Back", centerX, centerY, width, height);
	}
}

void Setting::configureButton(Button& btn, const std::string& label, float cx, float cy, float width, float height)
{
    btn.shape.setSize({ width, height });

    sf::Color buttonColor = NormalBtnColor;
    if (btn.hover && !m_keyboardMode) {
        buttonColor = HoverBtnColor;
    }

    btn.shape.setFillColor(buttonColor);
    btn.shape.setOutlineThickness(2.f);
    btn.shape.setOutlineColor(sf::Color(50, 50, 50));
    btn.shape.setPosition(cx - width / 2.f, cy - height / 2.f);

    // Configure button text
    btn.text.setFont(*font);
    btn.text.setString(label);
    btn.text.setCharacterSize(static_cast<unsigned int>(height * 0.5f));
    btn.text.setFillColor(TextColor);
    
    // Center text on button
    sf::FloatRect textBounds = btn.text.getLocalBounds();
    btn.text.setPosition(
        cx - textBounds.width / 2.f,
        cy - textBounds.height / 2.f - textBounds.top / 2.f
    );
}

bool Setting::setSystemVolume(int newVol)
{
    #if defined(__APPLE__)
        std::string cmd = "osascript -e 'set volume output volume " + std::to_string(newVol) + "'";
        return (system(cmd.c_str()) == 0);
    #elif defined(__linux__)
        std::string cmd = "amixer set Master " + std::to_string(newVol) + "% > /dev/null 2>&1";
        return (system(cmd.c_str()) == 0);
    #else
        return false;
    #endif
}

bool Setting::handleKeyboardSelection()
{
    if (m_currentSection == 0) {
        // Section Volume - pas de sélection spéciale, juste un feedback visuel
        return false;
    }
    else if (m_currentSection == 1) {
        // Section Résolutions
        switch (m_currentButtonInSection) {
            case 0: // 1920x1080
                window->create(sf::VideoMode(1920, 1080), "Zappy GUI", sf::Style::Titlebar | sf::Style::Close);
                m_winW = 1920;
                m_winH = 1080;
                break;
            case 1: // 1280x720
                window->create(sf::VideoMode(1280, 720), "Zappy GUI", sf::Style::Titlebar | sf::Style::Close);
                m_winW = 1280;
                m_winH = 720;
                break;
            case 2: // 1440x900
                window->create(sf::VideoMode(1440, 900), "Zappy GUI", sf::Style::Titlebar | sf::Style::Close);
                m_winW = 1440;
                m_winH = 900;
                break;
        }
        return false;
    }
    else if (m_currentSection == 2) {
        return true;
    }
    return false;
}


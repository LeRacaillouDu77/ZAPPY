/*
*** Zappy
** File description:
** Theme.cpp
*/

#include "Themes/Theme.hpp"
#include <thread>
#include <chrono>
#include <algorithm>
#include <cstdlib>

Theme::Theme(sf::RenderWindow* renderWindow, sf::Font& fontRef) : window(renderWindow), font(&fontRef)
{
	// Load background texture and initialize basic Themes
	if (backgroundTexture.loadFromFile("Assets/MAIN_VILLAGE/ZAPPY.png")) {
		backgroundSprite.setTexture(backgroundTexture);
		volumeLabel.setFont(*font);
		volumeLabel.setString("VOLUME");
		volumeLabel.setCharacterSize(36);
		volumeLabel.setFillColor(sf::Color::White);
		volume = 50;
	}
	
	// Initialize selection arrows
	m_leftArrow.setFont(*font);
	m_leftArrow.setString("<");
	m_leftArrow.setCharacterSize(32);
	m_leftArrow.setFillColor(sf::Color::Yellow);
	
	m_rightArrow.setFont(*font);
	m_rightArrow.setString(">");
	m_rightArrow.setCharacterSize(32);
	m_rightArrow.setFillColor(sf::Color::Yellow);
	
	// Get system volume based on OS
#if defined(__APPLE__)
	{
		const char *cmd = "osascript -e 'output volume of (get volume Themes)'";
		FILE* pipe = popen(cmd, "r");
		if (pipe) {
			char buf[8] = {0};
			if (fgets(buf, sizeof(buf), pipe))
				volume = std::atoi(buf);
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
				volume = std::atoi(buf);
			pclose(pipe);
		}
	}
#endif
	volume = std::max(0, std::min(volume, 100));

	// Initialize volume display text
	volumeValueText.setFont(*font);
	volumeValueText.setCharacterSize(32);
	volumeValueText.setFillColor(sf::Color::White);
	
	// Initialize window label
	windowLabel.setFont(*font);
	windowLabel.setString("THEMES");
	windowLabel.setCharacterSize(36);
	windowLabel.setFillColor(sf::Color::White);
	
	// Get initial window size
	auto sz = window->getSize();
	m_winW = sz.x;
	m_winH = sz.y;

	updateLayout();
}

void Theme::show()
{
	while (window->isOpen()) {
		// Update window dimensions
		auto sz = window->getSize();
		m_winW = sz.x;
		m_winH = sz.y;
		window->setView(sf::View(sf::FloatRect(0, 0, (float)m_winW, (float)m_winH)));

		// Update UI state
		updateLayout();
		if (!m_keyboardMode) {
			updateHoverStates();
		}
		
		// Handle user input
		if (handleInput())
			break;

		// Render frame
		draw();
		window->display();
	}
}

void Theme::updateHoverStates()
{
	sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*window));
	
	// Update hover states for all buttons
	warButton.hover = warButton.shape.getGlobalBounds().contains(mousePos);
	nightButton.hover = nightButton.shape.getGlobalBounds().contains(mousePos);
	capitalButton.hover = capitalButton.shape.getGlobalBounds().contains(mousePos);
	backButton.hover = backButton.shape.getGlobalBounds().contains(mousePos);
}

bool Theme::handleKeyboardSelection()
{
	// Reset all hover states first
	warButton.hover = false;
	nightButton.hover = false;
	capitalButton.hover = false;
	backButton.hover = false;
	
	// Set hover state for currently selected button
	if (m_currentSection == 0) { // Section des thèmes
		switch (m_currentButtonInSection) {
			case 0: warButton.hover = true; break;
			case 1: nightButton.hover = true; break;
			case 2: capitalButton.hover = true; break;
		}
	} else if (m_currentSection == 1) { // Section back button
		backButton.hover = true;
	}
	
	return false;
}

void Theme::drawSelectionArrows()
{
	if (!m_keyboardMode) return;
	
	Button* selectedButton = nullptr;
	
	// Determine which button is selected
	if (m_currentSection == 0) {
		switch (m_currentButtonInSection) {
			case 0: selectedButton = &warButton; break;
			case 1: selectedButton = &nightButton; break;
			case 2: selectedButton = &capitalButton; break;
		}
	} else if (m_currentSection == 1) {
		selectedButton = &backButton;
	}
	
	if (selectedButton) {
		sf::FloatRect buttonBounds = selectedButton->shape.getGlobalBounds();
		
		// Position left arrow
		m_leftArrow.setPosition(
			buttonBounds.left - 40.f,
			buttonBounds.top + buttonBounds.height / 2.f - 16.f
		);
		
		// Position right arrow
		m_rightArrow.setPosition(
			buttonBounds.left + buttonBounds.width + 10.f,
			buttonBounds.top + buttonBounds.height / 2.f - 16.f
		);
		
		window->draw(m_leftArrow);
		window->draw(m_rightArrow);
	}
}

bool Theme::handleInput()
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
		
		// Handle keyboard navigation
		if (event.type == sf::Event::KeyPressed) {
			m_keyboardMode = true;
			
			switch (event.key.code) {
				case sf::Keyboard::Left:
					if (m_currentButtonInSection > 0) {
						m_currentButtonInSection--;
					} else {
						m_currentButtonInSection = m_buttonsPerSection[m_currentSection] - 1;
					}
					handleKeyboardSelection();
					break;
					
				case sf::Keyboard::Right:
					m_currentButtonInSection = (m_currentButtonInSection + 1) % m_buttonsPerSection[m_currentSection];
					handleKeyboardSelection();
					break;
					
				case sf::Keyboard::Up:
					if (m_currentSection > 0) {
						m_currentSection--;
						m_currentButtonInSection = std::min(m_currentButtonInSection, m_buttonsPerSection[m_currentSection] - 1);
					}
					handleKeyboardSelection();
					break;
					
				case sf::Keyboard::Down:
					if (m_currentSection < m_buttonsPerSection.size() - 1) {
						m_currentSection++;
						m_currentButtonInSection = std::min(m_currentButtonInSection, m_buttonsPerSection[m_currentSection] - 1);
					}
					handleKeyboardSelection();
					break;
					
				case sf::Keyboard::Enter:
				case sf::Keyboard::Space:
					// Activate selected button
					if (m_currentSection == 0) {
						switch (m_currentButtonInSection) {
							case 0: // War theme
								NormalBtnColor = sf::Color(207, 99, 17); // Orange
								TextColor = sf::Color::White;
								break;
							case 1: // Night village
								NormalBtnColor = sf::Color(63, 20, 163); // Violet Bleu
								TextColor = sf::Color::White;
								break;
							case 2: // Capital
								NormalBtnColor = sf::Color(41, 105, 12); // Vert
								TextColor = sf::Color::White;
								break;
							default:
								break;
						}
					} else if (m_currentSection == 1) {
						return true;
					}
					break;
					
				default:
					break;
			}
		}
		
		// Handle mouse movement (switch to mouse mode)
		if (event.type == sf::Event::MouseMoved) {
			m_keyboardMode = false;
		}
		
		// Handle mouse clicks
		if (event.type == sf::Event::MouseButtonPressed &&
			event.mouseButton.button == sf::Mouse::Left) {
			
			m_keyboardMode = false;
			int mouseX = event.mouseButton.x;
			int mouseY = event.mouseButton.y;
			
			// War theme button
			if (warButton.shape.getGlobalBounds().contains(mouseX, mouseY)) {
				NormalBtnColor = sf::Color(207, 99, 17); // Orange
				TextColor = sf::Color::White;
			}
			// Night village button
			else if (nightButton.shape.getGlobalBounds().contains(mouseX, mouseY)) {
				NormalBtnColor = sf::Color(63, 20, 163); // Violet Bleu
				TextColor = sf::Color::White;
			}
			// Capital button
			else if (capitalButton.shape.getGlobalBounds().contains(mouseX, mouseY)) {
				NormalBtnColor = sf::Color(41, 105, 12); // Vert
				TextColor = sf::Color::White;
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
	
	// Update keyboard selection if in keyboard mode
	if (m_keyboardMode) {
		handleKeyboardSelection();
	}
	
	return false;
}

void Theme::draw()
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
	
	// Draw window section
	window->draw(windowLabel);

	// Draw resolution buttons with shadows
	{
		sf::RectangleShape shadow = warButton.shape;
		shadow.move(shadowOffset);
		shadow.setFillColor(ShadowColor);
		window->draw(shadow);
	}
	window->draw(warButton.shape);
	window->draw(warButton.text);

	{
		sf::RectangleShape shadow = nightButton.shape;
		shadow.move(shadowOffset);
		shadow.setFillColor(ShadowColor);
		window->draw(shadow);
	}
	window->draw(nightButton.shape);
	window->draw(nightButton.text);

	{
		sf::RectangleShape shadow = capitalButton.shape;
		shadow.move(shadowOffset);
		shadow.setFillColor(ShadowColor);
		window->draw(shadow);
	}
	window->draw(capitalButton.shape);
	window->draw(capitalButton.text);
	
	// Draw back button with shadow
	{
		sf::RectangleShape shadow = backButton.shape;
		shadow.move(shadowOffset);
		shadow.setFillColor(ShadowColor);
		window->draw(shadow);
	}
	window->draw(backButton.shape);
	window->draw(backButton.text);
	
	// Draw selection arrows if in keyboard mode
	drawSelectionArrows();
}

void Theme::updateLayout()
{
	// Window label
	{
		sf::FloatRect bounds = windowLabel.getLocalBounds();
		float x = (m_winW - bounds.width) / 2.f;
		float y = m_winH * 0.50f;
		windowLabel.setPosition(x, y);
	}

	// War
	{
		float width = m_winW * 0.18f;
		float height = m_winH * 0.07f;
		float centerX = m_winW * 0.25f;
		float centerY = m_winH * 0.65f;
		configureButton(warButton, "CLANS WAR", centerX, centerY, width, height);
	}

	// night
	{
		float width = m_winW * 0.20f;
		float height = m_winH * 0.07f;
		float centerX = m_winW * 0.50f;
		float centerY = m_winH * 0.65f;
		configureButton(nightButton, "SNOW VILLAGE", centerX, centerY, width, height);
	}

	// capital
	{
		float width = m_winW * 0.18f;
		float height = m_winH * 0.07f;
		float centerX = m_winW * 0.75f;
		float centerY = m_winH * 0.65f;
		configureButton(capitalButton, "CAPITAL", centerX, centerY, width, height);
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

void Theme::configureButton(Button& btn, const std::string& label, float cx, float cy, float width, float height)
{
	// Configure button shape
	btn.shape.setSize({ width, height });
	btn.shape.setFillColor(btn.hover ? HoverBtnColor : NormalBtnColor);
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

bool Theme::setSystemVolume(int newVol)
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

Themes_enum Theme::getCurrentTheme()
{
	if (NormalBtnColor == sf::Color(207, 99, 17)) {
		return Themes_enum::THEME_WAR;
	} else if (NormalBtnColor == sf::Color(63, 20, 163)) {
		return Themes_enum::THEME_NIGHT;
	} else if (NormalBtnColor == sf::Color(41, 105, 12)) {
		return Themes_enum::THEME_CAPITAL;
	}
	return Themes_enum::THEME_BASE;
}
/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** camera.cpp
*/

#include "gui.hpp"

Camera::Camera() : position(0, 0), zoomLevel(1.0f), size(800, 600) {
    camera_view.setSize(sf::Vector2f(800, 600));
    camera_view.setCenter(position);
}

Camera::Camera(sf::Vector2f base_position, float base_zoom, sf::Vector2f size) : position(base_position), zoomLevel(base_zoom), size(size) {
    camera_view.setSize(size);
    camera_view.setCenter(position);
}

void Camera::setPosition(float x, float y) {
    position = sf::Vector2f(x, y);
    camera_view.setCenter(position);
}

void Camera::setZoom(float zoom) {
    if (zoom < 1) {
        zoom = 1;
    }
    if (zoom > 10) {
        zoom = 10;
    }
    zoomLevel = zoom;
    camera_view.setSize(sf::Vector2f(1200 / zoomLevel, 800 / zoomLevel));
}

void Camera::move(sf::Vector2f offset) {
    position = camera_view.getCenter(); // Adjust position based on the current view size
    position += offset;
    if (position.x < size.x / zoomLevel / 2) {
        position.x = size.x / zoomLevel / 2;
    }
    if (position.y < size.y / zoomLevel / 2) {
        position.y = size.y / zoomLevel / 2;
    }
    if (position.x > size.x - size.x / zoomLevel / 2) {
        position.x = size.x - size.x / zoomLevel /  2;
    }
    if (position.y > size.y - size.y / zoomLevel / 2) {
        position.y = size.y - size.y / zoomLevel / 2;
    }
    camera_view.setCenter(position);
}

float Camera::getZoom() const {
    return zoomLevel;
}

sf::Vector2f Camera::getPosition() const {
    return position;
}

sf::Vector2f Camera::getSize() const {
    return size;
}

const sf::View& Camera::getView() const {
    return camera_view;
}

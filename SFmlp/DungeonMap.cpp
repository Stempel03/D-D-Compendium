#include "DungeonMap.h"
#include <fstream>

DungeonMap::DungeonMap(int w, int h, float tSize)
    : width_(w), height_(h), tileSize_(tSize), isDragging(false) {
    grid.clear();
    grid.resize(width_);
    for (int x = 0; x < width_; ++x) {
        grid[x].resize(height_);
    }
    clear(TileType::Empty);
    cameraView.setSize({ 1200.f, 800.f });
    cameraView.setCenter({ (width_ * tileSize_) / 2.f, (height_ * tileSize_) / 2.f });
}

bool DungeonMap::load(const std::string& path) {
    std::ifstream file(path);
    if (!file) return false;
    int w, h;
    if (!(file >> w >> h)) return false;
    width_ = w; height_ = h;
    grid.clear();
    grid.resize(width_);
    for (int x = 0; x < width_; ++x) {
        grid[x].resize(height_);
    }
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            int t; file >> t;
            set(x, y, static_cast<TileType>(t));
        }
    }
    return true;
}

void DungeonMap::set(int x, int y, TileType type) {
    if (x >= 0 && x < width_ && y >= 0 && y < height_) {
        grid[x][y].type = type;
        switch (type) {
        case TileType::Floor:    grid[x][y].shape.setFillColor(sf::Color(120, 120, 120)); break;
        case TileType::Wall:     grid[x][y].shape.setFillColor(sf::Color(60, 60, 60)); break;
        case TileType::Door:     grid[x][y].shape.setFillColor(sf::Color(150, 75, 0)); break;
        case TileType::Trap:     grid[x][y].shape.setFillColor(sf::Color(200, 50, 50)); break;
        case TileType::Treasure: grid[x][y].shape.setFillColor(sf::Color(255, 215, 0)); break;
        default:                 grid[x][y].shape.setFillColor(sf::Color(30, 30, 30)); break;
        }
    }
}

TileType DungeonMap::get(int x, int y) const {
    if (x >= 0 && x < width_ && y >= 0 && y < height_) return grid[x][y].type;
    return TileType::Empty;
}

void DungeonMap::clear(TileType type) {
    for (int x = 0; x < width_; ++x) {
        for (int y = 0; y < height_; ++y) {
            grid[x][y].shape.setSize(sf::Vector2f(tileSize_ - 1.f, tileSize_ - 1.f));
            grid[x][y].shape.setPosition(sf::Vector2f(x * tileSize_, y * tileSize_));
            set(x, y, type);
        }
    }
}

bool DungeonMap::save(const std::string& path) const {
    std::ofstream file(path);
    if (!file) return false;
    file << width_ << " " << height_ << "\n";
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            file << static_cast<int>(grid[x][y].type) << " ";
        }
        file << "\n";
    }
    return true;
}

void DungeonMap::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (const auto* scroll = event.getIf<sf::Event::MouseWheelScrolled>()) {
        cameraView.zoom(scroll->delta > 0 ? 0.9f : 1.1f);
    }
    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (click->button == sf::Mouse::Button::Right) {
            isDragging = true;
            lastMousePos = sf::Mouse::getPosition(window);
        }
    }
    if (const auto* release = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (release->button == sf::Mouse::Button::Right) isDragging = false;
    }
}
void DungeonMap::resetView(sf::RenderWindow& window) {
    cameraView.setSize({ static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y) });
    cameraView.setCenter({ (width_ * tileSize_) / 2.f, (height_ * tileSize_) / 2.f });
    window.setView(cameraView);
}
void DungeonMap::update(sf::RenderWindow& window) {
    if (isDragging) {
        sf::Vector2i currentMousePos = sf::Mouse::getPosition(window);
        sf::Vector2f delta = window.mapPixelToCoords(lastMousePos) - window.mapPixelToCoords(currentMousePos);
        cameraView.move(delta);
        lastMousePos = currentMousePos;
    }
    window.setView(cameraView);
}

void DungeonMap::draw(sf::RenderWindow& window) {
    for (int x = 0; x < width_; ++x) {
        for (int y = 0; y < height_; ++y) {
            window.draw(grid[x][y].shape);
        }
    }
}
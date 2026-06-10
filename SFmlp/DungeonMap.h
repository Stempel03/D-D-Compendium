#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum class TileType { Empty, Floor, Wall, Door, Trap, Treasure };

struct Tile {
    sf::RectangleShape shape;
    TileType type = TileType::Empty;
};

class DungeonMap {
public:
    DungeonMap(int w = 20, int h = 12, float tSize = 32.f);
    int width() const { return width_; }
    int height() const { return height_; }
    float tileSize() const { return tileSize_; }

    void set(int x, int y, TileType type);
    TileType get(int x, int y) const;
    void clear(TileType type = TileType::Empty);
    bool save(const std::string& path) const;
    bool load(const std::string& path);

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    void resetView(sf::RenderWindow& window);
private:
    int width_, height_;
    float tileSize_;
    std::vector<std::vector<Tile>> grid;
    sf::View cameraView;
    bool isDragging;
    sf::Vector2i lastMousePos;
};
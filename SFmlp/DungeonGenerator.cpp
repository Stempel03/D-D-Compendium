#include "DungeonGenerator.h"
#include <random>

DungeonGenerator::DungeonGenerator(unsigned seed) : rng_(seed) {}

void DungeonGenerator::generateRandomWalk(DungeonMap& map, int steps, int roomChance) {
    // 1. Wypełniamy wszystko ścianami
    for (int x = 0; x < map.width(); ++x) {
        for (int y = 0; y < map.height(); ++y) {
            map.set(x, y, TileType::Wall);
        }
    }

    // 2. Startujemy na środku
    int x = map.width() / 2;
    int y = map.height() / 2;

    std::uniform_int_distribution<int> dirDist(0, 3);
    std::uniform_int_distribution<int> chanceDist(1, 100);

    for (int i = 0; i < steps; ++i) {
        map.set(x, y, TileType::Floor);

        // Losowy ruch: 0-Góra, 1-Dół, 2-Lewo, 3-Prawo
        int dir = dirDist(rng_);
        if (dir == 0 && y > 1) y--;
        else if (dir == 1 && y < map.height() - 2) y++;
        else if (dir == 2 && x > 1) x--;
        else if (dir == 3 && x < map.width() - 2) x++;

        // Szansa na stworzenie małego pokoju w danym miejscu
        if (chanceDist(rng_) <= roomChance) {
            int rW = 3, rH = 3;
            for (int rx = x - 1; rx <= x + 1; ++rx) {
                for (int ry = y - 1; ry <= y + 1; ++ry) {
                    if (rx > 0 && rx < map.width() - 1 && ry > 0 && ry < map.height() - 1)
                        map.set(rx, ry, TileType::Floor);
                }
            }
        }
    }
}
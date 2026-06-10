#pragma once
#include "DungeonMap.h"
#include <random>

class DungeonGenerator {
public:
    explicit DungeonGenerator(unsigned seed = std::random_device{}());

    void generateRandomWalk(DungeonMap& map, int steps, int roomChance = 12);
    void fillBorderWalls(DungeonMap& map);

private:
    std::mt19937 rng_;
};

#pragma once
#include "CombatTracker.h"
#include "Database.h"
#include "DungeonGenerator.h"
#include "DungeonMap.h"

class App {
public:
    App();
    DungeonMap& getMap() { return map_; }
    DungeonGenerator& getGenerator() { return generator_; }
    CombatTracker& getCombat() { return combat_; }
    Database& getDatabase() { return database_; }

private:
    Database database_;
    DungeonMap map_;
    DungeonGenerator generator_;
    CombatTracker combat_;
};
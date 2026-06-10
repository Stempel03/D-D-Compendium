#pragma once
#include <string>
#include <vector>

struct MonsterTemplate {
    std::string name;
    int hp = 0;
    int attack = 0;
    int defense = 0;
    std::vector<std::string> effects;
};

struct StatusEffect {
    std::string name;
    int duration = 0;
};

class Database {
public:
    bool load(const std::string& monsterFile, const std::string& effectsFile);
    const std::vector<MonsterTemplate>& monsters() const;
    const std::vector<StatusEffect>& effects() const;

    const MonsterTemplate* findMonster(const std::string& name) const;

private:
    std::vector<MonsterTemplate> monsters_;
    std::vector<StatusEffect> effects_;

    void loadDefaults();
};

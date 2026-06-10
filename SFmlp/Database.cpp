#include "Database.h"
#include "Utils.h"
#include <fstream>
#include <sstream>

bool Database::load(const std::string& monsterFile, const std::string& effectsFile) {
    monsters_.clear();
    effects_.clear();

    std::ifstream mIn(monsterFile);
    std::ifstream eIn(effectsFile);

    if (!mIn.good() || !eIn.good()) {
        loadDefaults();
        return false;
    }

    std::string line;
    while (std::getline(mIn, line)) {
        line = utils::trim(line);
        if (line.empty() || line[0] == '#') continue;
        auto parts = utils::split(line, ',');
        if (parts.size() < 4) continue;

        MonsterTemplate m;
        m.name = utils::trim(parts[0]);
        m.hp = std::stoi(utils::trim(parts[1]));
        m.attack = std::stoi(utils::trim(parts[2]));
        m.defense = std::stoi(utils::trim(parts[3]));
        if (parts.size() >= 5) {
            auto eff = utils::split(parts[4], '|');
            for (auto& e : eff) {
                e = utils::trim(e);
                if (!e.empty()) m.effects.push_back(e);
            }
        }
        monsters_.push_back(m);
    }

    while (std::getline(eIn, line)) {
        line = utils::trim(line);
        if (line.empty() || line[0] == '#') continue;
        auto parts = utils::split(line, ',');
        if (parts.size() < 2) continue;

        StatusEffect eff;
        eff.name = utils::trim(parts[0]);
        eff.duration = std::stoi(utils::trim(parts[1]));
        effects_.push_back(eff);
    }

    if (monsters_.empty()) {
        loadDefaults();
        return false;
    }

    return true;
}

const std::vector<MonsterTemplate>& Database::monsters() const {
    return monsters_;
}

const std::vector<StatusEffect>& Database::effects() const {
    return effects_;
}

const MonsterTemplate* Database::findMonster(const std::string& name) const {
    for (const auto& m : monsters_) {
        if (m.name == name) return &m;
    }
    return nullptr;
}

void Database::loadDefaults() {
    monsters_ = {
        {"Goblin", 7, 2, 1, {"Poisoned"}},
        {"Ork", 15, 4, 2, {"Bleeding"}},
        {"Szkielet", 12, 3, 3, {"Fragile"}},
        {"Ogar Cienia", 20, 6, 2, {"Frightened"}}
    };

    effects_ = {
        {"Poisoned", 3},
        {"Bleeding", 2},
        {"Fragile", 4},
        {"Frightened", 1}
    };
}

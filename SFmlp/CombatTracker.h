#pragma once
#include <string>
#include <vector>
#include "Database.h"

struct Combatant {
    std::string name;
    int hp = 0;
    int maxHp = 0;
    int initiative = 0;
    bool isMonster = true;
    std::vector<StatusEffect> effects;
};

class CombatTracker {
public:
    void clear();
    void addCustom(const std::string& name, int hp, int initiative, bool isMonster = true);
    void addEffectToCurrent(const std::string& effectName, int duration);
    void rollInitiative();
    void removeDefeated();
    void nextTurn();
    void damageCurrent(int amount);
    void healCurrent(int amount);
    bool empty() const;

    std::vector<Combatant>& getCombatants() { return combatants_; }
    std::size_t getCurrentIndex() const { return currentIndex_; }
    std::string getEffectsString(const Combatant& c) const;

    // Obsługa stanu wpisywania nowego moba przez graficzne UI
    bool isAddingUIActive = false;
    std::string inputName = "";
    std::string inputHp = "";
    std::string inputInit = "";
    int activeInputField = 0; // 0: nazwa, 1: hp, 2: inicjatywa, 3: nowy stan (efekt)

private:
    std::vector<Combatant> combatants_;
    std::size_t currentIndex_ = 0;
};
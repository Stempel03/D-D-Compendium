#include "CombatTracker.h"
#include <algorithm>
#include <iostream>

void CombatTracker::clear() {
    combatants_.clear();
    currentIndex_ = 0;
}

void CombatTracker::addCustom(const std::string& name, int hp, int initiative, bool isMonster) {
    if (name.empty()) return;
    Combatant c;
    c.name = name;
    c.hp = hp;
    c.maxHp = hp;
    c.initiative = initiative;
    c.isMonster = isMonster;
    combatants_.push_back(c);
}

void CombatTracker::addEffectToCurrent(const std::string& effectName, int duration) {
    if (combatants_.empty() || effectName.empty()) return;
    combatants_[currentIndex_].effects.push_back({ effectName, duration });
}

void CombatTracker::rollInitiative() {
    for (auto& c : combatants_) {
        if (c.initiative == 0) c.initiative = (rand() % 20) + 1;
    }
    std::sort(combatants_.begin(), combatants_.end(), [](const Combatant& a, const Combatant& b) {
        return a.initiative > b.initiative;
        });
    currentIndex_ = 0;
}

void CombatTracker::removeDefeated() {
    combatants_.erase(std::remove_if(combatants_.begin(), combatants_.end(),
        [](const Combatant& c) { return c.hp <= 0; }), combatants_.end());
    if (currentIndex_ >= combatants_.size()) currentIndex_ = 0;
}

void CombatTracker::nextTurn() {
    if (combatants_.empty()) return;

    // Zmniejsz czas trwania efektów dla wychodzącego aktora
    for (auto it = combatants_[currentIndex_].effects.begin(); it != combatants_[currentIndex_].effects.end();) {
        it->duration--;
        if (it->duration <= 0) it = combatants_[currentIndex_].effects.erase(it);
        else ++it;
    }

    currentIndex_ = (currentIndex_ + 1) % combatants_.size();
}

void CombatTracker::damageCurrent(int amount) {
    if (combatants_.empty()) return;
    combatants_[currentIndex_].hp -= amount;
    if (combatants_[currentIndex_].hp < 0) combatants_[currentIndex_].hp = 0;
}

void CombatTracker::healCurrent(int amount) {
    if (combatants_.empty()) return;
    auto& c = combatants_[currentIndex_];
    c.hp += amount;
    if (c.hp > c.maxHp) c.hp = c.maxHp;
}

bool CombatTracker::empty() const { return combatants_.empty(); }

std::string CombatTracker::getEffectsString(const Combatant& c) const {
    if (c.effects.empty()) return "Brak";
    std::string res;
    for (const auto& e : c.effects) res += e.name + "[" + std::to_string(e.duration) + "t] ";
    return res;
}
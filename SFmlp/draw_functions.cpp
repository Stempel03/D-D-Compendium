#include <SFML/Graphics.hpp>
#include "DungeonMap.h"
#include "CombatTracker.h"
#include "Database.h"
#include "Shop.h"

void drawMapEditor(sf::RenderWindow& window, const sf::Font& font, DungeonMap& map, TileType& currentTileType) {
    map.draw(window);

    // Przygotowanie tekstu w prawym górnym rogu (screen space)
    std::string typeStr;
    switch (currentTileType) {
    case TileType::Empty: typeStr = "PUSTY"; break;
    case TileType::Floor: typeStr = "PODLOGA"; break;
    case TileType::Wall: typeStr = "SCIANA"; break;
    case TileType::Door: typeStr = "DRZWI"; break;
    case TileType::Trap: typeStr = "PULAPKA"; break;
    case TileType::Treasure: typeStr = "SKARB"; break;
    }
    std::string infoText = "Tryb edytora | Aktywny typ: " + typeStr +
        "\nKlawisze 1-6 zmieniaja typ | C - czysc | G - generuj | S/L - zapisz/wczytaj | PPM - przesun widok";

    sf::Text info(font, infoText, 16);
    info.setFillColor(sf::Color::White);

    // Ustawienie w prawym górnym rogu (z marginesem 10px)
    sf::FloatRect bounds = info.getLocalBounds();
    info.setPosition({ static_cast<float>(window.getSize().x) - bounds.size.x - 10.f, 10.f });

    // Zapamiętujemy aktualny widok, ustawiamy domyślny do rysowania UI, potem przywracamy
    sf::View originalView = window.getView();
    window.setView(window.getDefaultView());
    window.draw(info);
    window.setView(originalView);
}

void drawCombatTracker(sf::RenderWindow& window, const sf::Font& font, CombatTracker& tracker) {
    float y = 100.f;
    const auto& units = tracker.getCombatants();

    if (units.empty()) {
        sf::Text empty(font, "Brak uczestnikow. Uzyj klawiszy 1 (dodaj potwora) lub 2 (dodaj bohatera)", 20);
        empty.setFillColor(sf::Color::Yellow);
        empty.setPosition({ 50.f, y });
        window.draw(empty);
    }
    else {
        for (size_t i = 0; i < units.size(); ++i) {
            const auto& u = units[i];
            std::string info = u.name + " | HP: " + std::to_string(u.hp) + "/" + std::to_string(u.maxHp) +
                " | Init: " + std::to_string(u.initiative);
            std::string effects = tracker.getEffectsString(u);
            if (!effects.empty()) info += " | Eff: " + effects;

            sf::Text text(font, info, 20);
            text.setPosition({ 50.f, y });
            text.setFillColor(i == tracker.getCurrentIndex() ? sf::Color::Yellow : sf::Color::White);
            window.draw(text);
            y += 40.f;
        }
    }

    // Instrukcje na dole
    sf::Text help(font, "Sterowanie trackerem: 1-dodaj potwora | 2-dodaj bohatera | 3-inicjatywa | 4-nastepna tura | 5-obrazenia | 6-leczenie | 7-usun pokonanych | 8-wyczysc", 14);
    help.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect helpBounds = help.getLocalBounds();
    help.setPosition({ static_cast<float>(window.getSize().x) - helpBounds.size.x - 10.f, static_cast<float>(window.getSize().y) - 30.f });
    window.draw(help);
}

void drawDatabase(sf::RenderWindow& window, const sf::Font& font, const Database& db) {
    float y = 100.f;
    sf::Text title(font, "BAZA POTWOROW", 28);
    title.setPosition({ 50.f, 60.f });
    window.draw(title);

    for (const auto& m : db.monsters()) {
        sf::Text text(font, "- " + m.name + " (HP: " + std::to_string(m.hp) + ", ATK: " + std::to_string(m.attack) + ", DEF: " + std::to_string(m.defense) + ")", 18);
        text.setPosition({ 70.f, y });
        window.draw(text);
        y += 30.f;
    }

    y += 30.f;
    sf::Text effTitle(font, "EFEKTY:", 22);
    effTitle.setPosition({ 50.f, y });
    window.draw(effTitle);
    y += 30.f;
    for (const auto& e : db.effects()) {
        sf::Text text(font, "- " + e.name + " (czas: " + std::to_string(e.duration) + " rund)", 18);
        text.setPosition({ 70.f, y });
        window.draw(text);
        y += 30.f;
    }
}

void drawShop(sf::RenderWindow& window, const sf::Font& font, Shop& shop) {
    shop.draw(window, font);
}
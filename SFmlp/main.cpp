#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>

// Nagłówki Twoich modułów
#include "App.h"
#include "Shop.h"
#include "DmNotes.h"
#include "DungeonMap.h"

// DEKLARACJE FUNKCJI RYSUJĄCYCH
// Są one zdefiniowane w innych plikach (np. draw_functions.cpp), 
// tutaj tylko informujemy kompilator o ich istnieniu.
void drawMapEditor(sf::RenderWindow& window, const sf::Font& font, DungeonMap& map, TileType& currentTileType);
void drawCombatTracker(sf::RenderWindow& window, const sf::Font& font, CombatTracker& tracker);
void drawDatabase(sf::RenderWindow& window, const sf::Font& font, const Database& db);
void drawShop(sf::RenderWindow& window, const sf::Font& font, Shop& shop);

int main() {
    // Tworzenie okna aplikacji
    sf::RenderWindow window(sf::VideoMode({ 1200, 800 }), "Dungeon Builder & DM Tools");
    window.setFramerateLimit(60);

    sf::Font font;
    // Ścieżka do czcionki - upewnij się, że plik istnieje w tej lokalizacji
    if (!font.openFromFile("C:\\Users\\lukas\\source\\repos\\SFmlp\\SFmlp\\assets\\fonts\\DejaVuSans.ttf")) {
        std::cerr << "Blad: Nie mozna wczytac czcionki!\n";
        return -1;
    }

    // Inicjalizacja modułów
    App app;
    Shop shop;
    DmNotes dmNotes;

    // Próba wczytania danych z plików
    shop.loadFromFile("items.txt");
    dmNotes.loadFromFile("dm_notes.txt");

    // System zarządzania ekranami
    enum class Screen { MainMenu, MapEditor, CombatTracker, Database, Shop, DmNotes };
    Screen currentScreen = Screen::MainMenu;
    TileType currentTileType = TileType::Floor;

    sf::View defaultView = window.getDefaultView();

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            // Zamknięcie okna
            if (event->is<sf::Event::Closed>()) {
                dmNotes.saveToFile("dm_notes.txt"); // Automatyczny zapis notatek
                window.close();
            }

            // Obsługa skrótów klawiszowych w Menu Głównym
            if (currentScreen == Screen::MainMenu) {
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    switch (keyPressed->code) {
                    case sf::Keyboard::Key::Num1: currentScreen = Screen::MapEditor; break;
                    case sf::Keyboard::Key::Num2: currentScreen = Screen::CombatTracker; break;
                    case sf::Keyboard::Key::Num3: currentScreen = Screen::Database; break;
                    case sf::Keyboard::Key::Num4: currentScreen = Screen::Shop; break;
                    case sf::Keyboard::Key::Num5: currentScreen = Screen::DmNotes; break;
                    case sf::Keyboard::Key::Num0: window.close(); break;
                    default: break;
                    }
                }
            }
            // Powrót do menu (Esc)
            else {
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    // W notatniku Esc może mieć inne znaczenie (np. wyjście z edycji), 
                    // dlatego sprawdzamy go tylko jeśli nie jesteśmy w DmNotes lub gdy moduł na to pozwoli
                    if (keyPressed->code == sf::Keyboard::Key::Escape) {
                        currentScreen = Screen::MainMenu;
                        window.setView(defaultView);
                    }
                }
            }

            // Przekazywanie zdarzeń do aktywnego modułu
            if (currentScreen == Screen::Shop) {
                shop.handleEvent(*event, window);
            }
            else if (currentScreen == Screen::DmNotes) {
                dmNotes.handleEvent(*event, window);
            }
            else if (currentScreen == Screen::MapEditor) {
                app.getMap().handleEvent(*event, window);
            }
        }

        // Aktualizacja logiki (np. ruch kamery w edytorze map)
        if (currentScreen == Screen::MapEditor) {
            app.getMap().update(window);
        }

        // Renderowanie obrazu
        window.clear(sf::Color(30, 30, 40));

        if (currentScreen == Screen::MainMenu) {
            window.setView(defaultView);
            sf::Text title(font, "Dungeon Builder", 48);
            title.setPosition({ 150.f, 80.f });
            window.draw(title);

            std::vector<std::string> options = {
                "1 - Map Maker",
                "2 - Combat Tracker",
                "3 - Monsters Database",
                "4 - Shop Manager",
                "5 - DM Notes",
                "0 - Exit"
            };

            for (size_t i = 0; i < options.size(); ++i) {
                sf::Text opt(font, options[i], 28);
                opt.setPosition({ 350.f, 220.f + i * 60.f });
                window.draw(opt);
            }
        }
        else if (currentScreen == Screen::MapEditor) {
            drawMapEditor(window, font, app.getMap(), currentTileType);
        }
        else if (currentScreen == Screen::CombatTracker) {
            drawCombatTracker(window, font, app.getCombat());
        }
        else if (currentScreen == Screen::Database) {
            drawDatabase(window, font, app.getDatabase());
        }
        else if (currentScreen == Screen::Shop) {
            window.setView(defaultView);
            drawShop(window, font, shop);
        }
        else if (currentScreen == Screen::DmNotes) {
            window.setView(defaultView);
            dmNotes.draw(window, font);
        }

        window.display();
    }

    return 0;
}
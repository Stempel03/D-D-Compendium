#include "Shop.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

Shop::Shop() {
    loadFromFile("items.txt"); // Domyślne ładowanie bazy przy starcie
}

bool Shop::loadFromFile(const std::string& path) {
    database.clear();
    // Przedmioty startowe (hardcoded) na wypadek braku pliku
    database.emplace_back("Miecz", "weapon", 150, "Atak +2");
    database.emplace_back("Tarcza", "armor", 1250, "Obrona +1");
    database.emplace_back("Amulet", "accessory", 10500, "Moc");

    // Parsowanie pliku tekstowego formatu: Nazwa|Typ|Cena|Opis
    std::ifstream file(path);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string name, type, desc;
            int price;
            if (std::getline(ss, name, '|') && std::getline(ss, type, '|') &&
                (ss >> price) && std::getline(ss, desc)) {
                if (!desc.empty() && desc[0] == '|') desc.erase(0, 1);
                database.emplace_back(name, type, price, desc);
            }
        }
        return true;
    }
    return !database.empty();
}

void Shop::reset() {
    shelf.clear();
    selectedShelfIdx = -1;
    isEditingPrice = false;
}

void Shop::draw(sf::RenderWindow& window, const sf::Font& font) {
    // 1. Rysowanie pustych komórek siatki (tło sklepu)
    for (int i = 0; i < gridCols; ++i) {
        for (int j = 0; j < gridRows; ++j) {
            sf::RectangleShape cell(sf::Vector2f(cellWidth - 2.f, cellHeight - 2.f));
            cell.setPosition(sf::Vector2f(gridOffset.x + i * cellWidth, gridOffset.y + j * cellHeight));
            cell.setFillColor(sf::Color(45, 45, 50));
            window.draw(cell);
        }
    }

    // 2. Rysowanie przedmiotów umieszczonych na półkach
    for (int i = 0; i < (int)shelf.size(); ++i) {
        sf::RectangleShape r(sf::Vector2f(cellWidth - 4.f, cellHeight - 4.f));
        r.setPosition(sf::Vector2f(shelf[i].gridPos.x + 2.f, shelf[i].gridPos.y + 2.f));
        r.setFillColor(sf::Color(60, 100, 60)); // Zielonkawy kolor przedmiotu

        // Wyróżnienie zaznaczonego przedmiotu (Edycja: Magenta, Wybór: Cyan)
        if (i == selectedShelfIdx) {
            r.setOutlineThickness(3.f);
            r.setOutlineColor(isEditingPrice ? sf::Color::Magenta : sf::Color::Cyan);
        }
        else {
            r.setOutlineThickness(1.f);
            r.setOutlineColor(sf::Color::Yellow);
        }
        window.draw(r);

        // Napisy: Nazwa i sformatowana cena
        sf::Text nameTxt(font, shelf[i].name, 11);
        nameTxt.setPosition(sf::Vector2f(shelf[i].gridPos.x + 8.f, shelf[i].gridPos.y + 6.f));
        window.draw(nameTxt);

        sf::Text priceTxt(font, shelf[i].getFormattedPrice(), 11);
        priceTxt.setPosition(sf::Vector2f(shelf[i].gridPos.x + 8.f, shelf[i].gridPos.y + 28.f));
        priceTxt.setFillColor(sf::Color(255, 215, 0));
        window.draw(priceTxt);
    }

    // 3. Panel boczny z listą dostępnych przedmiotów
    if (panelVisible) {
        float windowW = static_cast<float>(window.getSize().x);
        sf::RectangleShape bg(sf::Vector2f(panelWidth, static_cast<float>(window.getSize().y)));
        bg.setPosition(sf::Vector2f(windowW - panelWidth, 0.f));
        bg.setFillColor(sf::Color(35, 35, 45, 250));
        window.draw(bg);

        float entryY = 60.f;
        for (size_t i = 0; i < database.size(); ++i) {
            sf::RectangleShape r(sf::Vector2f(panelWidth - 30.f, 45.f));
            r.setPosition(sf::Vector2f(windowW - panelWidth + 15.f, entryY));
            r.setFillColor(sf::Color(80, 80, 110));
            database[i].rect = r; // Aktualizacja prostokąta kolizji
            window.draw(r);

            sf::Text t(font, database[i].name, 13);
            t.setPosition(sf::Vector2f(windowW - panelWidth + 25.f, entryY + 12.f));
            window.draw(t);
            entryY += 55.f;
        }
    }

    // Wyświetlanie komunikatu trybu edycji ceny
    if (isEditingPrice) {
        sf::Text help(font, "EDYTUJESZ CENE: Wpisz liczbe i kliknij obok (jednostka: srebro)", 15);
        help.setPosition(sf::Vector2f(50.f, 30.f));
        help.setFillColor(sf::Color::Magenta);
        window.draw(help);
    }

    // Rysowanie "ducha" (półprzezroczystego kwadratu) podczas przeciągania
    if (dragging && draggedItem) {
        sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        sf::RectangleShape ghost(sf::Vector2f(cellWidth, cellHeight));
        ghost.setPosition(sf::Vector2f(mPos.x - dragOffset.x, mPos.y - dragOffset.y));
        ghost.setFillColor(sf::Color(255, 255, 255, 120));
        window.draw(ghost);
    }
}

void Shop::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    // A. Obsługa wpisywania nowej ceny za pomocą klawiatury numerycznej
    if (isEditingPrice && selectedShelfIdx != -1) {
        if (const auto* textEvent = event.getIf<sf::Event::TextEntered>()) {
            if (textEvent->unicode == 8) { // Backspace - usuwanie znaku
                if (!inputBuffer.empty()) inputBuffer.pop_back();
            }
            else if (textEvent->unicode >= '0' && textEvent->unicode <= '9') { // Cyfry
                if (inputBuffer.length() < 8) inputBuffer += static_cast<char>(textEvent->unicode);
            }

            // Natychmiastowa aktualizacja ceny w obiekcie na półce
            if (!inputBuffer.empty()) shelf[selectedShelfIdx].price = std::stoi(inputBuffer);
            else shelf[selectedShelfIdx].price = 0;
        }
    }

    // B. Skróty klawiszowe (Tab - panel, R - reset)
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Tab) panelVisible = !panelVisible;
        if (key->code == sf::Keyboard::Key::R) reset();
    }

    // C. Obsługa Myszy (Kliknięcie i Przeciąganie)
    if (const auto* pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2f mPos = window.mapPixelToCoords(sf::Vector2i(pressed->position.x, pressed->position.y));

        if (pressed->button == sf::Mouse::Button::Left) {
            bool foundOnShelf = false;
            // Sprawdzenie, czy kliknięto w przedmiot na półce (do przemieszczenia lub edycji)
            for (size_t i = 0; i < shelf.size(); ++i) {
                sf::FloatRect b(shelf[i].gridPos, sf::Vector2f(cellWidth, cellHeight));
                if (b.contains(mPos)) {
                    selectedShelfIdx = static_cast<int>(i);
                    isEditingPrice = true;
                    inputBuffer = std::to_string(shelf[i].price);
                    startDrag(new ShopItem(shelf[i]), mPos, true, static_cast<int>(i));
                    foundOnShelf = true;
                    break;
                }
            }

            // Jeśli nie kliknięto w półkę, sprawdź panel boczny (dodawanie nowych przedmiotów)
            if (!foundOnShelf) {
                if (panelVisible && mPos.x > window.getSize().x - panelWidth) {
                    for (size_t i = 0; i < database.size(); ++i) {
                        if (database[i].rect.getGlobalBounds().contains(mPos)) {
                            startDrag(new ShopItem(database[i]), mPos, false);
                            break;
                        }
                    }
                }
                isEditingPrice = false;
                selectedShelfIdx = -1;
            }
        }
    }
    else if (const auto* released = event.getIf<sf::Event::MouseButtonReleased>()) {
        // Upuszczenie przedmiotu
        if (released->button == sf::Mouse::Button::Left && dragging) {
            sf::Vector2f mPos = window.mapPixelToCoords(sf::Vector2i(released->position.x, released->position.y));
            float gridMaxX = gridOffset.x + (gridCols * cellWidth);
            float gridMaxY = gridOffset.y + (gridRows * cellHeight);

            // Jeśli upuszczono w obrębie siatki - wyrównaj do komórki (Snapping)
            if (mPos.x > gridOffset.x && mPos.x < gridMaxX && mPos.y > gridOffset.y && mPos.y < gridMaxY) {
                float gx = std::floor((mPos.x - gridOffset.x) / cellWidth) * cellWidth + gridOffset.x;
                float gy = std::floor((mPos.y - gridOffset.y) / cellHeight) * cellHeight + gridOffset.y;

                draggedItem->gridPos = sf::Vector2f(gx, gy);
                if (isFromShelf && shelfIdx != -1) shelf[shelfIdx] = *draggedItem; // Przesunięcie istniejącego
                else shelf.push_back(*draggedItem); // Dodanie nowego
            }
            // Jeśli wyciągnięto przedmiot z półki poza siatkę - usuń go
            else if (isFromShelf && shelfIdx != -1) {
                shelf.erase(shelf.begin() + shelfIdx);
                selectedShelfIdx = -1;
                isEditingPrice = false;
            }
            stopDrag();
        }
    }
}

void Shop::startDrag(ShopItem* item, sf::Vector2f mousePos, bool fromShelf, int idx) {
    draggedItem = item;
    dragging = true;
    isFromShelf = fromShelf;
    shelfIdx = idx;
    // Środek "ducha" przedmiotu pod kursorem
    dragOffset = sf::Vector2f(cellWidth / 2.f, cellHeight / 2.f);
}

void Shop::stopDrag() {
    dragging = false;
    if (draggedItem) {
        delete draggedItem;
        draggedItem = nullptr;
    }
}
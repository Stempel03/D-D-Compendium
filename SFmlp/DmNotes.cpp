#include "DmNotes.h"
#include <fstream>
#include <sstream>
#include <iostream>

DmNotes::DmNotes() {
    // Domyślna sesja startowa, jeśli plik nie istnieje
    if (!loadFromFile("dm_notes.txt")) {
        SessionNotes s1;
        s1.sessionNumber = 1;
        s1.date = "12-05-2026";
        s1.notes.push_back({ "Bohater: Aragorn", "Uzyl mikstury leczenia w zlym momencie.", NoteCategory::Hero, {300.f, 100.f} });
        s1.notes.push_back({ "NPC: Karczmarz", "Wspomnial o ukrytym skarbie w lochach.", NoteCategory::NPC, {550.f, 150.f} });
        sessions.push_back(s1);
    }
}

sf::Color DmNotes::getCategoryColor(NoteCategory cat) const {
    switch (cat) {
    case NoteCategory::Hero: return sf::Color(173, 216, 230); // Jasnoniebieski
    case NoteCategory::NPC:  return sf::Color(144, 238, 144); // Jasnozielony
    default:                 return sf::Color(255, 255, 150); // Żółty sticky note
    }
}

std::string DmNotes::wrapText(const std::string& str, unsigned width, const sf::Font& font, unsigned charSize) {
    std::string wrapped;
    std::string line;
    std::stringstream ss(str);
    std::string word;

    // Proste dzielenie po spacjach (dla uproszczenia ignorujemy podziały wewnątrz bardzo długich słów)
    while (ss >> word) {
        sf::Text text(font, line + word + " ", charSize);
        if (text.getLocalBounds().size.x > width) {
            wrapped += line + "\n";
            line = word + " ";
        }
        else {
            line += word + " ";
        }
    }
    wrapped += line;
    return wrapped;
}

void DmNotes::draw(sf::RenderWindow& window, const sf::Font& font) {
    // 1. Tło obszaru roboczego (Tablica korkowa / ciemne tło)
    sf::RectangleShape bg(sf::Vector2f(window.getSize().x - panelWidth, static_cast<float>(window.getSize().y)));
    bg.setPosition({ panelWidth, 0.f });
    bg.setFillColor(sf::Color(40, 40, 45));
    window.draw(bg);

    // 2. Rysowanie karteczek (Sticky Notes) dla aktywnej sesji
    if (activeSessionIdx >= 0 && activeSessionIdx < (int)sessions.size()) {
        auto& activeNotes = sessions[activeSessionIdx].notes;
        for (auto& note : activeNotes) {
            // Cień karteczki
            sf::RectangleShape shadow(noteSize);
            shadow.setPosition(note.pos + sf::Vector2f(5.f, 5.f));
            shadow.setFillColor(sf::Color(0, 0, 0, 80));
            window.draw(shadow);

            // Główna karteczka
            sf::RectangleShape rect(noteSize);
            rect.setPosition(note.pos);
            rect.setFillColor(getCategoryColor(note.category));
            if (&note == editedNote) {
                rect.setOutlineThickness(3.f);
                rect.setOutlineColor(editingTitle ? sf::Color::Red : sf::Color::Blue);
            }
            window.draw(rect);

            // Tytuł
            sf::Text titleTxt(font, note.title.empty() ? "[Tytul]" : note.title, 14);
            titleTxt.setPosition(note.pos + sf::Vector2f(10.f, 10.f));
            titleTxt.setFillColor(sf::Color::Black);
            titleTxt.setStyle(sf::Text::Bold);
            window.draw(titleTxt);

            // Linia oddzielająca
            sf::RectangleShape line(sf::Vector2f(noteSize.x - 20.f, 1.f));
            line.setPosition(note.pos + sf::Vector2f(10.f, 30.f));
            line.setFillColor(sf::Color(0, 0, 0, 100));
            window.draw(line);

            // Treść (zawijana)
            std::string dispText = note.content.empty() ? "[Tresc - kliknij by pisac]" : note.content;
            sf::Text contentTxt(font, wrapText(dispText, noteSize.x - 20, font, 12), 12);
            contentTxt.setPosition(note.pos + sf::Vector2f(10.f, 35.f));
            contentTxt.setFillColor(sf::Color(30, 30, 30));
            window.draw(contentTxt);
        }
    }

    // 3. Panel boczny z listą sesji
    sf::RectangleShape panelBg(sf::Vector2f(panelWidth, static_cast<float>(window.getSize().y)));
    panelBg.setFillColor(sf::Color(25, 25, 30));
    window.draw(panelBg);

    float startY = 20.f;
    sf::Text headerTxt(font, "NOTATKI DM", 16);
    headerTxt.setPosition({ 20.f, startY });
    headerTxt.setStyle(sf::Text::Bold);
    window.draw(headerTxt);
    startY += 40.f;

    // Przycisk "Nowa Sesja"
    sf::RectangleShape newSessBtn({ panelWidth - 20.f, 30.f });
    newSessBtn.setPosition({ 10.f, startY });
    newSessBtn.setFillColor(sf::Color(70, 70, 90));
    window.draw(newSessBtn);
    sf::Text btnTxt(font, "+ Nowa Sesja", 13);
    btnTxt.setPosition({ 25.f, startY + 6.f });
    window.draw(btnTxt);
    startY += 40.f;

    // Lista sesji
    for (int i = 0; i < (int)sessions.size(); ++i) {
        sf::Text sessTxt(font, "Sesja #" + std::to_string(sessions[i].sessionNumber) + " (" + sessions[i].date + ")", 13);
        sessTxt.setPosition({ 15.f, startY });
        if (i == activeSessionIdx) sessTxt.setFillColor(sf::Color::Yellow);
        window.draw(sessTxt);
        startY += 25.f;

        // Przycisk dodawania notatki w aktywnej sesji
        if (i == activeSessionIdx) {
            sf::Text addNoteTxt(font, "  [+ Dodaj karteczke]", 12);
            addNoteTxt.setPosition({ 15.f, startY });
            addNoteTxt.setFillColor(sf::Color(100, 255, 100));
            window.draw(addNoteTxt);
            startY += 25.f;

            // Instrukcja zmiany kategorii
            sf::Text infoTxt(font, "  Tab - zmien typ\n  Klik - tytul/tresc", 10);
            infoTxt.setPosition({ 15.f, startY });
            infoTxt.setFillColor(sf::Color(150, 150, 150));
            window.draw(infoTxt);
            startY += 30.f;
        }
    }
}

void DmNotes::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // A. Obsługa wpisywania tekstu do zaznaczonej notatki
    if (editedNote) {
        if (const auto* textEvent = event.getIf<sf::Event::TextEntered>()) {
            std::string& targetStr = editingTitle ? editedNote->title : editedNote->content;

            if (textEvent->unicode == 8) { // Backspace
                if (!targetStr.empty()) targetStr.pop_back();
            }
            else if (textEvent->unicode == 9) { // Tab - zmiana kategorii
                int c = static_cast<int>(editedNote->category);
                editedNote->category = static_cast<NoteCategory>((c + 1) % 3);
            }
            else if (textEvent->unicode == 13) { // Enter
                if (!editingTitle) targetStr += "\n";
                else editingTitle = false; // Enter w tytule przechodzi do treści
            }
            else if (textEvent->unicode >= 32 && textEvent->unicode < 128) { // Zwykłe znaki
                targetStr += static_cast<char>(textEvent->unicode);
            }
            saveToFile("dm_notes.txt"); // Auto-zapis po znaku
        }
    }

    // B. Obsługa myszy (Kliknięcia)
    if (const auto* pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (pressed->button == sf::Mouse::Button::Left) {
            // 1. Sprawdzenie kliknięcia w przycisk "+ Nowa Sesja"
            if (sf::FloatRect({ 10.f, 60.f }, { panelWidth - 20.f, 30.f }).contains(mPos)) {
                SessionNotes nextSess;
                nextSess.sessionNumber = sessions.empty() ? 1 : sessions.back().sessionNumber + 1;
                nextSess.date = "DD-MM-YYYY"; // Można podpiąć systemową datę
                sessions.push_back(nextSess);
                activeSessionIdx = (int)sessions.size() - 1;
                saveToFile("dm_notes.txt");
                return;
            }

            // 2. Sprawdzenie kliknięcia w elementy listy sesji
            float startY = 100.f;
            for (int i = 0; i < (int)sessions.size(); ++i) {
                if (sf::FloatRect({ 10.f, startY }, { panelWidth, 20.f }).contains(mPos)) {
                    activeSessionIdx = i;
                    editedNote = nullptr;
                    return;
                }
                startY += 25.f;

                if (i == activeSessionIdx) {
                    // Kliknięcie w "+ Dodaj karteczke"
                    if (sf::FloatRect({ 15.f, startY }, { 150.f, 20.f }).contains(mPos)) {
                        // Dodajemy na środku
                        sessions[i].notes.push_back({ "", "", NoteCategory::General, {panelWidth + 50.f, 100.f} });
                        editedNote = &sessions[i].notes.back();
                        editingTitle = true;
                        saveToFile("dm_notes.txt");
                        return;
                    }
                    startY += 55.f; // przeskakujemy przycisk i instrukcje
                }
            }

            // 3. Sprawdzenie kliknięcia w karteczki (obszar roboczy)
            if (activeSessionIdx >= 0 && activeSessionIdx < (int)sessions.size()) {
                editedNote = nullptr; // Reset
                auto& activeNotes = sessions[activeSessionIdx].notes;
                // Sprawdzamy od końca (te rysowane najwyżej)
                for (auto it = activeNotes.rbegin(); it != activeNotes.rend(); ++it) {
                    if (sf::FloatRect(it->pos, noteSize).contains(mPos)) {
                        editedNote = &(*it);
                        // Jeśli kliknięto górną część -> edycja tytułu
                        editingTitle = (mPos.y < it->pos.y + 30.f);
                        break;
                    }
                }
            }
        }
    }
}

bool DmNotes::saveToFile(const std::string& path) const {
    std::ofstream file(path);
    if (!file) return false;

    for (const auto& sess : sessions) {
        file << "SESSION|" << sess.sessionNumber << "|" << sess.date << "\n";
        for (const auto& note : sess.notes) {
            // Zamiana \n na <br>, żeby nie złamać linii w pliku
            std::string safeContent = note.content;
            size_t pos = 0;
            while ((pos = safeContent.find("\n", pos)) != std::string::npos) {
                safeContent.replace(pos, 1, "<br>");
                pos += 4;
            }
            file << "NOTE|" << note.title << "|" << safeContent << "|"
                << static_cast<int>(note.category) << "|" << note.pos.x << "|" << note.pos.y << "\n";
        }
    }
    return true;
}

bool DmNotes::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) return false;

    sessions.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        if (std::getline(ss, type, '|')) {
            if (type == "SESSION") {
                SessionNotes s;
                std::string numStr;
                std::getline(ss, numStr, '|');
                s.sessionNumber = std::stoi(numStr);
                std::getline(ss, s.date);
                sessions.push_back(s);
            }
            else if (type == "NOTE" && !sessions.empty()) {
                StickyNote n;
                std::string catStr, xStr, yStr;
                std::getline(ss, n.title, '|');
                std::getline(ss, n.content, '|');
                std::getline(ss, catStr, '|');
                std::getline(ss, xStr, '|');
                std::getline(ss, yStr);

                // Odtwarzanie \n z <br>
                size_t pos = 0;
                while ((pos = n.content.find("<br>", pos)) != std::string::npos) {
                    n.content.replace(pos, 4, "\n");
                    pos += 1;
                }

                n.category = static_cast<NoteCategory>(std::stoi(catStr));
                n.pos = { std::stof(xStr), std::stof(yStr) };
                sessions.back().notes.push_back(n);
            }
        }
    }
    return !sessions.empty();
}
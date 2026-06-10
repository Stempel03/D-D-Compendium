#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum class NoteCategory { General, Hero, NPC };

struct StickyNote {
    std::string title;
    std::string content;
    NoteCategory category = NoteCategory::General;
    sf::Vector2f pos; // Pozycja na ekranie roboczym

    StickyNote() = default;
    StickyNote(const std::string& t, const std::string& c, NoteCategory cat, sf::Vector2f p)
        : title(t), content(c), category(cat), pos(p) {
    }
};

struct SessionNotes {
    int sessionNumber = 1;
    std::string date;
    std::vector<StickyNote> notes;
    bool isExpanded = false; // Czy lista notatek w panelu jest rozwinięta
};

class DmNotes {
public:
    DmNotes();
    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path) const;

    void draw(sf::RenderWindow& window, const sf::Font& font);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);

private:
    std::vector<SessionNotes> sessions;
    int activeSessionIdx = 0; // Która sesja jest aktualnie wyświetlana na tablicy

    // Stan edycji tekstu
    StickyNote* editedNote = nullptr;
    bool editingTitle = false; // true = edycja tytułu, false = edycja treści

    // Ustawienia wizualne
    float panelWidth = 220.f;
    sf::Vector2f noteSize = { 200.f, 200.f };

    // Pomocnicza funkcja do zawijania tekstu w SFML
    std::string wrapText(const std::string& str, unsigned width, const sf::Font& font, unsigned charSize);
    sf::Color getCategoryColor(NoteCategory cat) const;
};
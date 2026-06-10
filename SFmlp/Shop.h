#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// Struktura reprezentująca pojedynczy przedmiot w sklepie
struct ShopItem {
    std::string name;        // Nazwa przedmiotu
    std::string type;        // Typ (np. broń, pancerz)
    int price = 0;           // Cena przechowywana w najmniejszej jednostce (Srebro)[cite: 1]
    std::string description; // Opis przedmiotu
    sf::Vector2f gridPos;    // Pozycja przedmiotu na siatce sklepu
    sf::RectangleShape rect; // Kształt używany do detekcji kliknięć w panelu bocznym

    // Przelicza cenę ze srebra na system: Platyna, Złoto, Srebro (1p = 100g, 1g = 100s)
    std::string getFormattedPrice() const {
        if (price <= 0) return "0s";
        int p = price / 10000;             // 100 * 100
        int g = (price % 10000) / 100;
        int s = price % 100;

        std::string res = "";
        if (p > 0) res += std::to_string(p) + "p ";
        if (g > 0) res += std::to_string(g) + "g ";
        if (s > 0 || res == "") res += std::to_string(s) + "s";
        return res;
    }

    ShopItem() = default;
    ShopItem(const std::string& n, const std::string& t, int p, const std::string& d)
        : name(n), type(t), price(p), description(d) {
    }
};

class Shop {
public:
    Shop();
    bool loadFromFile(const std::string& path);           // Wczytywanie bazy przedmiotów
    void draw(sf::RenderWindow& window, const sf::Font& font); // Renderowanie interfejsu
    void handleEvent(const sf::Event& event, sf::RenderWindow& window); // Obsługa myszy i klawiatury
    void reset(); // Czyści przedmioty wystawione na półkach

private:
    std::vector<ShopItem> database; // Wszystkie dostępne przedmioty w grze
    std::vector<ShopItem> shelf;    // Przedmioty aktualnie wystawione w sklepie

    // Ustawienia wizualne siatki (gridu)
    int gridCols = 8;
    int gridRows = 12;
    float cellWidth = 145.f;
    float cellHeight = 55.f;
    sf::Vector2f gridOffset = sf::Vector2f(50.f, 80.f);

    // Stan UI i edycji
    bool panelVisible = true;       // Czy boczny panel z bazą jest widoczny
    float panelWidth = 260.f;
    int selectedShelfIdx = -1;      // Indeks wybranego przedmiotu na półce
    std::string inputBuffer;        // Przechowuje wpisywane znaki podczas zmiany ceny
    bool isEditingPrice = false;    // Czy gracz jest w trybie wpisywania ceny

    // Mechanika Drag & Drop
    bool dragging = false;          // Czy aktualnie coś przeciągamy
    ShopItem* draggedItem = nullptr; // Wskaźnik na kopię przeciąganego przedmiotu
    sf::Vector2f dragOffset;        // Przesunięcie kursora względem środka przedmiotu
    bool isFromShelf = false;       // Czy przedmiot był zabrany z półki (true) czy z bazy (false)
    int shelfIdx = -1;              // Oryginalny indeks na półce (do aktualizacji pozycji)

    void startDrag(ShopItem* item, sf::Vector2f mousePos, bool fromShelf, int idx = -1);
    void stopDrag();
};
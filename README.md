# Dokumentacja Techniczna: Dungeon Builder & DM Tools

Aplikacja wspomagająca Mistrza Gry (DM) napisana w języku **C++** z wykorzystaniem biblioteki **SFML (w wersji 3.0.2)**. Program łączy w sobie funkcje edytora map lochów, managera walki, bazy potworów, kreatora sklepu oraz systemu notatek sesyjnych.

---

## 1. Architektura Systemu i Przepływ Danych

Program opiera się na architekturze modułowej zarządzanej przez maszynę stanów (reprezentowaną przez ekran główny i ekrany modułów w `main.cpp`). Głównym kontenerem danych systemowych jest klasa `App`, która unika globalnego stanu, agregując kluczowe podsystemy.

```
       [ main.cpp (Pętla Główna i UI) ]
                       |
                       v
               [ Klasa App ]
                       |
   +-------------------+-------------------+-------------------+
   |                   |                   |                   |
[DungeonMap]     [CombatTracker]      [Database]            [Shop]
   |                   |                   |                   |
[DungeonGenerator]     |             (monsters.db)        (items.txt)
                       v             (effects.db)
                  [Combatant]

```

---

## 2. Opis Modułów i Klas

### Moduł Główny (`App`, `main`, `utils`)

* **`main.cpp`**: Inicjalizuje okno aplikacji (**1200x800**), ładuje czcionki oraz obsługuje główną pętlę zdarzeń i przełączanie ekranów (`Screen::MainMenu`, `Screen::MapEditor` itp.).
* **`App`**: Klasa pełniąca rolę fasady. Przechowuje instancje obiektów `DungeonMap`, `DungeonGenerator`, `CombatTracker` oraz `Database`, udostępniając je poprzez referencje.
* **`utils` (Przestrzeń nazw)**: Zawiera funkcje pomocnicze do przetwarzania tekstu (`trim`, `split`) oraz bezpiecznego odczytu strumienia wejściowego z konsoli (`readInt`).

---

### Moduł Mapy i Generatora (`DungeonMap`, `DungeonGenerator`)

Odpowiada za renderowanie dwuwymiarowej siatki kafelków oraz automatyczne tworzenie układu lochów.

* **`DungeonMap`**:
* Zarządza dynamiczną siatką kafelków `Tile` o strukturze dwuwymiarowego wektora (`std::vector<std::vector<Tile>>`).
* Obsługuje typy kafelków (`TileType`): `Empty`, `Floor`, `Wall`, `Door`, `Trap`, `Treasure`.
* Implementuje zaawansowaną obsługę kamery (`sf::View`), umożliwiając **zoomowanie** (scroll myszy) oraz **przesuwanie widoku** (przeciąganie Prawym Przyciskiem Myszy).


* **`DungeonGenerator`**:
* Wykorzystuje silnik losowości `std::mt19937`.
* Implementuje algorytm **Random Walk** (`generateRandomWalk`), który generuje organiczne kształty jaskiń i korytarzy na podstawie zadanej liczby kroków i szansy na pokój.



---

### Moduł Walki (`CombatTracker`)

Klasa zarządzająca kolejnością inicjatywy oraz stanem zdrowia uczestników starcia.

* **Struktura `Combatant**`: Przechowuje parametry walczącego: imię, aktualne HP, maksymalne HP, wartość inicjatywy oraz aktywne efekty statusowe (`StatusEffect`).
* **Mechanika działania**:
* `rollInitiative()`: Losuje inicjatywę dla postaci (rzut kostką k20) i sortuje wektor uczestników malejąco.
* `nextTurn()`: Przesuwa indeks aktywnej tury w pętli kołowej.
* Umożliwia dynamiczne zadawanie obrażeń, leczenie oraz automatyczne usuwanie ciał z listy (`removeDefeated`).



---

### Moduł Baz Danych (`Database`)

Odpowiada za wczytywanie zewnętrznych szablonów obiektów gry.

* **Format plików wejściowych**:
* `monsters.db`: Plik CSV, format linii: `Nazwa, HP, Atak, Obrona, Efekty|Separowane|Potokiem`
* `effects.db`: Format linii: `Nazwa_Efektu, Czas_Trwania`


* W przypadku braku plików komponent automatycznie wywołuje metodę `loadDefaults()`, zabezpieczając aplikację przed crashem.

---

### Moduł Sklepu (`Shop`)

Zarządza ekspozycją przedmiotów oraz nietypowym systemem walutowym.

* **System Walutowy**: Przechowuje ceny w najmniejszej jednostce (Srebro). Posiada wbudowany parser formatujący koszt do tradycyjnego systemu RPG: **1p (Platyna) = 100g (Złoto)**, **1g = 100s (Srebro)**.
* **Mechanika Drag & Drop**:
* Pozwala na przeciąganie myszą przedmiotów z bocznego panelu bazy danych na siatkę regałów sklepowych.
* Implementuje **Snapping** (przyciąganie) przedmiotów do najbliższej komórki siatki (`cellWidth: 145.f`, `cellHeight: 55.f`).
* Wyrzucenie przedmiotu poza obszar siatki skutkuje jego usunięciem z regału.



---

### Moduł Notatek (`DmNotes`)

System wirtualnej tablicy korkowej podzielonej na sesje RPG.

* **Struktura notatki**: Każdy obiekt `StickyNote` posiada pozycję dwuwymiarową `sf::Vector2f`, dzięki czemu może być dowolnie rozmieszczany na ekranie. Kolor notatki zależy od kategorii (`General` = Żółty, `Hero` = Jasnoniebieski, `NPC` = Jasnozielony).
* **Persystencja danych (`dm_notes.txt`)**:
* Zapis i odczyt odbywają się za pomocą formatu tekstowego opartego na separatorze `|`.
* Wielolinijkowa treść notatek jest serializowana poprzez zamianę znaków nowej linii (`\n`) na tager `<br>`, co zapobiega uszkodzeniu struktury pliku podczas odczytu.



---

## 3. Sterowanie i Interfejs Użytkownika

Program jest sterowany hybrydowo przy użyciu myszy i klawiatury. Poniższe tabele zestawiają klawiszologię dla poszczególnych ekranów:

### Edytor Mapy (`MapEditor`)

| Klawisz / Akcja | Funkcja |
| --- | --- |
| **Klawisze 1 - 6** | Wybór aktywnego typu kafelka (Pusty, Podłoga, Ściana itd.) |
| **PPM + Ruch myszy** | Przesuwanie kamery edytora |
| **Scroll myszy** | Przybliżanie / Oddalanie widoku |
| **Klawisz G** | Uruchomienie automatycznego generatora (Random Walk) |
| **Klawisz C** | Czyszczenie całej mapy |
| **Klawisz S / L** | Zapis (`save`) / Odczyt (`load`) układu mapy |

### Tracker Walki (`CombatTracker`)

| Klawisz | Funkcja |
| --- | --- |
| **1** | Dodaj potwora z bazy danych |
| **2** | Dodaj niestandardowego bohatera |
| **3** | Rzuć na inicjatywę i posortuj kolejkę |
| **4** | Następna tura |
| **5 / 6** | Zadaj obrażenia / Ulecz aktualną postać |
| **7** | Usuń poległe postacie (HP = 0) |
| **8** | Wyczyść całe starcie |

---

## 4. Specyfikacja Formatów Zapisu Plików

### Format Notatek (`dm_notes.txt`)

Plik zapisywany jest linia po linii. Zastosowano dwa znaczniki struktury:

```text
SESSION|NumerSesji|DataSesji
NOTE|TytułNotatki|Treść z zamienionymi tagami <br>|Kategoria|PozycjaX|PozycjaY

```

### Format Sklepu (`items.txt`)

Prosty format tekstowy parsujący przedmioty do bazy sklepu:

```text
Miecz|weapon|150|Atak +2
Tarcza|armor|1250|Obrona +1
Amulet|accessory|10500|Moc magiczna

```

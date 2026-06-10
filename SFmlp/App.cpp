#include "App.h"

App::App() : map_(20, 12), generator_() {
    database_.load("data/monsters.db", "data/effects.db");
}
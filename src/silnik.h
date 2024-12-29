#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <vector>
#include <random>
using namespace std;
class Game {
private:
    long double money;                   // Obecna ilość pieniędzy
    long double multiplier;              // Aktualny mnożnik
    bool p[5][5];                        // Plansza z bombami
    int bombs_cnt;                       // Liczba bomb
    int bet;                             // Postawiona stawka
    int left;                            // Ilość pozostałych pól do kliknięcia
    vector<std::pair<int, int>> bombs; // Wektor z pozycjami bomb
    mt19937 rng;                    // Generator liczb losowych

public:
    // Konstruktor
    Game();

    // Ustawianie pieniędzy
    void set_money(int m);

    // Rozpoczynanie nowej gry
    void new_game(int how_many, int price);

    // Kliknięcie w pole
    int click(int x, int y);

    // Zakończenie gry (wypłata wygranej)
    void game_end();

    // Pobranie obecnej ilości pieniędzy
    long double get_money();

    // Sprawdzenie, czy pole zawiera bombę
    bool get_p(int x, int y);

    // Pobranie potencjalnej wygranej
    long double get_potential();

    // Pobranie obecnego mnożnika
    long double get_multiplier();
};

#endif // GAME_H

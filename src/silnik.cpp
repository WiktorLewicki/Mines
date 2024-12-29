#include "silnik.h"
#include <chrono>
#include <algorithm>

Game::Game() {
    rng = std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count());
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            bombs.push_back({i, j});
        }
    }
}

void Game::set_money(int m) {
    money = m;
}

void Game::new_game(int how_many, int price) {
    multiplier = 0.98;
    left = 25;
    bombs_cnt = how_many;
    bet = price;
    money -= bet;
    std::shuffle(bombs.begin(), bombs.end(), rng);

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            p[i][j] = false;
        }
    }

    for (int i = 0; i < bombs_cnt; i++) {
        std::pair<int, int> b = bombs[i];
        p[b.first][b.second] = true;
    }
}

int Game::click(int x, int y) {
    left--;
    if (p[x][y]) {
        multiplier = 0;
        return 0;
    } else {
        multiplier *= (long double)1.0 / ((long double)(left + 1 - bombs_cnt) / (long double)(left + 1));
        if (left - bombs_cnt == 0) return 2;
        return 1;
    }
}

void Game::game_end() {
    money += multiplier * bet;
}

long double Game::get_money() {
    return money;
}

bool Game::get_p(int x, int y) {
    return p[x][y];
}

long double Game::get_potential() {
    return multiplier * bet;
}

long double Game::get_multiplier() {
    return multiplier;
}

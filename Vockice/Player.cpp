#include "Headers/Player.h"
#include <iostream>

Player::Player(int initialBalance)
    : balance(initialBalance), currentBet(10), totalSpins(0), totalWinnings(0), totalLosses(0), highestWin(0) {}

int Player::getBalance() const {
    return balance;
}

int Player::getCurrentBet() const {
    return currentBet;
}

void Player::increaseBet() {
    if (currentBet + 10 <= balance) { // Ne dozvoli opkladu veću od balansa
        currentBet += 10;
    }
}

void Player::decreaseBet() {
    if (currentBet - 10 >= 0) { // Ne dozvoli opkladu manju od 0
        currentBet -= 10;
    }
}

void Player::placeBet() {
    if (currentBet <= balance) {
        balance -= currentBet; // Oduzmi trenutni bet od balansa
    }
}

void Player::addWinnings(int amount) {
    balance += amount; // Dodaj osvojeni iznos balansu
}

void Player::addFunds(int amount) {
    balance += amount; // Dodaj sredstva balansu
}

int Player::getTotalSpins() const {
    return totalSpins;
}

int Player::getTotalWinnings() const {
    return totalWinnings;
}

int Player::getTotalLosses() const {
    return totalLosses;
}

int Player::getHighestWin() const {
    return highestWin;
}

void Player::recordSpin(int result) {
    totalSpins++;

    if (result > 0) {
        totalWinnings += result;
        if (result > highestWin) {
            highestWin = result; // Ažuriraj najveći dobitak
        }
    }
    else {
        totalLosses += currentBet; // Dodaj trenutni bet u gubitke
    }
}

void Player::printStatistics() const {
    std::cout << "\n--- Game Statistics ---\n";
    std::cout << "Total Spins: " << totalSpins << "\n";
    std::cout << "Total Winnings: " << totalWinnings << "\n";
    std::cout << "Total Losses: " << totalLosses << "\n";
    std::cout << "Net Balance: " << (totalWinnings - totalLosses) << "\n";
    std::cout << "Highest Win: " << highestWin << "\n";
    std::cout << "Final Balance: " << balance << "\n";
    std::cout << "-----------------------\n";
}

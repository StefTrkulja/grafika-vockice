#ifndef PLAYER_H
#define PLAYER_H

class Player {
private:
    int balance;      // Trenutni balans igrača
    int currentBet;   // Trenutna opklada igrača
    int totalSpins;   // Ukupan broj povlačenja ručice
    int totalWinnings; // Ukupan iznos osvojenih para
    int totalLosses;  // Ukupan iznos izgubljenih para
    int highestWin;   // Najveći dobitak tokom igre

public:
    Player(int initialBalance);

    int getBalance() const;
    int getCurrentBet() const;


    int getTotalSpins() const;
    int getTotalWinnings() const;
    int getTotalLosses() const;
    int getHighestWin() const;


    void increaseBet();
    void decreaseBet();
    void placeBet();  
    void addWinnings(int amount);
    void addFunds(int amount);

    void recordSpin(int result);
    void printStatistics() const;
};

#endif

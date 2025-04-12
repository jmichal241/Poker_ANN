#ifndef TABLE_H
#define TABLE_H

#include <algorithm> 
#include "player.h"





enum GameStatus {
    PREFLOP,
    FLOP,
    TURN,
    RIVER,
    FINISHED
};

class Table{
private:
    Deck deck;
    vector<Player> players;
    GameStatus status;
    int pot;
    int button=0;
    int raise=0;
    Card publicCards[5];
public:
    Table();
    void GameLoop();
    void resetboard();
    
    //operacje na karciochach
    void deal();        //rozdanie kart
    void flop();        //flop cards show
    void turn();        //turn card show
    void river();       //river card show
    void showdown();    //card showdown, 

    void smallBlind();
    void bigBlind();
    void passButton();
    int getPot();
    int allActionMade();
    int defineHand(Player);   //check how strong is your hand
    int defineWinner();//define who wins this hand

    //funckje pomocnicze
    void displayBoard();
    void displayPlayers();
    void displayPot();
};

#endif 
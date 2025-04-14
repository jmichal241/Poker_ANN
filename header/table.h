#ifndef TABLE_H
#define TABLE_H

#include <algorithm> 
#include <unordered_map>
#include <set> 
#include <fstream>
#include <sstream>
#include <string>
#include "player.h"
#include "kloziobot.h"
#include "GPT1bot.h"
#include "GPT2bot.h"
#include "lukibot.h"






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
    std::vector<Player*> players;
    GameStatus status;
    int pot;
    int button=0;
    int raise=0;
    Card publicCards[5];
public:
    Table();
    void GameLoop();
    void resetboard();
    
    //Cards operations
    void deal();        //Cards for players
    void flop();        //flop cards show
    void turn();        //turn card show
    void river();       //river card show
    void showdown();    //card showdown, 

    //Game organizing staff
    void smallBlind();
    void bigBlind();
    void passButton();
    int allActionMade();

    //Methods for players
    int getPot();
    int getRaise();
    int getButton();

    //Methods to define winners
    vector<int> defineHand(Player);   //check how strong is your hand
    vector<int> defineWinner();       //define who wins this hand

    //Debug functions
    void displayBoard();
    void displayPlayers();
    void displayPot();

    //Creating dataset
    void createHeader(int);
    void registerAction(Action,int, int);
    void registerWin(int, int);
};

#endif 
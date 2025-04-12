#ifndef PLAYER_H
#define PLAYER_H


#include "deck.h"

enum Action{
    CHECK,
    CALL,
    PASS,
    RAISE,
    NONE
};

class Player{
private:
    int stack=0;    //money
    bool button;  //dealer button
    Card hand[2];   //hand
    int Raise=0;    //raise number
    Action action;
public:
    Player(int);
    void display();
    void getCard(Card);
    Action makeAction(int);
    Action getAction();
    void resetRaise();
    void resetHand();
    void changeStack(int);
    void changeButton(bool);
    int getStack();
    Card returnCard(int);

    //akcje
    void check();
    void pass();
    void raise(int);
    void call();
    //boty
    void alwaysCall();
};



#endif 
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
protected:
    int stack=0;    //money
    bool button;  //dealer button
    Card hand[2];   //hand
    int Raise=0;    //raise number
    Action action;
    int potAgency=0;
public:
    Player(int);
    void display();
    void getCard(Card);

    virtual Action makeAction(int raiseMoney, int pot, int button);

    void setAction(Action);
    Action getAction();
    int getRaise();
    void resetRaise();
    void resetHand();
    void changeStack(int);
    void changeButton(bool);
    int getStack();
    Card returnCard(int);
    void changePotAgency(int);
    int getPotAgency();

    //akcje
    void check();
    void pass();
    void raise(int);
    void call();
};



#endif 
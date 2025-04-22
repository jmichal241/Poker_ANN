#include "../header/lukibot.h"
#include <iostream>
#include <cstdlib>  // For random actions, if needed
#define BIG 10

// In Kloziobot constructor
Lukibot::Lukibot(int initialStack) : Player(initialStack) {}

Action Lukibot::makeAction(int raiseMoney, int pot, int tableButton) {

    int handScore = 0;
    int colorScore = 5;
    int pairScore = 10;
    int cardsConnected = 4;
    int cardsLongConnected = 3;
    int cardsVeryLongConnected = 1;
    int highCardScore = 2;
    int midCardScore = 1;
    int aceBonus = 1;

    // hand bonuses

    if (hand[0].getColour() == hand[1].getColour()) handScore += colorScore;
    if (hand[0].getNumber() == hand[1].getNumber()) handScore += pairScore;

    int higherCardValue = std::max(hand[0].getNumber(), (hand[1].getNumber()));
    int lowerCardValue = std::min(hand[0].getNumber(), (hand[1].getNumber()));

    if (higherCardValue - 1 == lowerCardValue) handScore += cardsConnected;
    else if (higherCardValue - 2 == lowerCardValue) handScore += cardsLongConnected;
    else if (higherCardValue - 3 == lowerCardValue) handScore += cardsVeryLongConnected;


    if (hand[0].getNumber() >= 12) handScore += highCardScore;
    else if (hand[0].getNumber() >= 8) handScore += midCardScore;
    if (hand[1].getNumber() >= 12) handScore += highCardScore;
    else if (hand[1].getNumber() >= 8) handScore += midCardScore;

    if (hand[0].getNumber() == 14) handScore += aceBonus;
    if (hand[1].getNumber() == 14) handScore += aceBonus;

    if (lowerCardValue == 2 &&
        higherCardValue == 7 &&
        hand[0].getColour() != hand[1].getColour()) handScore += 3;



    // placement



    // stack, pot itp.

    



    if(stack<=0){
        setAction(PASS);
        return PASS;
    }
    setAction(CALL);
    return CALL;

}
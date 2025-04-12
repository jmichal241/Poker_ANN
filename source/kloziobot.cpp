#include "../header/kloziobot.h"
#include <iostream>
#include <cstdlib>  // For random actions, if needed
#define BIG 10

// In Kloziobot constructor
Kloziobot::Kloziobot(int initialStack) : Player(initialStack) {}



Action Kloziobot::makeAction(int raiseMoney, int pot, int tableButton) {
    
    // Hand strength evaluation logic
    bool isSuited = 0, isConnected = 0, isPair = 0, bothHigh = 0, bothVeryHigh = 0, haveAce = 0, bothWeak = 0, bothVeryWeak = 0;
    
    if (hand[0].getColour() == hand[1].getColour()) isSuited = 1;
    if (hand[0].getNumber() == hand[1].getNumber()) isPair = 1;
    if (hand[0].getNumber() == hand[1].getNumber() - 1 || hand[0].getNumber() == hand[1].getNumber() + 1) isConnected = 1;
    else if ((hand[0].getNumber() == 2 && hand[1].getNumber() == 14) || (hand[0].getNumber() == 14 && hand[1].getNumber() == 2))
        isConnected = 1;

    if (hand[0].getNumber() + hand[1].getNumber() >= 24) bothVeryHigh = 1;
    else if (hand[0].getNumber() + hand[1].getNumber() >= 20) bothHigh = 1;
    else if (hand[0].getNumber() + hand[1].getNumber() <= 5) bothVeryWeak = 1;
    else if (hand[0].getNumber() + hand[1].getNumber() <= 10) bothWeak = 1;

    if (hand[0].getNumber() == 14 || hand[1].getNumber() == 14) haveAce = 1;

    int handStrength = isSuited * 2 + isConnected * 1 + isPair * 4 + bothHigh * 2 + bothVeryHigh * 4 + haveAce * 2 + bothWeak * (-2) + bothVeryWeak * (-3);
    
    // std::cout << "My hand is: " << endl;
    // hand[0].display();
    // hand[1].display();
    // std::cout << "My strength is: " << handStrength << std::endl;
    if(stack<=0){
        setAction(PASS);
        return PASS;
    }
    if(handStrength < 1 && button != 1){
        setAction(PASS);
        return PASS;
    }
    
    else if(handStrength >= 3 && raiseMoney <= 10){
        setAction(RAISE);
        Raise = std::min(2 * BIG, stack);
        return RAISE;
    }
    else if(handStrength >= 5 && raiseMoney <= 10){
        setAction(RAISE);
        Raise = std::min(4 * BIG, stack);
        return RAISE;
    }
    else if(handStrength >= 7 && raiseMoney <= 10){
        setAction(RAISE);
        Raise = std::min(6 * BIG, stack);
        return RAISE;
    }
    else if (raiseMoney <= 10){
        setAction(CALL);
        return CALL;
    }
    
    if(raiseMoney >= 20 && raiseMoney <= 40 && handStrength >= 3 && handStrength < 5){
        setAction(CALL);
        return CALL;
    }
    else if(raiseMoney >= 20 && raiseMoney <= 40 && handStrength >= 5 && handStrength < 7){
        setAction(RAISE);
        Raise = std::min(2 * raiseMoney, stack);
        return RAISE;
    }
    else if(raiseMoney >= 20 && raiseMoney <= 40 && handStrength >= 7){
        setAction(RAISE);
        Raise = std::min(4 * raiseMoney, stack);
        return RAISE;
    }
    else{
        setAction(PASS);
        return PASS;
    }
    
    if(raiseMoney >= 40 && raiseMoney <= 100 && handStrength >= 5){
        setAction(CALL);
        return CALL;
    }
    else if(raiseMoney >= 40 && raiseMoney <= 100 && handStrength >= 8){
        setAction(RAISE);
        Raise = std::min(4 * raiseMoney, stack);
        return RAISE;
    }
    else{
        setAction(PASS);
        return PASS;
    }
    
    if(raiseMoney >= 100 && handStrength >= 7 && handStrength < 9){
        setAction(CALL);
        return CALL;
    }
    else if(raiseMoney >= 100 && handStrength >= 9){
        setAction(RAISE);
        Raise = std::min(4 * raiseMoney, stack);
        return RAISE;
    }
    else {
        setAction(PASS);
        return PASS;      
    }
    
    setAction(PASS);
    return PASS;
    
}
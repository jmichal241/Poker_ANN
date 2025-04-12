#include "../header/kloziobot.h"
#include <iostream>
#include <cstdlib>  // For random actions, if needed

Kloziobot::Kloziobot(int initialStack) : Player(initialStack) {}


Action Kloziobot::makeAction(int raise,int pot, int button) {
    bool isSuited=0, isConnected=0, isPair=0;
    if(hand[0].getColour()==hand[1].getColour())
        isSuited=1;



    return raise > 0 ? RAISE : CALL;  // Example logic
}

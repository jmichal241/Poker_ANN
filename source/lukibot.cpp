#include "../header/lukibot.h"
#include <iostream>
#include <cstdlib>  // For random actions, if needed
#define BIG 10

// In Kloziobot constructor
Lukibot::Lukibot(int initialStack) : Player(initialStack) {}

Action Lukibot::makeAction(int raiseMoney, int pot, int tableButton) {

    if(stack<=0){
        setAction(PASS);
        return PASS;
    }
    setAction(CALL);
    return CALL;

}
#include "../header/player.h"

Player::Player(int Stack){
    stack = Stack;
    button=0;
    // cout << stack << endl;
}

void Player::display(){
    
    cout << "stack: " << stack << " button: " << button << " HAND: ";
    hand[0].display();
    hand[1].display();
    cout << endl;
}

void Player::getCard(Card card){
    if(hand[0].getNumber()==0){
        hand[0]=card;
    }
    else 
        hand[1]=card;
}
void Player::resetRaise(){
    Raise=0;
}
void Player::resetHand(){
    hand[0].setNumber(0);
    hand[0].setColour(SPADE);
    hand[1].setNumber(0);
    hand[1].setColour(SPADE);
    action=NONE;
}
void Player::changeStack(int change){
    stack = stack + change;
}
void Player::changeButton(bool change){
    button = change;
}
int Player::getStack(){
    return stack;
}
void Player::call(){
    action=CALL;
}

void Player::check(){
    action=CHECK;
}
void Player::pass(){
    action=PASS;
}

void Player::raise(int number){
    action=RAISE;
    Raise=Raise + number;
}

Action Player::getAction(){
    return action;
}
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
Action Player::makeAction(int raiseMoney){
    int x=0;
    cout << "Przed" << action << endl;
    while(true){
        cout << "Give number from 0 - 3" << endl;
        cout << "Option 0 - Check" << endl;
        cout << "Option 1 - Call" << endl;
        cout << "Option 2 - Fold" << endl;
        cout << "Option 3 - Raise" << endl;
        cin >> x;
        
        if(x>=0 && x<4){
            cout << "Correct number" << endl;
            if(x==0){
                action=CHECK;
            }
            else if(x==1){
                action=CALL;
            }
            else if(x==2){
                action=PASS;
            }
            else{
                action=RAISE;
                cout << "Give raise amount" << endl;
                cin >> Raise;
            }
            break;
        }
        else {
            cout << "Wrong data, try again" << endl;
        }
    }
    cout << "Po" << action << endl;
    return action;
}
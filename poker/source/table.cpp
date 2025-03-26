#include "../header/table.h"
#define STACK_SIZE 1000
#define PLAYER 5
#define SMALL 5
#define BIG 10

Table::Table(){
    for (int i = 1; i <= PLAYER; ++i) {
        players.emplace_back(STACK_SIZE);
        players[i].resetHand();
        players[i].changeStack(STACK_SIZE);
        players[i].changeButton(0);
    }
    deck.shuffle();
}

void Table::flop(){
    publicCards[0] = deck.draw();
    publicCards[1]= deck.draw();
    publicCards[2]= deck.draw();
}

void Table::turn(){
    publicCards[3]= deck.draw();
}

void Table::river(){
    publicCards[4]= deck.draw();
}


void Table::resetboard(){
    for(int i=0; i<PLAYER; i++){
        players[i].resetHand();
    }
    pot=0;
    for(int i=0; i<5; i++){
        publicCards[i].setNumber(0);
    }
}
void Table::deal(){
    int num;
    for(int i=0; i<2;i++){
        for(int j=0; j<PLAYER; j++){
            num = ((j + button) % PLAYER);  // numPlayers is an integer variable

            players[num].getCard(deck.draw());
        }
    }
}
void Table::smallBlind(){
    int temp = (button + 1) % PLAYER;
    int money = players[temp].getStack()-SMALL;
    players[temp].changeStack(money);
}
void Table::bigBlind(){
    int temp = (button + 2) % PLAYER;
    int money = players[temp].getStack()-BIG;
    players[temp].changeStack(money);
}
void Table::passButton(){
    button = (button + 1) % PLAYER;
}
void Table::GameLoop(){
    resetboard();
    while(1){
        status = PREFLOP;
        smallBlind();
        bigBlind();
        deal();
        //preflop


        //flop
        status = FLOP;
        flop();
        //turn
        status = TURN;
        turn(); 
        //river
        status = RIVER;
        river();
        //showdown




        // passButton();
    }

}

void Table::displayBoard(){
    cout << "Board: " << endl;
    for(int i=0; i<5; i++){
        if(publicCards[i].getNumber()!=0){
            publicCards[i].display();
        }
    }
}

void Table::displayPlayers(){
    for(int i=0; i<PLAYER; i++){
        players[i].display();
    }
}
void Table::displayPot(){
    cout << pot << endl;
}
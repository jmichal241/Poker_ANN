#include "../header/table.h"
#define STACK_SIZE 1000
#define PLAYER 5
#define SMALL 5
#define BIG 10

Table::Table(){
    for (int i = 0; i < PLAYER; ++i) {
        players.emplace_back(STACK_SIZE);
        players[i].resetHand();
        // players[i].changeButton(0);
    }
    players[0].changeButton(1);
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
    deck.shuffle();
    for(int i=0; i<2;i++){
        for(int j=0; j<PLAYER; j++){
            num = ((j + button) % PLAYER);  // numPlayers is an integer variable

            players[num].getCard(deck.draw());
        }
    }
}
void Table::smallBlind(){
    int temp = (button + 1) % PLAYER;
    players[temp].changeStack(-SMALL);
}
void Table::bigBlind(){
    int temp = (button + 2) % PLAYER;
    players[temp].changeStack(-BIG);
}
void Table::passButton(){
    button = (button + 1) % PLAYER;
    for(int i=0; i<PLAYER; i++){
        players[i].changeButton(0);
    }
    players[button].changeButton(1);
}
int Table::allActionMade(){
    int pass=0;
    int call=0;
    int check=0;
    Action action=NONE;
    for(int i=0; i<PLAYER; i++){
        if (players[i].getAction()==PASS)
            pass++;
        if (players[i].getAction()==CHECK)
            check++;
    }
    if(pass==(PLAYER-1))
        return 1;               //everyone fold
    if(check==PLAYER)
        return 2;               //everyone checked
    return 0;
}       
void Table::GameLoop(){
    resetboard();
    while(1){
        status = PREFLOP;
        smallBlind();
        bigBlind();
        raise=BIG;
        deal();
        int counter=0;
        while(allActionMade()==0){
            players[(button+counter)%PLAYER].makeAction(raise);
            counter++;
        }
        
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




        passButton();
        for(int i=0; i<PLAYER; i++){
            players[i].resetHand();
        }
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
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
    players.emplace_back(STACK_SIZE);
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
        // while(allActionMade()==0){
        //     players[(button+counter)%PLAYER].makeAction(raise);
        //     counter++;
        // }
        
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

        // defineWinner();
        for(int i=0;i<PLAYER;i++){
            defineHand(players[i]);
        }
        // defineHand(players[0]);

        passButton();
        for(int i=0; i<PLAYER; i++){
            players[i].resetHand();
        }
        break;
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

int Table::defineHand(Player player){

    //Part to make 7 cards
    vector<Card> cards(7);
    for (int i = 0; i < 5; i++) {
        cards[i] = publicCards[i];
    }
    cards[5] = player.returnCard(0);
    cards[6] = player.returnCard(1);
    //Part to sort them in order of their numbers
    std::sort(cards.begin(), cards.end(), [](const Card& a, const Card& b) {
        return a.getNumber() < b.getNumber();  // Sorting by the card's number (value)
    });  



    //count colours
    int spades=0, clubs=0, hearts=0, diamonds=0;
    Colour colour;
    for (int i = 0; i < 7; i++) {
        colour=cards[i].getColour();
        if(colour==0)
            spades++;
        else if (colour==1)
            clubs++;
        else if (colour==2)
            diamonds++;
        else
            hearts++;
    }
    bool isStraight=0, isFlash=0;
    //Check flush
    if(spades>=5 || diamonds>=5 || clubs>=5 || hearts>=5)
        isFlash==1;

    //Check straight
    int straight=1;


    if(cards[0].getNumber()==2 && cards[6].getNumber()==14){
        straight++;
    }
    for(int i=1; i<7;i++){
        // cout << "First card = " << cards[i-1].getNumber() << " Second card = " << cards[i].getNumber() << endl;
        if(cards[i].getNumber()==cards[i-1].getNumber()+1){
            // cout << "First card = " << cards[i-1].getNumber() << " Second card = " << cards[i].getNumber() << endl;
            straight++;
            
        }
        else if(cards[i].getNumber()==cards[i-1].getNumber()){
            //Do nothing
        }
        else    
            straight=1;
        if(straight==5){
            isStraight=1;
            break;
        }
    }

    if(isStraight){
        for (int i = 0; i < 7; i++) {
            cards[i].display();
        }
        cout << "Cards for straight" << straight << endl;
    }
    //royal flush
    
    return 9;
    //straight flush
    return 8;
    //4 of a kind
    return 7;
    //full house
    return 6;
    //flush
    return 5;
    //straight
    return 4;
    //3 of a kind
    return 3;
    //2 pair
    return 2;
    //a pair

    return 1;
    //high card
    
    return 0;

}
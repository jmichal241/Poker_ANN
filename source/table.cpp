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
            cout << defineHand(players[i]) << endl;;
            
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
    for(int i=0;i<7;i++){
        cards[i].display();
    }
    //count colours
    int spades=0, clubs=0, hearts=0, diamonds=0;
    Colour colour;
    int flushColour;
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

    bool isStraight=0, isFlush=0;

    //Check flush
    if(spades>=5){
        isFlush=1;
        flushColour=0;
    }
    else if(clubs>=5){
        isFlush=1;
        flushColour=1;
    }
    else if(diamonds>=5){
        isFlush=1;
        flushColour=2;
    }
    else if(hearts>=5){
        isFlush=1;
        flushColour=3;
    }
    //Check straight
    int consecutive = 1;

    if (cards[0].getNumber() == 2 && cards[6].getNumber() == 14) {
        consecutive++;
    }
    
    for (int i = 1; i < 7; i++) {
        int prev = cards[i - 1].getNumber();
        int curr = cards[i].getNumber();
    
        if (curr == prev + 1) {
            consecutive++;
        } else if (curr != prev) {
            consecutive = 1;
        }
    
        if (consecutive == 5) {
            isStraight = true;
            break;
        }
    }

    //check for 4 of a kind
    bool isFourOfKind = false;
    int currentCount = 1;

    for (int i = 1; i < 7; i++) {
        if (cards[i].getNumber() == cards[i - 1].getNumber()) {
            currentCount++;
            if (currentCount == 4) {
                isFourOfKind = true;
                break;
            }
        } else {
            currentCount = 1;
        }
    }
    //check for full houses
    bool isFullHouse = false;
    int counts[15] = {0}; // Index 2–14 used for card numbers

    for (int i = 0; i < 7; i++) {
        counts[cards[i].getNumber()]++;
    }

    bool hasThree = false;
    bool hasPair = false;

    for (int i = 2; i <= 14; i++) {
        if (counts[i] >= 3 && !hasThree) {
            hasThree = true;
            counts[i] -= 3; // Remove the triple to avoid double-counting it as a pair
        }
    }

    for (int i = 2; i <= 14; i++) {
        if (counts[i] >= 2) {
            hasPair = true;
            break;
        }
    }

    if (hasThree && hasPair) {
        isFullHouse = true;
    }

    //Checking for 3 of a kind, 2 pairs and a pair
    bool isThreeOfKind = false;
    bool isTwoPair = false;
    bool isOnePair = false;

    int pairCount = 0;

    for (int i = 2; i <= 14; i++) {
        if (counts[i] == 3) {
            isThreeOfKind = true;
        }
        else if (counts[i] == 2) {
            pairCount++;
        }
    }

    if (pairCount >= 2) {
        isTwoPair = true;
    }
    else if (pairCount == 1) {
        isOnePair = true;
    }

    bool isRoyal=0, isStraightFlush=0;
    //checking for royal/straight flush
    if(isFlush){
        int count = 1;
        int prev = -1;
        int firstInSequence = -1;

        for (int i = 0; i < 7; i++) {
            if (cards[i].getColour() == flushColour) {
                if (prev == -1) {
                    count = 1;
                    firstInSequence = cards[i].getNumber();
                } else if (cards[i].getNumber() == prev + 1) {
                    count++;
                } else if (cards[i].getNumber() != prev) {
                    count = 1;
                    firstInSequence = cards[i].getNumber();
                }
                prev = cards[i].getNumber();
            }

            if (count == 5) {
                isStraightFlush = true;
                if (prev == 14 && firstInSequence == 10) {
                    isRoyal = true;
                }
                break;
            }
        }
    }

    //royal flush
    if(isRoyal)
        return 9;
    //straight flush
    else if(isStraightFlush)
        return 8;
    //4 of a kind
    else if(isFourOfKind)
        return 7;
    //full house
    else if(isFullHouse)
        return 6;
    //flush
    else if(isFlush)
        return 5;
    //straight
    else if(isStraight)
        return 4;
    //3 of a kind
    else if(isThreeOfKind)
        return 3;
    //2 pair
    else if(isTwoPair)
        return 2;
    //a pair
    else if(isOnePair)
        return 1;
    //high card
    else
        return 0;

}
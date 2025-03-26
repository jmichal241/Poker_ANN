#include "../header/deck.h"
#include <iostream>
#include <algorithm>
#include <random>

using namespace std;


Deck::Deck(){
    int counter=0;
    Colour colour=SPADE;
    

    for(int j=0;j<4;j++){
        
        if(j==0){
            colour=SPADE;
        }
        else if(j==1){
            colour=CLUBS;
        }
        else if(j==2){
            colour=HEART;
        }
        else if(j==3){
            colour=DIAMOND;
        }
        
        for(int i=2;i<15;i++){
            cards.push_back(Card(0,DIAMOND));
            cards[counter].setNumber(i);
            cards[counter].setColour(colour);
            counter++;
        }
        
    }
    
}

void Deck::display() {
    cout << "Displaying Deck of Cards:" << endl;
    for(int i=0; i<52; i++){
        cards[i].display();
    }
}

void Deck::shuffle(){
    std::random_device rd;  
    std::mt19937 g(rd());  
    std::shuffle(cards.begin(), cards.end(), g);
}

Card Deck::draw(){
    Card tempCard = cards.back();  // Store the last card
    cards.pop_back();              // Remove it from the end
    cards.insert(cards.begin(), tempCard); // Move it to the front
    // tempCard.display();
    return tempCard;   
}
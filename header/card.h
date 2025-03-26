#ifndef CARD_H
#define CARD_H

#include <iostream>

using namespace std;


enum Colour {
    SPADE,   // Changed to uppercase
    CLUBS,
    DIAMOND,
    HEART 
};

class Card {
private:
    int number;
    Colour colour;
public:
    Card() : number(0), colour(SPADE) {}
    Card(int NUMBER, Colour COLOUR);

    int getNumber();
    void setNumber(int);
    Colour getColour();
    void setColour(Colour);
    void display();
};
#endif // CARD_H
#include "../header/deck.h"

std::string colourToString(Colour colour) {
    switch(colour) {
        case SPADE:
            return "Spade";
        case CLUBS:
            return "Clubs";
        case DIAMOND:
            return "Diamond";
        case HEART:
            return "Heart";
        default:
            return "Unknown"; // Default case for handling unknown enum values
    }
}

Card::Card(int NUMBER, Colour COLOUR) : number(NUMBER), colour(COLOUR) {}

void  Card::display(){
    cout << number << "  " << colourToString(colour) << endl;
}

int Card::getNumber(){
    return number;
}

Colour Card::getColour(){
    return colour;
}

void Card::setNumber(int NUMBER){
    number = NUMBER;
}
void Card::setColour(Colour COLOUR){
    colour=COLOUR;
}
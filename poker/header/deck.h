#ifndef DECK_H
#define DECK_H
#define STACK 100

#include "card.h"
#include <iostream>
#include <string>
#include <vector>

class Deck{
private:
    std::vector<Card> cards;
public:
    Deck();
    void display();
    void shuffle();
    Card draw();
};

#endif 
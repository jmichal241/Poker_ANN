#include <iostream>
#include <vector>
#include <string>
#include "../header/table.h"

using namespace std;


int main()
{

    Table table;
    table.GameLoop();

    return 0;
}

// Deck deck;
// deck.shuffle();
// deck.display();
// Player player(100);
// player.getCard(deck.draw());
// player.getCard(deck.draw());
// player.display();
// Kloziobot player(1000);  // Create a stack-allocated Kloziobot object
// player.makeAction(100, 500, 1);  // Calls makeAction on the Kloziobot object (direct call)

// Player player(100);
// Kloziobot bot1(100);
// player.makeAction(100);
// table.deal();
// table.displayPlayers();
// table.smallBlind();
// table.bigBlind();
// table.displayPlayers(); 
// table.passButton();
// table.smallBlind();
// table.bigBlind();
// table.displayPlayers(); 
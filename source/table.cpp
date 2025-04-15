#include "../header/table.h"
#define STACK_SIZE 1000
#define PLAYER 5
#define SMALL 5
#define BIG 10

// In Table class constructor (table.cpp)
Table::Table() {
    players.push_back(new Kloziobot(STACK_SIZE));  // Use pointer to store Kloziobot
    players[0]->resetHand();  // Reset hand for Kloziobot
    players.push_back(new Kloziobot(STACK_SIZE));  // Add another Kloziobot as the second player
    players[1]->resetHand();  // Reset hand for the second Kloziobot
    players.push_back(new Kloziobot(STACK_SIZE));  // Add another Kloziobot as the third player
    players[2]->resetHand();  // Reset hand for the third Kloziobot
    players.push_back(new Kloziobot(STACK_SIZE));  // Add another Kloziobot as the third player
    players[3]->resetHand(); 
    players.push_back(new Kloziobot(STACK_SIZE));  // Add another Kloziobot as the third player
    players[4]->resetHand(); 

    players[0]->changeButton(1);  // Assign button to the first player (the first Kloziobot)
    deck.shuffle();  // Shuffle the deck
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
        players[i]->resetHand();
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

            players[num]->getCard(deck.draw());
        }
    }
}

void Table::smallBlind() {
    int temp = (button + 1) % PLAYER;

    // Szukaj gracza z kasą
    for (int i = 0; i < PLAYER; ++i) {
        int idx = (temp + i) % PLAYER;
        if (players[idx]->getStack() > 0) {
            players[idx]->changeStack(-SMALL);
            pot += SMALL;
            break;
        }
    }
}

void Table::bigBlind() {
    int temp = (button + 2) % PLAYER;

    // Szukaj gracza z kasą
    for (int i = 0; i < PLAYER; ++i) {
        int idx = (temp + i) % PLAYER;
        if (players[idx]->getStack() > 0) {
            players[idx]->changeStack(-BIG);
            pot += BIG;
            break;
        }
    }
}

void Table::passButton(){
    button = (button + 1) % PLAYER;
    for(int i=0; i<PLAYER; i++){
        players[i]->changeButton(0);
    }
    players[button]->changeButton(1);
}

int Table::allActionMade(){
    int pass = 0;
    int check = 0;
    int call = 0;
    int raise = 0;
    Action action = NONE;

    for(int i = 0; i < PLAYER; i++){
        action = players[i]->getAction();

        if(action == PASS) pass++;
        if(action == CHECK) check++;
        if(action == CALL) call++;
        if(action == RAISE) raise++;
    }

    // Everyone folded (only pass left)
    if(pass == PLAYER-1) return 1;  // everyone folded

    // Everyone checked (no one raised or called)
    if(check + pass== PLAYER) return 2;     // everyone checked

    // Everyone has called or there was a raise (round complete)
    if(call + pass + raise== PLAYER ) return 3; // Everyone called, or a raise occurred

    return 0;  // Actions are still in progress
}
       
void Table::GameLoop() {
    resetboard();
    int handCounter = 0;

    while (true) {
        vector<int> winners;
        pot = 0;

        // End the game after x hands
        if (handCounter == 1) {
            break;
        }
        createHeader(handCounter);

        
        // Reset hands for all players
        for (int i = 0; i < PLAYER; i++) {
            players[i]->resetHand();
        }
        status = PREFLOP;
        smallBlind();
        bigBlind();
        raise = BIG;
        deal();
        heroInfo(handCounter,0);
        int counter = button + 3;
        Action tempAction;

        while (allActionMade() == 0) {
            // Get the current player
            int currentPlayerIndex = (button + counter) % PLAYER;
            Player* currentPlayer = players[currentPlayerIndex];

            // Check if this player has raised the same amount as the last raise
            if (currentPlayer->getAction() == RAISE && currentPlayer->getRaise() == raise) {
                break; // All players have matched the raise
            }

            // Get the action for the current player
            tempAction = currentPlayer->makeAction(raise, pot, button);

            registerAction(currentPlayer->getAction(), raise, currentPlayerIndex, handCounter);

            if (tempAction == PASS) {
                cout << "Player " << currentPlayerIndex << " folded" << endl;
                counter++; // Move to the next player
            } else {
                // Handle RAISE action
                if (tempAction == RAISE) {
                    raise = currentPlayer->getRaise();
                    currentPlayer->changeStack(-raise);
                    cout << "Player " << currentPlayerIndex << " raised by " << raise << endl;
                    pot += raise;
                } 
                // Handle CALL action
                else if (tempAction == CALL) {
                    int callAmount = (raise > currentPlayer->getStack()) ? currentPlayer->getStack() : raise;
                    currentPlayer->changeStack(-callAmount);
                    pot += callAmount;
                    cout << "Player " << currentPlayerIndex << " called " << callAmount << endl;
                }
                counter++; // Move to the next player
            }
        }

        // Handle community cards (flop, turn, river)
        status = FLOP;
        flop();
        status = TURN;
        turn();
        status = RIVER;
        river();

        // Determine the winners and display them
        winners = defineWinner();
        cout << "The winners are players: ";
        for (int winner : winners) {
            cout << "Player " << winner << " ";
        }
        cout << endl;
        registerWin(winners,handCounter);
        // Distribute the pot
        int share = pot / winners.size();
        int remainder = pot % winners.size();

        for (size_t i = 0; i < winners.size(); ++i) {
            int winnings = share;
            if (i == 0) winnings += remainder; // The first winner gets the remainder
            players[winners[i]]->changeStack(winnings);
        }

        cout << "Pot (" << pot << ") has been distributed among the winners." << endl;
        displayPlayers();

        // Check if only one player has enough stack to continue
        int playerCounter = 0;
        for (int i = 0; i < PLAYER; i++) {
            if (players[i]->getStack() <= 20) {
                playerCounter++;
            }
        }

        // End the game if only one player has enough stack to continue
        if (playerCounter == PLAYER - 1) {
            cout << "Hand counter is: " << handCounter << endl;
            break;
        }

        passButton();



        handCounter++;
    }

    cout << "Game ended after " << handCounter << " hands." << endl;
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
        players[i]->display();
    }
}

void Table::displayPot(){
    cout << pot << endl;
}

vector<int> Table::defineHand(Player player) {
    vector<Card> cards(7);
    for (int i = 0; i < 5; i++) {
        cards[i] = publicCards[i];
    }
    cards[5] = player.returnCard(0);
    cards[6] = player.returnCard(1);

    std::sort(cards.begin(), cards.end(), [](const Card& a, const Card& b) {
        return a.getNumber() < b.getNumber();
    });

    // Variables to track the best hand
    vector<Card> bestHand;

    // Count colours
    int spades = 0, clubs = 0, hearts = 0, diamonds = 0;
    Colour colour;
    int flushColour;
    for (int i = 0; i < 7; i++) {
        colour = cards[i].getColour();
        if (colour == 0) spades++;
        else if (colour == 1) clubs++;
        else if (colour == 2) diamonds++;
        else hearts++;
    }

    bool isStraight = 0, isFlush = 0;
    int consecutive = 1;

    // Check flush
    if (spades >= 5) {
        isFlush = 1;
        flushColour = 0;
    } else if (clubs >= 5) {
        isFlush = 1;
        flushColour = 1;
    } else if (diamonds >= 5) {
        isFlush = 1;
        flushColour = 2;
    } else if (hearts >= 5) {
        isFlush = 1;
        flushColour = 3;
    }

    // Check straight
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

    bool isFourOfKind = false, isFullHouse = false, isThreeOfKind = false, isTwoPair = false, isOnePair = false;
    int counts[15] = {0}; // Index 2–14 used for card numbers

    for (int i = 0; i < 7; i++) {
        counts[cards[i].getNumber()]++;
    }

    int pairCount = 0;
    for (int i = 2; i <= 14; i++) {
        if (counts[i] == 4) isFourOfKind = true;
        if (counts[i] == 3) isThreeOfKind = true;
        if (counts[i] == 2) pairCount++;
    }

    if (isThreeOfKind && pairCount == 1) {
        isFullHouse = true;
    }

    if (pairCount >= 2) isTwoPair = true;
    if (pairCount == 1) isOnePair = true;

    bool isRoyal = 0, isStraightFlush = 0;

    if (isFlush) {
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

    //Handle Royal Flush and Straight Flush directly
    if (isRoyal) {
        for (const auto& card : cards) {
            if (card.getColour() == flushColour && card.getNumber() >= 10 && card.getNumber() <= 14) {
                bestHand.push_back(card);
            }
        }
    
        if (bestHand.size() != 5) return {0, 0, 0, 0, 0, 0}; // Royal Flush musi mieć 5 kart
    
        sort(bestHand.begin(), bestHand.end(), [](const Card& a, const Card& b) {
            return a.getNumber() < b.getNumber();
        });
    
        vector<int> result = {9}; // Royal Flush
        for (const auto& card : bestHand) result.push_back(card.getNumber());
    
        return result; // guaranteed to be size 6
    }
    
    
    else if (isStraightFlush) {
        vector<Card> straightFlushCards;
        for (const auto& card : cards) {
            if (card.getColour() == flushColour) {
                straightFlushCards.push_back(card);
            }
        }
    
        sort(straightFlushCards.begin(), straightFlushCards.end(), [](const Card& a, const Card& b) {
            return a.getNumber() < b.getNumber();
        });
    
        // Usuwamy duplikaty numerów (np. dwie 10-ki w tym samym kolorze)
        vector<Card> uniqueCards;
        for (const auto& card : straightFlushCards) {
            if (uniqueCards.empty() || uniqueCards.back().getNumber() != card.getNumber()) {
                uniqueCards.push_back(card);
            }
        }
    
        for (int i = 0; i <= uniqueCards.size() - 5; ++i) {
            bool isStraight = true;
            for (int j = i; j < i + 4; ++j) {
                if (uniqueCards[j].getNumber() + 1 != uniqueCards[j + 1].getNumber()) {
                    isStraight = false;
                    break;
                }
            }
    
            if (isStraight) {
                bestHand = vector<Card>(uniqueCards.begin() + i, uniqueCards.begin() + i + 5);
    
                vector<int> result = {8}; // Straight Flush
                for (const auto& card : bestHand) {
                    result.push_back(card.getNumber());
                }
                return result;
            }
        }
    }
    

    if (isFourOfKind) {
        int fourKindValue = -1;
        for (int i = 2; i <= 14; i++) {
            if (counts[i] == 4) {
                fourKindValue = i;
                break;
            }
        }
    
        for (const auto& card : cards) {
            if (card.getNumber() == fourKindValue) {
                bestHand.push_back(card);
            }
        }
    
        // Find highest kicker
        Card kicker;
        bool found = false;
        for (const auto& card : cards) {
            if (card.getNumber() != fourKindValue) {
                if (!found || card.getNumber() > kicker.getNumber()) {
                    kicker = card;
                    found = true;
                }
            }
        }
    
        if (found) {
            bestHand.push_back(kicker);
        }
    
        // Sort the four of a kind part
        sort(bestHand.begin(), bestHand.begin() + 4, [](const Card& a, const Card& b) {
            return a.getNumber() < b.getNumber();
        });
    
        vector<int> result = {7};
        for (const auto& card : bestHand) {
            result.push_back(card.getNumber());
        }
    
        // Fill with 0 if needed (shouldn't happen but just in case)
        while (result.size() < 6) {
            result.push_back(0);
        }
    
        return result;
    }
    
    if (isFullHouse) {
        vector<Card> threeOfAKind, pairCards;
        int threeVal = -1, pairVal = -1;
    
        for (int i = 14; i >= 2; i--) {
            if (counts[i] == 3 && threeVal == -1) {
                threeVal = i;
                for (const auto& card : cards) {
                    if (card.getNumber() == i) {
                        threeOfAKind.push_back(card);
                    }
                }
            } else if (counts[i] >= 2 && i != threeVal && pairVal == -1) {
                pairVal = i;
                for (const auto& card : cards) {
                    if (card.getNumber() == i && pairCards.size() < 2) {
                        pairCards.push_back(card);
                    }
                }
            }
        }
    
        if (threeOfAKind.size() == 3 && pairCards.size() == 2) {
            bestHand.insert(bestHand.end(), threeOfAKind.begin(), threeOfAKind.end());
            bestHand.insert(bestHand.end(), pairCards.begin(), pairCards.end());
    
            vector<int> result = {6};
            for (const auto& card : bestHand) {
                result.push_back(card.getNumber());
            }
    
            // Dla pewności dopełniamy do 6
            while (result.size() < 6) {
                result.push_back(0);
            }
    
            return result;
        }
    }
    
    
    else if (isFlush) {
        vector<Card> flushCards;
    
        // Group cards of the flush colour
        for (const auto& card : cards) {
            if (card.getColour() == flushColour) {
                flushCards.push_back(card);
            }
        }
    
        // If we have at least 5 cards of the same suit
        if (flushCards.size() >= 5) {
            // Sort cards in descending order
            sort(flushCards.begin(), flushCards.end(), [](const Card& a, const Card& b) {
                return a.getNumber() > b.getNumber();
            });
    
            // Select the 5 strongest cards
            bestHand = vector<Card>(flushCards.begin(), flushCards.begin() + 5);
    
            // Build result vector
            vector<int> result = {5};
            for (const auto& card : bestHand) {
                result.push_back(card.getNumber());
            }
    
            return result;
        }
    }
    
    else if (isStraight) {
        // Zbierz unikalne wartości kart
        set<int> uniqueValues;
        for (const auto& card : cards) {
            uniqueValues.insert(card.getNumber());
        }
    
        // Dodaj asa jako 1, jeśli występuje 14 (A), do obsługi A-2-3-4-5
        if (uniqueValues.count(14)) {
            uniqueValues.insert(1);
        }
    
        // Zamień na wektor i posortuj rosnąco
        vector<int> sortedValues(uniqueValues.begin(), uniqueValues.end());
    
        // Szukaj najlepszego straighta
        vector<int> bestStraight;
        for (int i = sortedValues.size() - 1; i >= 4; --i) {
            bool isStraightFound = true;
            for (int j = 0; j < 4; ++j) {
                if (sortedValues[i - j] - 1 != sortedValues[i - j - 1]) {
                    isStraightFound = false;
                    break;
                }
            }
            if (isStraightFound) {
                // Zbierz 5 wartości straighta w kolejności od najwyższej do najniższej
                for (int k = 0; k < 5; ++k) {
                    bestStraight.push_back(sortedValues[i - k]);
                }
                break;
            }
        }
    
        if (!bestStraight.empty()) {
            vector<int> result = {4};
            result.insert(result.end(), bestStraight.begin(), bestStraight.end());
            return result;
        }
    }
    
    
    if (isThreeOfKind) {
        int threeKindValue = -1;
        vector<Card> kickerCards;
        bestHand.clear();
    
        for (int i = 14; i >= 2; i--) {
            if (counts[i] == 3) {
                threeKindValue = i;
                for (const auto& card : cards) {
                    if (card.getNumber() == i) bestHand.push_back(card);
                }
                break;
            }
        }
    
        for (const auto& card : cards) {
            if (card.getNumber() != threeKindValue) {
                kickerCards.push_back(card);
            }
        }
    
        sort(kickerCards.begin(), kickerCards.end(), [](const Card& a, const Card& b) {
            return a.getNumber() > b.getNumber();
        });
    
        bestHand.push_back(kickerCards[0]);
        bestHand.push_back(kickerCards[1]);
    
        vector<int> result = {3};
        for (const auto& card : bestHand) result.push_back(card.getNumber());
        return result;
    }
    
    else if (isTwoPair) {
        vector<int> pairValues;
        bestHand.clear();
    
        for (int i = 14; i >= 2; i--) {
            if (counts[i] == 2) {
                pairValues.push_back(i);
            }
        }
    
        if (pairValues.size() >= 2) {
            for (int i = 0; i < 2; ++i) {
                for (const auto& card : cards) {
                    if (card.getNumber() == pairValues[i]) {
                        bestHand.push_back(card);
                    }
                }
            }
    
            // Add kicker
            vector<Card> kickers;
            for (const auto& card : cards) {
                if (card.getNumber() != pairValues[0] && card.getNumber() != pairValues[1]) {
                    kickers.push_back(card);
                }
            }
    
            sort(kickers.begin(), kickers.end(), [](const Card& a, const Card& b) {
                return a.getNumber() > b.getNumber();
            });
    
            bestHand.push_back(kickers[0]);
    
            vector<int> result = {2};
            for (const auto& card : bestHand) result.push_back(card.getNumber());
            return result;
        }
    }
    
    else if (isOnePair) {
        int pairValue = -1;
        bestHand.clear();
    
        for (int i = 14; i >= 2; i--) {
            if (counts[i] == 2) {
                pairValue = i;
                for (const auto& card : cards) {
                    if (card.getNumber() == i) bestHand.push_back(card);
                }
                break;
            }
        }
    
        vector<Card> kickers;
        for (const auto& card : cards) {
            if (card.getNumber() != pairValue) {
                kickers.push_back(card);
            }
        }
    
        sort(kickers.begin(), kickers.end(), [](const Card& a, const Card& b) {
            return a.getNumber() > b.getNumber();
        });
    
        bestHand.push_back(kickers[0]);
        bestHand.push_back(kickers[1]);
        bestHand.push_back(kickers[2]);
    
        vector<int> result = {1};
        for (const auto& card : bestHand) result.push_back(card.getNumber());
        return result;
    }
    
    // High card
    vector<Card> highCardHand = cards;
    sort(highCardHand.begin(), highCardHand.end(), [](const Card& a, const Card& b) {
        return a.getNumber() > b.getNumber();
    });
    highCardHand.resize(5);
    
    vector<int> result = {0};
    for (const auto& card : highCardHand) result.push_back(card.getNumber());
    return result;
    }

vector<int> Table::defineWinner() {
    vector<int> winners;
    int bestHandValue = -1;  // Najlepsza wartość ręki

    vector<vector<int>> handsResults;
    for (int i = 0; i < PLAYER; i++) {
        handsResults.push_back(defineHand(*players[i]));
    }

    cout << "Wyniki rąk graczy:" << endl;
    for (int i = 0; i < PLAYER; i++) {
        // cout << "Gracz " << i ;
        // if (players[i]->getAction() == PASS) {
        //      cout << " (FOLD)";
        // }
        // cout << ": ";
        // for (int card : handsResults[i]) {
        //     cout << card << " ";
        // }
        // cout << endl;
    }

    for (int i = 0; i < PLAYER; i++) {
        if (players[i]->getAction() == PASS) continue;  // Pomiń graczy, którzy spasowali

        vector<int> hand = handsResults[i];
        int handValue = hand[0];

        // cout << "Porównuję rękę gracza " << i << " (Typ: " << handValue << "): ";
        for (int card : hand) {
            // cout << card << " ";
        }
        cout << endl;

        if (winners.empty()) {
            bestHandValue = handValue;
            winners.push_back(i);
            // cout << "Nowy zwycięzca: Gracz " << i << endl;
        } else if (handValue > bestHandValue) {
            bestHandValue = handValue;
            winners.clear();
            winners.push_back(i);
            // cout << "Nowy zwycięzca: Gracz " << i << endl;
        } else if (handValue == bestHandValue) {
            bool isBetter = false;
            bool isEqual = true;
            for (int j = 1; j < hand.size(); j++) {
                if (hand[j] > handsResults[winners[0]][j]) {
                    isBetter = true;
                    isEqual = false;
                    break;
                } else if (hand[j] < handsResults[winners[0]][j]) {
                    isEqual = false;
                    break;
                }
            }

            if (isBetter) {
                winners.clear();
                winners.push_back(i);
                // cout << "Nowy zwycięzca: Gracz " << i << endl;
            } else if (isEqual) {
                winners.push_back(i); // Remis
                // cout << "Remis! Dodano gracza " << i << " do listy zwycięzców." << endl;
            }
        }
    }

    return winners;
}
   
void Table::createHeader(int handNumber){
    string fileName = "dataset/" + std::to_string(handNumber) + ".txt";
    ofstream plik(fileName);

    if (plik.is_open()) {
        plik << "Hand nr: " << handNumber << endl;
        plik << "Number of players: " << PLAYER << endl;
        plik << "Button on player: " << button << endl;
        //stack info
        for(int i=0; i<PLAYER;i++){
            plik << "Player " << i << " have " << players[i]->getStack() << " in his stack" << endl;
        }
        plik << "Current raise is:" << raise << endl;
        plik.close(); // zamykamy plik po zakończeniu operacji
    } else {
        std::cerr << "Nie udalo sie otworzyc pliku do zapisu.\n";
    }
}

void Table::registerAction(Action action, int raiseMoney, int playerNum, int handNumber){
    string fileName = "dataset/" + std::to_string(handNumber) + ".txt";
    ifstream inFile(fileName);
    stringstream buffer;

    if (inFile.is_open()) {
        buffer << inFile.rdbuf(); // read entire file into buffer
        inFile.close();
    } else {
        std::cerr << "Cannot open file to read.\n";
    }

    std::string content = buffer.str();

    // Now modify the content
    size_t pos = content.find("Hand nr: 3");
    if (pos != std::string::npos) {
        content.replace(pos, 10, "Hand nr: X");
    }

    
    ofstream outFile(fileName); // open again in write mode
    if (outFile.is_open()) {
        outFile << content;
        outFile << "Player " << playerNum;

        if(action==PASS)
            outFile << " pass" << endl;
        else if(action==CHECK)
            outFile << " check" << endl;
        else if(action==CALL)
            outFile << " call" << endl;
        else if(action==RAISE)
            outFile << " raised to " << raise << endl;
        outFile.close();
    } else {
        std::cerr << "Cannot open file to write.\n";
    }
}

void Table::registerWin(vector<int>& winners, int handNumber){
    string fileName = "dataset/" + std::to_string(handNumber) + ".txt";
    ifstream inFile(fileName);
    stringstream buffer;

    if (inFile.is_open()) {
        buffer << inFile.rdbuf(); // read entire file into buffer
        inFile.close();
    } else {
        std::cerr << "Cannot open file to read.\n";
    }

    std::string content = buffer.str();

    // Now modify the content
    size_t pos = content.find("Hand nr: 3");
    if (pos != std::string::npos) {
        content.replace(pos, 10, "Hand nr: X");
    }

    
    ofstream outFile(fileName); // open again in write mode
    if (outFile.is_open()) {
        outFile << content;
        if(winners.size()==1){
            outFile << "The winner is: " << winners[0] << endl;
        }
        else{
            outFile << "The winner are: ";
            for(int i=0; i< winners.size(); i++){
                outFile << winners[i] << " ";
            }
            outFile << endl;
            
        }   
        outFile << "The pot was: " << pot << endl;
        outFile.close();
    } else {
        std::cerr << "Cannot open file to write.\n";
    }
}

void Table::heroInfo(int handNumber, int playerNumber){
    string fileName = "dataset/" + std::to_string(handNumber) + ".txt";

    Card tempHand[2];
    tempHand[0] = players[playerNumber]->returnCard(0);
    tempHand[1] = players[playerNumber]->returnCard(1);

    ifstream inFile(fileName);
    stringstream buffer;

    if (inFile.is_open()) {
        buffer << inFile.rdbuf(); // read entire file into buffer
        inFile.close();
    } else {
        std::cerr << "Cannot open file to read.\n";
    }

    std::string content = buffer.str();

    // Now modify the content
    size_t pos = content.find("Hand nr: 3");
    if (pos != std::string::npos) {
        content.replace(pos, 10, "Hand nr: X");
    }

    
    ofstream outFile(fileName); // open again in write mode
    if (outFile.is_open()) {
        outFile << content;   
        outFile << "Hero is player: " << playerNumber << " his hand is: " << endl;
        for(int i=0; i<2;i++){
            outFile << "Card " << i << ": "; //<< tempHand[i].getNumber() << " " << tempHand[i].getColour() << endl;
            if(tempHand[i].getNumber()==11)
                outFile << "J ";
            else if(tempHand[i].getNumber()==12)
                outFile << "Q ";
            else if(tempHand[i].getNumber()==13)
                outFile << "K ";
            else if(tempHand[i].getNumber()==14)
                outFile << "A ";   
            else 
                outFile <<  tempHand[i].getNumber() << " ";
            if(tempHand[i].getColour()==0)
                outFile << "Spade"; 
            else if(tempHand[i].getColour()==1)
                outFile << "Clubs";
            else if(tempHand[i].getColour()==2)
                outFile << "Diamond";
            else
                outFile << "Heart"; 
            outFile << endl;
        }
        outFile.close();
    } else {
        std::cerr << "Cannot open file to write.\n";
    }
}
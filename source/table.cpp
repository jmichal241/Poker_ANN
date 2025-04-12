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
        vector<int> winners;

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
        displayBoard();
        for(int i=0; i<PLAYER;i++){
            players[i].display();
        }
        winners = defineWinner();
        std::cout << "Zwycięzcy to gracze: ";
        for (int winner : winners) {
            std::cout << "Gracz " << winner << " ";
        }
        std::cout << std::endl;

        passButton();
        for(int i=0; i<PLAYER; i++){
            players[i].resetHand();
        }
    break;
    }
}
// for(int i=0;i<7;i++){
//     cards[i].display();
// }
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
        int bestHandValue = -1;  // Najlepsza wartość ręki (np. 1 = para, 2 = trójka, itd.)
    
        vector<vector<int>> handsResults;
        for (int i = 0; i < PLAYER; i++) {
            handsResults.push_back(defineHand(players[i]));  // Zapisz wyniki rąk graczy
        }
    
        // Wyświetl wyniki rąk graczy (ułatwienie debugowania)
        cout << "Wyniki rąk graczy:" << endl;
        for (int i = 0; i < PLAYER; i++) {
            cout << "Gracz " << i + 1 << ": ";
            for (int card : handsResults[i]) {
                cout << card << " ";
            }
            cout << endl;
        }
    
        // Porównanie rąk graczy
        for (int i = 0; i < PLAYER; i++) {
            vector<int> hand = handsResults[i];
            int handValue = hand[0];  // Pierwsza wartość to ocena typu ręki (np. 1 = para, 2 = trójka, itd.)
    
            // Wyświetlanie porównania dla debugowania
            cout << "Porównuję rękę gracza " << i  << " (Typ: " << handValue << "): ";
            for (int card : hand) {
                cout << card << " ";
            }
            cout << endl;
    
            if (handValue > bestHandValue) {
                bestHandValue = handValue;
                winners.clear();  // Wyczyść poprzednich zwycięzców
                winners.push_back(i);  // Dodaj nowego zwycięzcę
                cout << "Nowy zwycięzca: Gracz " << i + 1 << endl;
            } else if (handValue == bestHandValue) {
                // Jeśli ręka jest taka sama jak najlepsza, porównaj poszczególne karty
                bool isWinner = false;
                for (int j = 1; j < hand.size(); j++) {  // Zaczynaj porównywać od drugiego elementu (kart)
                    if (handsResults[i][j] > handsResults[winners[0]][j]) {
                        isWinner = true;
                        break;
                    } else if (handsResults[i][j] < handsResults[winners[0]][j]) {
                        break;
                    }
                }
    
                if (isWinner) {
                    winners.clear();  // Wyczyszczenie poprzednich zwycięzców, ponieważ mamy nowego
                    winners.push_back(i);
                    cout << "Nowy zwycięzca: Gracz " << i  << " (lepszy kicker)" << endl;
                } else if (handsResults[i] == handsResults[winners[0]]) {
                    winners.push_back(i);  // Dodaj do zwycięzców, jeśli są równi
                    cout << "Remis z Graczem " << winners[0]  << ": Gracz " << i  << " też wygrywa!" << endl;
                }
            }
        }
    
        // Wyświetl zwycięzców
        cout << "Zwycięzcy: ";
        for (int winner : winners) {
            cout << "Gracz " << winner << " ";
        }
        cout << endl;
    
        return winners;  // Zwróć wektor zwycięzców
    }
    
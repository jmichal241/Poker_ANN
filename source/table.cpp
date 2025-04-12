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
    int counter2=0;
    while(1){
        counter2++;
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
        // for(int i=0;i<PLAYER;i++){
        //     if(defineHand(players[i])==6)
        //         break;
        // }
        // defineHand(players[0]);
        if(defineHand(players[0])==9){
            players[0].display();
            cout << counter2 << endl;
            break;
        }
        passButton();
        for(int i=0; i<PLAYER; i++){
            players[i].resetHand();
        }
    // break;
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
int Table::defineHand(Player player) {
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

    // Handle Royal Flush and Straight Flush directly
    if (isRoyal) {
        for (const auto& card : cards) {
            if (card.getColour() == flushColour && (card.getNumber() >= 10 && card.getNumber() <= 14)) {
                bestHand.push_back(card);
            }
        }
        sort(bestHand.begin(), bestHand.end(), [](const Card& a, const Card& b) {
            return a.getNumber() < b.getNumber();
        });

        return 9; // Royal Flush
    } else if (isStraightFlush) {
        vector<Card> straightFlushCards;
        for (const auto& card : cards) {
            if (card.getColour() == flushColour) {
                straightFlushCards.push_back(card);
            }
        }
        sort(straightFlushCards.begin(), straightFlushCards.end(), [](const Card& a, const Card& b) {
            return a.getNumber() < b.getNumber();
        });

        // Check for a consecutive sequence (straight flush)
        for (int i = 0; i <= straightFlushCards.size() - 5; ++i) {
            bool isStraight = true;
            for (int j = i; j < i + 4; ++j) {
                if (straightFlushCards[j].getNumber() != straightFlushCards[j + 1].getNumber() - 1) {
                    isStraight = false;
                    break;
                }
            }
            if (isStraight) {
                bestHand = vector<Card>(straightFlushCards.begin() + i, straightFlushCards.begin() + i + 5);
                return 8; // Straight Flush
            }
        }
    }

    // Handle other hand types and select the best 5 cards
    if (isFourOfKind) {
        // Find the value of the "Four of a Kind"
        int fourKindValue = -1;
        for (int i = 2; i <= 14; i++) {
            if (counts[i] == 4) {
                fourKindValue = i;
                break;
            }
        }
    
        // Add the four of a kind cards to bestHand
        for (const auto& card : cards) {
            if (card.getNumber() == fourKindValue) {
                bestHand.push_back(card);
            }
        }
    
        // Find the strongest remaining card (the one that is not part of the four of a kind)
        Card strongestRemainingCard;
        bool foundStrongerCard = false;
    
        for (const auto& card : cards) {
            if (card.getNumber() != fourKindValue) {
                if (!foundStrongerCard || card.getNumber() > strongestRemainingCard.getNumber()) {
                    strongestRemainingCard = card;
                    foundStrongerCard = true;
                }
            }
        }
    
        // Add the strongest remaining card to the end of the best hand
        if (foundStrongerCard) {
            bestHand.push_back(strongestRemainingCard);
        }
    
        // Sort the first 4 cards to arrange them in order (excluding the extra card)
        sort(bestHand.begin(), bestHand.end() - 1, [](const Card& a, const Card& b) {
            return a.getNumber() < b.getNumber();
        });
    
        return 7; // Four of a Kind
    }

    if (isFullHouse) {
        vector<Card> threeOfAKind, pairCards;
        for (int i = 2; i <= 14; i++) {
            if (counts[i] == 3) {
                for (const auto& card : cards) {
                    if (card.getNumber() == i) {
                        threeOfAKind.push_back(card);
                    }
                }
            }
            if (counts[i] == 2) {
                for (const auto& card : cards) {
                    if (card.getNumber() == i) {
                        pairCards.push_back(card);
                    }
                }
            }
        }
        
        // Ensure we have 3 cards in three of a kind and 2 in pair
        if (threeOfAKind.size() == 3 && pairCards.size() == 2) {
            bestHand.insert(bestHand.end(), threeOfAKind.begin(), threeOfAKind.end());
            bestHand.insert(bestHand.end(), pairCards.begin(), pairCards.end());
            return 6; // Full House
        }
    }
    
    else if (isFlush) {
        vector<Card> flushCards;
        
        // Group cards of the same suit
        for (const auto& card : cards) {
            if (card.getColour() == cards[0].getColour()) {
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
    
            return 5; // Flush
        }
    }
    
    else if (isStraight) {
        vector<Card> straightCards;
    
        // Gather unique card numbers
        set<int> uniqueNumbers;
        for (const auto& card : cards) {
            uniqueNumbers.insert(card.getNumber());
        }
    
        // Convert set to vector
        for (int num : uniqueNumbers) {
            straightCards.push_back(Card(num, cards[0].getColour()));
        }
    
        // If we have less than 5 cards in straightCards, we cannot form a straight
        if (straightCards.size() < 5) {
            return 0; // No straight
        }
    
        // Sort cards in ascending order
        sort(straightCards.begin(), straightCards.end(), [](const Card& a, const Card& b) {
            return a.getNumber() < b.getNumber();
        });
    
        vector<Card> bestStraight;
    
        // Check for a valid straight
        for (int i = 0; i <= straightCards.size() - 5; ++i) {
            bool validStraight = true;
            for (int j = i; j < i + 4; ++j) {
                if (straightCards[j].getNumber() != straightCards[j + 1].getNumber() - 1) {
                    validStraight = false;
                    break;
                }
            }
            if (validStraight) {
                bestStraight = vector<Card>(straightCards.begin() + i, straightCards.begin() + i + 5);
                break;
            }
        }
    
        return 4; // Straight
    }
    else if (isThreeOfKind) {
        vector<Card> kickerCards;
    
        // Find the three-of-a-kind
        for (int i = 2; i <= 14; i++) {
            if (counts[i] == 3) {
                for (const auto& card : cards) {
                    if (card.getNumber() == i) {
                        bestHand.push_back(card); // Add cards to the three-of-a-kind
                    }
                }
                break; // We found the three-of-a-kind, so exit the loop
            }
        }
    
        // Add the kickers (cards that are not part of the three-of-a-kind)
        for (const auto& card : cards) {
            bool isInThreeOfKind = false;
            // Check if the card is part of the three-of-a-kind
            for (const auto& threeCard : bestHand) {
                if (card.getNumber() == threeCard.getNumber() && card.getColour() == threeCard.getColour()) {
                    isInThreeOfKind = true;
                    break;
                }
            }
    
            if (!isInThreeOfKind) {
                kickerCards.push_back(card);  // Add cards that are not part of the three-of-a-kind
            }
        }
    
        // Sort kickers in descending order (highest first)
        sort(kickerCards.begin(), kickerCards.end(), [](const Card& a, const Card& b) {
            return a.getNumber() > b.getNumber();
        });
    
        // Add the two best kickers (if available)
        bestHand.push_back(kickerCards[0]);
        bestHand.push_back(kickerCards[1]);
    
        return 3; // Three of a Kind
    }
    
    else if (isTwoPair) {
        vector<Card> pairCards;
        vector<Card> kickerCards;
        vector<int> pairValues; // Store the values of pairs to choose the top 2 pairs
    
        // Find two pairs
        for (int i = 2; i <= 14; i++) {
            if (counts[i] == 2) {
                pairValues.push_back(i);  // Add pair value
                for (const auto& card : cards) {
                    if (card.getNumber() == i) {
                        pairCards.push_back(card);  // Add cards to the pair
                    }
                }
            }
        }
    
        // Sort pairs by value (highest pair first)
        sort(pairValues.begin(), pairValues.end(), greater<int>());
    
        // Select the two best pairs
        vector<Card> bestPairs;
        for (int i = 0; i < 2; i++) {
            for (const auto& card : cards) {
                if (card.getNumber() == pairValues[i]) {
                    bestPairs.push_back(card);
                }
            }
        }
    
        // Add the two best pairs to the hand
        bestHand.insert(bestHand.end(), bestPairs.begin(), bestPairs.end());
    
        // Add kickers (cards not part of the pairs)
        for (const auto& card : cards) {
            bool isInPair = false;
            // Check if the card is part of the best pairs
            for (const auto& pairCard : bestPairs) {
                if (card.getNumber() == pairCard.getNumber() && card.getColour() == pairCard.getColour()) {
                    isInPair = true;
                    break;
                }
            }
    
            if (!isInPair) {
                kickerCards.push_back(card);  // Add cards that are not part of the pairs
            }
        }
    
        // Sort kickers in descending order (highest first)
        sort(kickerCards.begin(), kickerCards.end(), [](const Card& a, const Card& b) {
            return a.getNumber() > b.getNumber();
        });
    
        // Add the best kicker (if available)
        bestHand.push_back(kickerCards[0]);
    
        return 2; // Two Pair
    }
    
    else if (isOnePair) {
        vector<Card> pairCards;
        vector<Card> kickerCards;
        int pairValue = -1;  // Variable to store the pair value
    
        // Find the pair value
        for (int i = 2; i <= 14; i++) {
            if (counts[i] == 2) {
                pairValue = i;  // Store pair value
                for (const auto& card : cards) {
                    if (card.getNumber() == i) {
                        pairCards.push_back(card);  // Add cards that form the pair
                    }
                }
            }
        }
    
        // Add kicker cards (all cards that are not part of the pair)
        for (const auto& card : cards) {
            if (card.getNumber() != pairValue) {
                kickerCards.push_back(card);  // Add cards that are not part of the pair
            }
        }
    
        // Add the pair as the first two cards
        bestHand.insert(bestHand.end(), pairCards.begin(), pairCards.end());
    
        // Sort kickers in descending order (highest first)
        sort(kickerCards.begin(), kickerCards.end(), [](const Card& a, const Card& b) {
            return a.getNumber() > b.getNumber();
        });
    
        // Add the three best kickers
        bestHand.insert(bestHand.end(), kickerCards.begin(), kickerCards.begin() + 3);
    
        return 1; // One Pair
    }
    
    // High card case
    vector<Card> highCardHand;
    for (const auto& card : cards) {
        highCardHand.push_back(card);
    }
    sort(highCardHand.begin(), highCardHand.end(), [](const Card& a, const Card& b) {
        return a.getNumber() > b.getNumber();  // Sort in descending order
    });
    highCardHand.resize(5); // Only the top 5 cards
    
    return 0; // High Card
    }
    

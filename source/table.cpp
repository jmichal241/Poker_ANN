#include "../header/table.h"
#define STACK_SIZE 1000
#define PLAYER 5
#define SMALL 5
#define BIG 10
#define MAXHANDS 1000

// In Table class constructor (table.cpp)
Table::Table() {
    players.push_back(new Lukibot(STACK_SIZE));  // Use pointer to store Kloziobot
    players[0]->resetHand();  // Reset hand for Kloziobot
    players.push_back(new GPT1bot(STACK_SIZE));  // Add another Kloziobot as the second player
    players[1]->resetHand();  // Reset hand for the second Kloziobot
    players.push_back(new GPT2bot(STACK_SIZE));  // Add another Kloziobot as the third player
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
        players[i]->resetInvestment();
        players[i]->setStack(STACK_SIZE);

    }
    pot=0;
    sidePotAmounts.clear();
    sidePotEligible.clear();
    for(int i=0; i<5; i++){
        publicCards[i].setNumber(0);
    }
}

void Table::resetHand(){
    for(int i=0; i<PLAYER; i++){
        players[i]->resetHand();
        players[i]->resetInvestment();
        players[i]->setAction(NONE);
    }
    pot = 0;
    sidePotAmounts.clear();
    sidePotEligible.clear();
    for(int i=0; i<5; i++)
        publicCards[i].setNumber(0);
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

    if (players[temp]->getStack() > 0) {
        int blindAmount = min(SMALL, players[temp]->getStack());
        players[temp]->changeStack(-blindAmount);
        players[temp]->changePotAgency(blindAmount);
        players[temp]->addInvestment(blindAmount);
        pot += blindAmount;
    }
}

void Table::bigBlind() {
    int temp = (button + 2) % PLAYER;


    if (players[temp]->getStack() > 0) {
        int blindAmount = min(BIG, players[temp]->getStack());
        players[temp]->changeStack(-blindAmount);
        players[temp]->changePotAgency(blindAmount);
        players[temp]->addInvestment(blindAmount);
        pot += blindAmount;
    }
}

void Table::passButton(){
    button = (button + 1) % PLAYER;
    for(int i=0; i<PLAYER; i++){
        players[i]->changeButton(0);
    }
    players[button]->changeButton(1);
}

void Table::prepareTable(){
    for (int i = 0; i < PLAYER; i++) {
        if (players[i]->getStack() == 0) {
            players[i]->setAction(PASS);  // hm
        } else {
            players[i]->setAction(NONE);
        }
    }
    smallBlind();
    bigBlind();
    raise = BIG;
    deal();
}

int Table::allActionMade() {
    int activePlayers = 0;
    int completedActions = 0;

    for (int i = 0; i < PLAYER; i++) {
        if (players[i]->getStack() > 0 && players[i]->getAction() != PASS) {
            activePlayers++;
            if (players[i]->getAction() == ALL_IN || players[i]->getPotAgency() >= raise){
                completedActions++;
            }
        }
    }

    // Sprawdź opcję dla big blinda
    int bigBlindIndex = (button + 2) % PLAYER;
    Player* bigBlind = players[bigBlindIndex];

    bool bigBlindShouldAct = (
        raise == BIG &&
        bigBlind->getStack() > 0 &&
        bigBlind->getAction() == CALL &&  // tylko call
        bigBlind->getPotAgency() == BIG   // zapłacił tylko big blinda
    );

    if (bigBlindShouldAct) return 0;

    return completedActions == activePlayers ? 1 : 0;
}

// vector<int> Table::getEligiblePlayers() {
//     vector<int> eligible;
//     for (int i = 0; i < PLAYER; ++i) {
//         if (players[i]->getAction() != PASS && players[i]->getInvestment() > 0 && players[i]->getStack() > 0) {
//             eligible.push_back(i);
//         }
//     }
//     return eligible;
// }

void Table::GameLoop() {
    resetboard();
    int handCounter = 0;
    std::random_device rd;  // Uzyskujemy ziarno
    std::mt19937 gen(rd()); // Generator Mersenne Twister
    std::uniform_int_distribution<> distrib(1, 100); // Przedział od 1 do 100

    // Losowanie liczby z przedziału
    int random_number;

    while (true) {
        resetHand();
        bool someoneWentAllIn = false; // for side pots
        sidePotAmounts.clear();
        sidePotEligible.clear();
        if (handCounter%500==0){
            resetboard();
        }
        vector<int> winners;
        pot = 0;
        random_number = distrib(gen)%5;
        // End the game after x hands

        if (handCounter == MAXHANDS){
            break;
        }
        raise = BIG;
        createHeader(handCounter, random_number);

        status = PREFLOP;
        // Reset hands for all players
        prepareTable();
        heroInfo(handCounter,random_number);
        int counter = button + 3;
        Action tempAction;

        while (allActionMade() == 0) {
            int currentPlayerIndex = (counter) % PLAYER;
            Player* currentPlayer = players[currentPlayerIndex];

            if (currentPlayer->getAction() == PASS || currentPlayer->getStack() == 0) {
                counter++;
                continue;
            }

            tempAction = currentPlayer->makeAction(raise, pot, button);

            if (tempAction == RAISE) {
                int newRaise = currentPlayer->getRaise();
                int toPay = newRaise - currentPlayer->getPotAgency();

                // you have to have enough money and cant raise by less than BIG
                if (newRaise >= raise + BIG && currentPlayer->getStack() >= toPay) {
                    // valid raise
                    raise = newRaise;
                    currentPlayer->changeStack(-toPay);
                    pot += toPay;
                    currentPlayer->changePotAgency(raise);
                    currentPlayer->setAction(RAISE);
                    currentPlayer->addInvestment(toPay);
                    cout << "Player " << currentPlayerIndex << " raised to " << raise << endl;
                } else if (currentPlayer->getStack() >= (raise - currentPlayer->getPotAgency())) {
                    // treat as CALL if raise too small but player has enough to call
                    tempAction = CALL;
                } else {
                    //if not enough money for call then all-in
                    tempAction = ALL_IN;
                }
            }

            // CALL
            if (tempAction == CALL) {
                int callAmount = raise - currentPlayer->getPotAgency();
                if (currentPlayer->getStack() >= callAmount) {
                    currentPlayer->changeStack(-callAmount);
                    pot += callAmount;
                    currentPlayer->changePotAgency(raise);
                    currentPlayer->setAction(CALL);
                    currentPlayer->addInvestment(callAmount);
                    cout << "Player " << currentPlayerIndex << " called " << raise << endl;
                } else { //if not enough money we all in
                    tempAction = ALL_IN;
                }
            }

            if (tempAction == ALL_IN) {
                if (currentPlayer->getStack() > 0) {
                    int allInAmount = currentPlayer->getStack();
                    int totalInvestment = currentPlayer->getPotAgency() + allInAmount;

                    pot += allInAmount;
                    currentPlayer->changePotAgency(totalInvestment);
                    currentPlayer->addInvestment(allInAmount);
                    currentPlayer->changeStack(-allInAmount);

                    if (totalInvestment > raise) {
                        raise = totalInvestment;
                    }

                    currentPlayer->setAction(ALL_IN);
                    someoneWentAllIn = true;

                    cout << "Player " << currentPlayerIndex << " goes ALL-IN with " << allInAmount << endl;
                }
                else { // if not enough money we pass
                    tempAction = PASS;
                }
            }

            // PASS
            if (tempAction == PASS) {
                currentPlayer->setAction(PASS);
                cout << "Player " << currentPlayerIndex << " folded" << endl;
            }

            counter++;
            registerAction(currentPlayer->getAction(), raise, currentPlayerIndex, handCounter);
        }

        int originalPot = pot;
    if (someoneWentAllIn) {
    // 1) Collect each player's total investment and build a sorted list of unique bet levels
    std::set<int> uniqueLevels;
    for (int i = 0; i < PLAYER; ++i) {
        if (players[i]->getAction() != PASS)
            uniqueLevels.insert(players[i]->getInvestment());
    }
    std::vector<int> levels(uniqueLevels.begin(), uniqueLevels.end());
    std::sort(levels.begin(), levels.end());

    int prevLevel = 0;
    int mainLevel = levels[0];  // The smallest investment becomes the main‐pot cutoff
    int mainPotAmt = 0;         // // Accumulate total money in the main pot
    std::vector<int> mainContrib;   // // list of players eligible for the main pot

        // 2) Calculate the main pot: for each player who invested at least mainLevel,
        //    they contribute (mainLevel - prevLevel) chips to the main pot.
    for (int i = 0; i < PLAYER; ++i) {
        if (players[i]->getAction() != PASS) {
            int inv = players[i]->getInvestment();
            if (inv >= mainLevel) {
                mainPotAmt += (mainLevel - prevLevel);
                mainContrib.push_back(i);
            }
        }
    }
    prevLevel = mainLevel;

        // 3) Build side pots for each higher investment level
        sidePotEligible.clear();    // Reset previous data
        sidePotAmounts.clear();
    for (size_t lvlIdx = 1; lvlIdx < levels.size(); ++lvlIdx) {
        int lvl = levels[lvlIdx];
        int sideAmt = 0;
        std::vector<int> contrib;       // Players eligible for this side pot
        for (int i = 0; i < PLAYER; ++i) {
            if (players[i]->getAction() != PASS && players[i]->getInvestment() >= lvl) {
                sideAmt += (lvl - prevLevel);
                contrib.push_back(i);
            }
        }
        prevLevel = lvl;
        if (!contrib.empty()) {
            sidePotEligible.push_back(contrib);
            sidePotAmounts.push_back(sideAmt);
        }
    }

    // Verify that main + all side pots sum to the original pot
    {
        int totalSide = std::accumulate(sidePotAmounts.begin(), sidePotAmounts.end(), 0);
        if (mainPotAmt + totalSide != originalPot) {
            std::cerr << "!!! POT CALC MISMATCH !!! original=" << originalPot
                      << " main=" << mainPotAmt
                      << " sideSum=" << totalSide << std::endl;
        }
    }

    // 5) Pay main pot
    {
        auto winners = defineWinnerAmong(mainContrib);
        int share = mainPotAmt / winners.size();
        int rem   = mainPotAmt % winners.size();
        for (size_t j = 0; j < winners.size(); ++j) {
            int win = share + (j == 0 ? rem : 0);
            players[winners[j]]->changeStack(win);
            std::cout << "Player " << winners[j]
                      << " wins " << win << " from MAIN pot\n";
        }
    }

    // 6) pay side pot
    for (size_t sp = 0; sp < sidePotAmounts.size(); ++sp) {
        auto winners = defineWinnerAmong(sidePotEligible[sp]);
        int sideAmt = sidePotAmounts[sp];
        int share   = sideAmt / winners.size();
        int rem     = sideAmt % winners.size();
        for (size_t j = 0; j < winners.size(); ++j) {
            int win = share + (j == 0 ? rem : 0);
            players[winners[j]]->changeStack(win);
                std::cout << "Player " << winners[j]
                      << " wins " << win << " from SIDE pot " << sp << "\n";
            }
        }
    } else {
        // 1) Collect everyone still in (non-folded who put chips in)
        std::vector<int> eligible;
        for (int i = 0; i < PLAYER; ++i) {
            if (players[i]->getAction() != PASS
                && players[i]->getInvestment() > 0) {
                eligible.push_back(i);
                }
        }

        // 2) If nobody is “eligible”, that means everyone except one folded (prevents crash when everybody folds)
        if (eligible.empty()) {
            // find the last non-folded player
            for (int i = 0; i < PLAYER; ++i) {
                if (players[i]->getAction() != PASS) {
                    players[i]->changeStack(pot);
                    std::cout << "Player " << i
                              << " wins " << pot
                              << " (everyone else folded)\n";
                    break;
                }
            }
        }
        // 3) Otherwise do a normal showdown split
        else {
            auto winners = defineWinnerAmong(eligible);
            int share = pot / static_cast<int>(winners.size());
            int rem   = pot % static_cast<int>(winners.size());
            for (size_t j = 0; j < winners.size(); ++j) {
                int win = share + (j == 0 ? rem : 0);
                players[winners[j]]->changeStack(win);
                std::cout << "Player " << winners[j]
                          << " wins " << win
                          << " from pot\n";
            }
        }
    }

        registerWin(winners, handCounter);

        cout << "Pot (" << pot << ") has been distributed among the winners." << endl;
        pot = 0;
        displayPlayers();
        displayBoard();

        int playerCounter = 0;
        for (int i = 0; i < PLAYER; i++) {
            if (players[i]->getStack() <= 20) {
                playerCounter++;
            }
        }

        if (playerCounter == PLAYER - 1) {
            cout << "Hand counter is: " << handCounter << endl;
            break;
        }

        passButton();
        handCounter++;
    }

    cout << "Game ended after " << handCounter << " hands." << endl;
}


vector<int> Table::defineWinnerAmong(const vector<int>& eligible) {
    vector<int> winners;
    int bestHandValue = -1;
    vector<vector<int>> handsResults(PLAYER);

    for (int i : eligible) {
        handsResults[i] = defineHand(*players[i]);
    }

    for (int i : eligible) {
        vector<int> hand = handsResults[i];
        int value = hand[0];

        if (winners.empty()) {
            bestHandValue = value;
            winners.push_back(i);
        } else if (value > bestHandValue) {
            bestHandValue = value;
            winners = {i};
        } else if (value == bestHandValue) {
            for (int j = 1; j < hand.size(); j++) {
                if (hand[j] > handsResults[winners[0]][j]) {
                    winners = {i};
                    break;
                } else if (hand[j] < handsResults[winners[0]][j]) {
                    break;
                } else if (j == hand.size() - 1) {
                    winners.push_back(i);  // tie
                }
            }
        }
    }

    return winners;
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

   
void Table::createHeader(int handNumber, int playerNum){
    string fileName;  

    if(handNumber<MAXHANDS*0.8)
        fileName = "dataset/train/" + std::to_string(handNumber) + ".json";
    else if (handNumber<MAXHANDS*0.9)
        fileName = "dataset/valid/" + std::to_string(handNumber) + ".json";
    else if (handNumber<MAXHANDS)
        fileName = "dataset/test/" + std::to_string(handNumber) + ".json";

    
    ofstream plik(fileName);
    playerNum=playerNum%PLAYER;
    if (plik.is_open()) {
        plik << "{" << endl;
        plik << "\"Number of players\": " << PLAYER << ", " << endl;
        plik << "\"Button on player\": " << button << ", " << endl;
        // plik << "\"Hero on spot\": " << playerNum << ", " << endl;
        plik << "\"BB is\": " << BIG << ", " << endl;
        plik.close(); // zamykamy plik po zakończeniu operacji
    } else {
        std::cerr << "Nie udalo sie otworzyc pliku do zapisu.\n";
    }
}

void Table::registerAction(Action action, int raiseMoney, int playerNum, int handNumber){
    string fileName;  

    if(handNumber<MAXHANDS*0.8)
        fileName = "dataset/train/" + std::to_string(handNumber) + ".json";
    else if (handNumber<MAXHANDS*0.9)
        fileName = "dataset/valid/" + std::to_string(handNumber) + ".json";
    else if (handNumber<MAXHANDS)
        fileName = "dataset/test/" + std::to_string(handNumber) + ".json";
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
        outFile << "{" << endl;
        outFile << "\"player\": " << playerNum << ", " << endl;
        outFile << "\"action\": ";
        if(action==PASS)
            outFile << "\"pass\"" << endl;
        else if(action==CHECK)
            outFile << "\"check\"" << endl;
        else if(action==CALL)
            outFile << "\"call\"" << endl;
        else if(action==RAISE)
            outFile << "\"raise\"" << ", " << endl << "\"amount\": " << raiseMoney << endl;
        outFile << "}," << endl;
        outFile.close();
    } else {
        std::cerr << "Cannot open file to write.\n";
    }
}

void Table::registerWin(vector<int>& winners, int handNumber){
    string fileName;  

    if(handNumber<MAXHANDS*0.8)
        fileName = "dataset/train/" + std::to_string(handNumber) + ".json";
    else if (handNumber<MAXHANDS*0.9)
        fileName = "dataset/valid/" + std::to_string(handNumber) + ".json";
    else if (handNumber<MAXHANDS)
        fileName = "dataset/test/" + std::to_string(handNumber) + ".json";
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
    content.pop_back();
    content.pop_back();
    ofstream outFile(fileName); // open again in write mode
    if (outFile.is_open()) {
        outFile << content;
        outFile << endl;
        outFile << "], " << endl;
        if(winners.size()==1){
            outFile << "\"winner\": [" << winners[0] << "], " << endl;
        }
        else{
            outFile << "\"winner\": [";
            for(int i=0; i< winners.size(); i++){
                if(i==winners.size()-1)
                    outFile << winners[i] << "]";
                else
                    outFile << winners[i] << ", ";
            }
            outFile << ", " << endl;
            
        }   
        outFile << "\"pot\": " << pot << endl;
        outFile << "}";
        outFile.close();
    } else {
        std::cerr << "Cannot open file to write.\n";
    }
}

void Table::heroInfo(int handNumber, int playerNumber){
    string fileName;  

    if(handNumber<MAXHANDS*0.8)
        fileName = "dataset/train/" + std::to_string(handNumber) + ".json";
    else if (handNumber<MAXHANDS*0.9)
        fileName = "dataset/valid/" + std::to_string(handNumber) + ".json";
    else if (handNumber<MAXHANDS)
        fileName = "dataset/test/" + std::to_string(handNumber) + ".json";

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

    ofstream outFile(fileName); // open again in write mode
    if (outFile.is_open()) {
        outFile << content;   
        outFile << "\"Hero_info\": " << "{" << endl << "\"player\": " << playerNumber << ", "<< endl;
        outFile << "\"cards\": " << "[" << endl;
        for(int i=0; i<2;i++){
            outFile << "{" << "\"card\": " << tempHand[i].getNumber() << ", " << "\"suit\": ";
            // if(tempHand[i].getNumber()==11)
            //     outFile << "J ";
            // else if(tempHand[i].getNumber()==12)
            //     outFile << "Q ";
            // else if(tempHand[i].getNumber()==13)
            //     outFile << "K ";
            // else if(tempHand[i].getNumber()==14)
            //     outFile << "A ";   
            // else 
            //     outFile <<  tempHand[i].getNumber() << " ";
            if(tempHand[i].getColour()==0)
                outFile << "\"Spade\""; 
            else if(tempHand[i].getColour()==1)
                outFile << "\"Clubs\"";
            else if(tempHand[i].getColour()==2)
                outFile << "\"Diamond\"";
            else
                outFile << "\"Heart\""; 
            outFile << "}";
            if(i==0)
                outFile << ", " << endl;
            else    
                outFile << endl;
        }
        outFile << "]" << endl;
        outFile << "}," << endl;
        outFile << "\"actions\": [" << endl;
        outFile.close();
    } else {
        std::cerr << "Cannot open file to write.\n";
    }
}
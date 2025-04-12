#include "../header/GPT2bot.h"
#include <iostream>
#include <cstdlib>  // For random actions, if needed
#define BIG 10


GPT2bot::GPT2bot(int initialStack) : Player(initialStack) {
    // Jeśli chcesz coś dodatkowego w konstruktorze, np. ustawienia specyficzne dla GPT1bot
    std::cout << "GPT1bot has been created with stack: " << initialStack << std::endl;
}

Action GPT2bot::makeAction(int raiseMoney, int pot, int button) {
    // Prosta analiza siły ręki preflop
    Card card1 = hand[0];
    Card card2 = hand[1];

    // Kody dla kart: 0-12 dla 2-10, 13-25 dla J-K, 26-38 dla A (przykład)
    int rank1 = card1.getNumber();
    int rank2 = card2.getNumber();

    // Zmienna do obliczenia nowego raise'a
    int Raise = raiseMoney;  // Początkowy raise to wartość przekazana jako argument
    if(stack <= 0){
        setAction(PASS);
        return PASS;
    }

    // **Premium Hands (Raise Aggressively)**: AA, KK, QQ, AK
    if ((rank1 == rank2 && rank1 >= 12) || // Pair AA, KK, QQ, JJ
        (rank1 == 12 && rank2 >= 8) || (rank2 == 12 && rank1 >= 8) || // AK, AQ
        (rank1 == 13 && rank2 == 12) || (rank2 == 13 && rank1 == 12)) { // KQ
        // Raise aggressively with strong hands
        Raise += 15;  // Increase raise more significantly for premium hands
        setAction(RAISE);
        return RAISE;
    }

    // **Middle Hands (Moderate Raise or Call)**: 88, 99, 10-10, A10, K10
    if ((rank1 == rank2 && rank1 >= 8) || // Pair 88, 99, 10-10
        (rank1 >= 8 && rank2 >= 8) || // 8-9, 9-10, A10, K10
        (rank1 == 11 && rank2 >= 8) || (rank2 == 11 && rank1 >= 8)) { // AJ, KJ
        // Consider call or moderate raise based on the situation
        if (raiseMoney > 10) {
            // If the previous raise is significant, call to see the flop
            setAction(CALL);
            return CALL;
        }
        // If it's a smaller raise, consider raising moderately
        Raise += 7;  // Moderate raise
        setAction(RAISE);
        return RAISE;
    }

    // **Weak Hands (Pass or Call if Pot is Large)**: 2-7, 3-9
    if ((rank1 < 8 && rank2 < 8) || // Low cards like 2-7, 3-9
        (rank1 != rank2 && (rank1 < 7 || rank2 < 7))) {
        // If the pot is large and the bot has a big stack, it might be willing to call with weak hands
        if (pot > 50 && stack > 30) {
            setAction(CALL);
            return CALL; // Call with weak hands in large pots
        }
        setAction(PASS);
        return PASS; // Pass weak hands
    }

    // **Default Action (Call)**: If none of the conditions match, just call
    setAction(CALL);
    return CALL;
}
#include "../header/GPT1bot.h"
#include <iostream>
#include <cstdlib>  // For random actions, if needed
#define BIG 10

GPT1bot::GPT1bot(int initialStack) : Player(initialStack) {
    // Jeśli chcesz coś dodatkowego w konstruktorze, np. ustawienia specyficzne dla GPT1bot
    std::cout << "GPT1bot has been created with stack: " << initialStack << std::endl;
}

Action GPT1bot::makeAction(int raiseMoney, int pot, int button) {
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

    // Ręce mocne (AA, KK, QQ, AK itp.)
    if ((rank1 == rank2 && rank1 >= 12) || // para AA, KK, QQ, JJ
        (rank1 == 12 && rank2 >= 8) || (rank2 == 12 && rank1 >= 8) || // AK, AQ
        (rank1 == 13 && rank2 == 12) || (rank2 == 13 && rank1 == 12)) { // KQ
        // Jeśli mamy mocną rękę, podnosimy stawkę
        Raise = std::max(raiseMoney * 2, 10);  // Podwajamy poprzednie podbicie (minimalnie 10)
        setAction(RAISE);
        return RAISE;
    }

    // Średnie ręce
    if ((rank1 == rank2 && rank1 >= 8) || // para 88, 99, 10-10
        (rank1 >= 8 && rank2 >= 8)) { // 8-9, 9-10, 8-10 itp.
        if (raiseMoney > 10) {
            // Jeśli już był raise, decydujemy, czy call, czy raise
            Raise = std::max(raiseMoney * 2, 5);  // Podwajamy poprzednie podbicie (minimalnie 5)
            setAction(RAISE);
            return RAISE;
        }
        Raise = std::max(raiseMoney * 2, 5);  // Podwajamy poprzednie podbicie (minimalnie 5)
        setAction(RAISE);
        return RAISE;  // Lekki raise
    }

    // Słabe ręce
    if ((rank1 < 8 && rank2 < 8) || // niskie karty jak 2-7, 3-9
        (rank1 != rank2 && (rank1 < 7 || rank2 < 7))) {
        setAction(PASS);
        return PASS; // Zbyt słaba ręka
    }

    // Jeśli żaden z powyższych warunków nie pasuje, decydujemy się na CALL
    setAction(CALL);
    return CALL;
}

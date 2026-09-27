#include "src/nfa.h"
#include "src/minimizer.h"

#include <iostream>
#include <string>
#include <vector>

int main() {
    DFA dfa;

    StateSet q0 = {0};
    StateSet q1 = {1};

    // ==================================================
    // CONSTRUCCION DEL DFA
    // ==================================================

    dfa.start_state = q0;

    dfa.states = {
        q0,
        q1
    };

    dfa.accept_states = {
        q1
    };

    dfa.alphabet = {
        'a'
    };

    // Transiciones:
    //
    // q0 --a--> q1
    // q1 --a--> q1
    //
    // q0 y q1 no son equivalentes porque
    // q1 es de aceptacion y q0 no lo es.

    dfa.transitions[q0]['a'] = q1;
    dfa.transitions[q1]['a'] = q1;

    // ==================================================
    // DFA ORIGINAL
    // ==================================================

    std::cout << "============================================\n";
    std::cout << "PRUEBA DE DFA YA MINIMO\n";
    std::cout << "============================================\n";

    std::cout << "\n--- DFA ORIGINAL ---\n";

    printDFA(dfa);

    std::cout << "\nEstados antes: "
              << dfa.states.size()
              << "\n";

    // ==================================================
    // MINIMIZACION
    // ==================================================

    DFA minimized = minimizeDFA(dfa);

    std::cout << "\n--- DFA DESPUES DE MINIMIZAR ---\n";

    printMinimizedDFA(minimized);

    std::cout << "\nEstados despues: "
              << minimized.states.size()
              << "\n";

    // ==================================================
    // COMPROBAR QUE YA ERA MINIMO
    // ==================================================

    bool sameSize =
        dfa.states.size() == minimized.states.size();

    std::cout << "DFA ya era minimo: "
              << (sameSize ? "OK" : "ERROR")
              << "\n";

    // ==================================================
    // PRUEBAS FUNCIONALES
    // ==================================================

    std::vector<std::string> tests = {
        "",
        "a",
        "aa",
        "aaa",
        "aaaa"
    };

    int correct = 0;

    std::cout << "\nPruebas funcionales:\n";

    for (const std::string& input : tests) {

        bool originalResult =
            testString(dfa, input);

        bool minimizedResult =
            testString(minimized, input);

        bool ok =
            originalResult == minimizedResult;

        std::cout << "\""
                  << input
                  << "\" -> Original: "
                  << (originalResult ? "ACEPTA" : "RECHAZA")
                  << " | Minimo: "
                  << (minimizedResult ? "ACEPTA" : "RECHAZA")
                  << " | "
                  << (ok ? "OK" : "ERROR")
                  << "\n";

        if (ok) {
            ++correct;
        }
    }

    std::cout << "\nResultado DFA ya minimo: "
              << correct
              << "/"
              << tests.size()
              << " correctas\n";

    return 0;
}

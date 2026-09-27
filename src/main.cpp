#include "nfa.h"
#include "minimizer.h"
#include "regex_to_nfa.h"

#include <iostream>
#include <string>
#include <vector>

// ==================================================
// PRUEBAS DE CADENAS
// ==================================================

void runTestSuite(
    const DFA& original,
    const DFA& minimized,
    const std::vector<std::string>& accepted,
    const std::vector<std::string>& rejected
) {
    int correct = 0;
    int total = 0;

    std::cout << "\nPRUEBAS DE ACEPTACION\n";

    for (const std::string& input : accepted) {
        bool originalResult = testString(original, input);
        bool minimizedResult = testString(minimized, input);

        bool ok =
            originalResult &&
            minimizedResult &&
            originalResult == minimizedResult;

        std::cout << "\"" << input << "\""
                  << " -> Original: "
                  << (originalResult ? "ACEPTA" : "RECHAZA")
                  << " | Minimo: "
                  << (minimizedResult ? "ACEPTA" : "RECHAZA")
                  << " | "
                  << (ok ? "OK" : "ERROR")
                  << "\n";

        if (ok) {
            ++correct;
        }

        ++total;
    }

    std::cout << "\nPRUEBAS DE RECHAZO\n";

    for (const std::string& input : rejected) {
        bool originalResult = testString(original, input);
        bool minimizedResult = testString(minimized, input);

        bool ok =
            !originalResult &&
            !minimizedResult &&
            originalResult == minimizedResult;

        std::cout << "\"" << input << "\""
                  << " -> Original: "
                  << (originalResult ? "ACEPTA" : "RECHAZA")
                  << " | Minimo: "
                  << (minimizedResult ? "ACEPTA" : "RECHAZA")
                  << " | "
                  << (ok ? "OK" : "ERROR")
                  << "\n";

        if (ok) {
            ++correct;
        }

        ++total;
    }

    std::cout << "\nResultado de pruebas: "
              << correct << "/" << total
              << " correctas\n";
}

// ==================================================
// EJECUTAR UNA REGEX COMPLETA
// ==================================================

void runRegex(
    int number,
    const std::string& regex,
    const std::vector<std::string>& accepted,
    const std::vector<std::string>& rejected
) {
    std::cout << "\n\n";
    std::cout << "============================================\n";
    std::cout << "REGEX " << number << ": " << regex << "\n";
    std::cout << "============================================\n";

    NFA nfa = regexToNFA(regex);

    DFA dfa = subsetConstruction(nfa);

    std::cout << "\n--- DFA ANTES DE MINIMIZAR ---\n";

    printDFA(dfa);

    std::cout << "\nNumero de estados original: "
              << dfa.states.size()
              << "\n";

    DFA minimized = minimizeDFA(dfa);

    std::cout << "\n--- DFA DESPUES DE MINIMIZAR ---\n";

    printMinimizedDFA(minimized);

    std::cout << "\nNumero de estados minimizado: "
              << minimized.states.size()
              << "\n";

    std::cout << "Verificacion |Q| >= |Q'|: ";

    if (dfa.states.size() >= minimized.states.size()) {
        std::cout << "OK\n";
    } else {
        std::cout << "ERROR\n";
    }

    runTestSuite(
        dfa,
        minimized,
        accepted,
        rejected
    );
}

// ==================================================
// PRUEBA DE DFA INCOMPLETO
// ==================================================

void testIncompleteDFA() {
    DFA dfa;

    StateSet q0 = {0};
    StateSet q1 = {1};

    dfa.start_state = q0;

    dfa.states = {
        q0,
        q1
    };

    dfa.accept_states = {
        q1
    };

    dfa.alphabet = {
        'a',
        'b'
    };

    // No existen transiciones con b.
    dfa.transitions[q0]['a'] = q1;
    dfa.transitions[q1]['a'] = q1;

    std::cout << "\n\n";
    std::cout << "============================================\n";
    std::cout << "PRUEBA DE DFA INCOMPLETO\n";
    std::cout << "============================================\n";

    std::cout << "\n--- DFA ORIGINAL ---\n";

    printDFA(dfa);

    DFA minimized = minimizeDFA(dfa);

    std::cout << "\n--- DFA MINIMIZADO Y COMPLETADO ---\n";

    printMinimizedDFA(minimized);

    std::cout << "\nEstados antes: "
              << dfa.states.size()
              << "\n";

    std::cout << "Estados despues: "
              << minimized.states.size()
              << "\n";

    std::cout << "\nPruebas funcionales:\n";

    std::vector<std::string> tests = {
        "",
        "a",
        "aa",
        "aaa",
        "b",
        "ab",
        "ba",
        "aab"
    };

    int correct = 0;

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

    std::cout << "\nResultado DFA incompleto: "
              << correct
              << "/"
              << tests.size()
              << " correctas\n";
}

// ==================================================
// PRUEBA DE ESTADO INALCANZABLE
// ==================================================

void testUnreachableState() {
    DFA dfa;

    StateSet q0 = {0};
    StateSet q1 = {1};
    StateSet q2 = {2};

    // q0 es inicial.
    dfa.start_state = q0;

    // El DFA contiene tres estados.
    dfa.states = {
        q0,
        q1,
        q2
    };

    // q1 es el unico estado de aceptacion.
    dfa.accept_states = {
        q1
    };

    dfa.alphabet = {
        'a'
    };

    // --------------------------------------------------
    // Transiciones
    // --------------------------------------------------
    //
    // q0 --a--> q1
    // q1 --a--> q1
    // q2 --a--> q2
    //
    // q2 nunca puede alcanzarse desde q0.

    dfa.transitions[q0]['a'] = q1;
    dfa.transitions[q1]['a'] = q1;
    dfa.transitions[q2]['a'] = q2;

    std::cout << "\n\n";
    std::cout << "============================================\n";
    std::cout << "PRUEBA DE ESTADO INALCANZABLE\n";
    std::cout << "============================================\n";

    std::cout << "\n--- DFA ORIGINAL ---\n";

    printDFA(dfa);

    std::cout << "\nEstados antes: "
              << dfa.states.size()
              << "\n";

    DFA minimized = minimizeDFA(dfa);

    std::cout << "\n--- DFA DESPUES DE MINIMIZAR ---\n";

    printMinimizedDFA(minimized);

    std::cout << "\nEstados despues: "
              << minimized.states.size()
              << "\n";

    // --------------------------------------------------
    // Comprobar que q2 desaparecio
    // --------------------------------------------------

    bool unreachableRemoved =
        minimized.states.size() == 2;

    std::cout << "Estado inalcanzable eliminado: "
              << (unreachableRemoved ? "OK" : "ERROR")
              << "\n";

    // --------------------------------------------------
    // Pruebas funcionales
    // --------------------------------------------------

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

    std::cout << "\nResultado estado inalcanzable: "
              << correct
              << "/"
              << tests.size()
              << " correctas\n";
}

// ==================================================
// PROGRAMA PRINCIPAL
// ==================================================

int main() {

    // ==================================================
    // REGEX 1
    // (a|b)*abb
    // ==================================================

    std::vector<std::string> accepted1 = {
        "abb",
        "aabb",
        "babb",
        "aaabb",
        "bbabb",
        "abababb",
        "baabb",
        "aaaabb",
        "bbbabb",
        "abbaabb"
    };

    std::vector<std::string> rejected1 = {
        "",
        "a",
        "b",
        "ab",
        "aba",
        "abba",
        "abbb",
        "baa",
        "bab",
        "aaaa"
    };

    // ==================================================
    // REGEX 2
    // (0|1)*01(0|1)*
    // ==================================================

    std::vector<std::string> accepted2 = {
        "01",
        "001",
        "011",
        "101",
        "010",
        "1101",
        "0010",
        "1011",
        "0101",
        "10010"
    };

    std::vector<std::string> rejected2 = {
        "",
        "0",
        "1",
        "00",
        "11",
        "10",
        "111",
        "000",
        "1110",
        "1100"
    };

    // ==================================================
    // REGEX 3
    // (a|b)*ab(a|b)*
    // ==================================================

    std::vector<std::string> accepted3 = {
        "ab",
        "aab",
        "abb",
        "aba",
        "bab",
        "aaab",
        "baba",
        "abba",
        "bbab",
        "aabb"
    };

    std::vector<std::string> rejected3 = {
        "",
        "a",
        "b",
        "aa",
        "bb",
        "ba",
        "aaa",
        "bbb",
        "bba",
        "baaa"
    };

    // ==================================================
    // EJECUTAR LAS TRES REGEX
    // ==================================================

    runRegex(
        1,
        "(a|b)*abb",
        accepted1,
        rejected1
    );

    runRegex(
        2,
        "(0|1)*01(0|1)*",
        accepted2,
        rejected2
    );

    runRegex(
        3,
        "(a|b)*ab(a|b)*",
        accepted3,
        rejected3
    );

    // ==================================================
    // CASOS LIMITE
    // ==================================================

    testIncompleteDFA();

    testUnreachableState();

    return 0;
}

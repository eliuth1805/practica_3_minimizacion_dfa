#include "nfa.h"

#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

// ==================================================
// MOVE
// ==================================================

StateSet move(
    const NFA& nfa,
    const StateSet& states,
    char symbol
) {
    StateSet result;

    for (State state : states) {
        auto stateIt = nfa.transitions.find(state);

        if (stateIt == nfa.transitions.end()) {
            continue;
        }

        auto symbolIt = stateIt->second.find(symbol);

        if (symbolIt == stateIt->second.end()) {
            continue;
        }

        result.insert(
            symbolIt->second.begin(),
            symbolIt->second.end()
        );
    }

    return result;
}

// ==================================================
// EPSILON-CLOSURE
// ==================================================

StateSet epsilonClosure(
    const NFA& nfa,
    const StateSet& states
) {
    StateSet closure = states;

    std::vector<State> stack(
        states.begin(),
        states.end()
    );

    while (!stack.empty()) {
        State current = stack.back();
        stack.pop_back();

        auto it =
            nfa.epsilon_transitions.find(current);

        if (it == nfa.epsilon_transitions.end()) {
            continue;
        }

        for (State next : it->second) {
            if (closure.insert(next).second) {
                stack.push_back(next);
            }
        }
    }

    return closure;
}

// ==================================================
// CONSTRUCCION DE SUBCONJUNTOS
// ==================================================

DFA subsetConstruction(const NFA& nfa) {
    DFA dfa;

    dfa.alphabet = nfa.alphabet;

    dfa.start_state =
        epsilonClosure(
            nfa,
            {nfa.start_state}
        );

    std::queue<StateSet> pending;

    dfa.states.insert(dfa.start_state);
    pending.push(dfa.start_state);

    while (!pending.empty()) {
        StateSet current = pending.front();
        pending.pop();

        for (char symbol : nfa.alphabet) {
            StateSet moved =
                move(
                    nfa,
                    current,
                    symbol
                );

            StateSet next =
                epsilonClosure(
                    nfa,
                    moved
                );

            if (next.empty()) {
                continue;
            }

            if (dfa.states.insert(next).second) {
                pending.push(next);
            }

            dfa.transitions[current][symbol] =
                next;
        }
    }

    // Determinar estados de aceptacion.
    for (const StateSet& state : dfa.states) {
        for (State accept : nfa.accept_states) {
            if (state.count(accept)) {
                dfa.accept_states.insert(state);
                break;
            }
        }
    }

    return dfa;
}

// ==================================================
// MOSTRAR UN CONJUNTO DE ESTADOS
// ==================================================

void printStateSet(const StateSet& states) {
    std::cout << "{";

    bool first = true;

    for (State state : states) {
        if (!first) {
            std::cout << ",";
        }

        std::cout << state;
        first = false;
    }

    std::cout << "}";
}

// ==================================================
// MOSTRAR NFA
// ==================================================

void printNFA(const NFA& nfa) {
    std::cout << "=== NFA ===\n";

    std::cout << "Estado inicial: "
              << nfa.start_state
              << "\n";

    std::cout << "Estados de aceptacion: ";

    printStateSet(nfa.accept_states);

    std::cout << "\n\n";

    std::cout << "Transiciones:\n";

    for (const auto& [state, bySymbol]
         : nfa.transitions) {

        for (const auto& [symbol, destinations]
             : bySymbol) {

            std::cout << state
                      << " --"
                      << symbol
                      << "--> ";

            printStateSet(destinations);

            std::cout << "\n";
        }
    }

    for (const auto& [state, destinations]
         : nfa.epsilon_transitions) {

        std::cout << state
                  << " --epsilon--> ";

        printStateSet(destinations);

        std::cout << "\n";
    }
}

// ==================================================
// MOSTRAR DFA ORIGINAL
// ==================================================

void printDFA(const DFA& dfa) {
    std::cout << "\n=== DFA ===\n";

    std::cout << "Estado inicial: ";
    printStateSet(dfa.start_state);
    std::cout << "\n";

    std::cout << "Estados de aceptacion:\n";

    for (const StateSet& state : dfa.accept_states) {
        std::cout << "  ";
        printStateSet(state);
        std::cout << "\n";
    }

    std::cout << "\nTabla de transiciones:\n";

    for (const StateSet& state : dfa.states) {

        auto stateIt =
            dfa.transitions.find(state);

        if (stateIt == dfa.transitions.end()) {
            continue;
        }

        for (char symbol : dfa.alphabet) {

            auto symbolIt =
                stateIt->second.find(symbol);

            if (symbolIt == stateIt->second.end()) {
                continue;
            }

            printStateSet(state);

            std::cout << " --"
                      << symbol
                      << "--> ";

            printStateSet(symbolIt->second);

            std::cout << "\n";
        }
    }
}

// ==================================================
// MOSTRAR DFA MINIMIZADO CON M0, M1, M2, ...
// ==================================================

void printMinimizedDFA(const DFA& dfa) {
    std::cout << "\n=== DFA MINIMIZADO ===\n";

    if (dfa.states.empty()) {
        std::cout << "DFA vacio\n";
        return;
    }

    // Relacion entre cada estado interno y su nombre M.
    std::map<StateSet, std::string> names;

    // Mantiene el orden M0, M1, M2, ...
    std::vector<StateSet> orderedStates;

    int nextName = 0;

    // --------------------------------------------------
    // M0 siempre es el estado inicial
    // --------------------------------------------------

    names[dfa.start_state] =
        "M" + std::to_string(nextName++);

    orderedStates.push_back(dfa.start_state);

    // --------------------------------------------------
    // Nombrar los estados restantes
    // --------------------------------------------------

    for (const StateSet& state : dfa.states) {
        if (state == dfa.start_state) {
            continue;
        }

        names[state] =
            "M" + std::to_string(nextName++);

        orderedStates.push_back(state);
    }

    // --------------------------------------------------
    // Estado inicial
    // --------------------------------------------------

    std::cout << "Estado inicial: "
              << names[dfa.start_state]
              << "\n";

    // --------------------------------------------------
    // Estados de aceptacion
    // --------------------------------------------------

    std::cout << "Estados de aceptacion: ";

    bool first = true;

    for (const StateSet& state : orderedStates) {

        if (!dfa.accept_states.count(state)) {
            continue;
        }

        if (!first) {
            std::cout << ", ";
        }

        std::cout << names[state];
        first = false;
    }

    if (first) {
        std::cout << "ninguno";
    }

    std::cout << "\n";

    // --------------------------------------------------
    // Tabla de transiciones
    // --------------------------------------------------

    std::cout << "\nTabla de transiciones:\n";

    // Se utiliza orderedStates para garantizar
    // el orden M0, M1, M2, ...
    for (const StateSet& state : orderedStates) {

        auto stateIt =
            dfa.transitions.find(state);

        if (stateIt == dfa.transitions.end()) {
            continue;
        }

        for (char symbol : dfa.alphabet) {

            auto symbolIt =
                stateIt->second.find(symbol);

            if (symbolIt == stateIt->second.end()) {
                continue;
            }

            const StateSet& destination =
                symbolIt->second;

            std::cout << names[state]
                      << " --"
                      << symbol
                      << "--> "
                      << names[destination]
                      << "\n";
        }
    }

    // --------------------------------------------------
    // Correspondencia con representacion interna
    // --------------------------------------------------

    std::cout << "\nCorrespondencia de estados:\n";

    for (const StateSet& state : orderedStates) {

        std::cout << names[state]
                  << " = ";

        printStateSet(state);

        if (state == dfa.start_state) {

            std::cout << "  [inicial";

            if (dfa.accept_states.count(state)) {
                std::cout << ", aceptacion";
            }

            std::cout << "]";
        }
        else if (state.empty()) {

            std::cout << "  [sumidero]";
        }
        else if (dfa.accept_states.count(state)) {

            std::cout << "  [aceptacion]";
        }

        std::cout << "\n";
    }
}

// ==================================================
// PRUEBA DE CADENAS EN EL DFA
// ==================================================

bool testString(
    const DFA& dfa,
    const std::string& input
) {
    StateSet current = dfa.start_state;

    for (char symbol : input) {

        // El simbolo debe pertenecer al alfabeto.
        if (!dfa.alphabet.count(symbol)) {
            return false;
        }

        auto stateIt =
            dfa.transitions.find(current);

        // Deben existir transiciones desde
        // el estado actual.
        if (stateIt == dfa.transitions.end()) {
            return false;
        }

        auto symbolIt =
            stateIt->second.find(symbol);

        // Debe existir una transicion
        // para el simbolo actual.
        if (symbolIt == stateIt->second.end()) {
            return false;
        }

        current =
            symbolIt->second;
    }

    // La cadena se acepta solamente si termina
    // en un estado de aceptacion.
    return
        dfa.accept_states.count(current) > 0;
}

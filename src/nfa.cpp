#include "nfa.h"

#include <iostream>
#include <queue>

// ==================================================
// MOVE
// ==================================================

StateSet move(const NFA& nfa, const StateSet& states, char symbol) {
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

        result.insert(symbolIt->second.begin(), symbolIt->second.end());
    }

    return result;
}

// ==================================================
// EPSILON-CLOSURE
// ==================================================

StateSet epsilonClosure(const NFA& nfa, const StateSet& states) {
    StateSet closure = states;
    std::vector<State> stack(states.begin(), states.end());

    while (!stack.empty()) {
        State current = stack.back();
        stack.pop_back();

        auto it = nfa.epsilon_transitions.find(current);

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
    dfa.start_state = epsilonClosure(nfa, {nfa.start_state});

    std::queue<StateSet> pending;

    dfa.states.insert(dfa.start_state);
    pending.push(dfa.start_state);

    while (!pending.empty()) {
        StateSet current = pending.front();
        pending.pop();

        for (char symbol : nfa.alphabet) {
            StateSet moved = move(nfa, current, symbol);
            StateSet next = epsilonClosure(nfa, moved);

            if (next.empty()) {
                continue;
            }

            if (dfa.states.insert(next).second) {
                pending.push(next);
            }

            dfa.transitions[current][symbol] = next;
        }
    }

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
// FUNCIONES PARA MOSTRAR RESULTADOS
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

void printNFA(const NFA& nfa) {
    std::cout << "=== NFA ===\n";

    std::cout << "Estado inicial: " << nfa.start_state << "\n";

    std::cout << "Estados de aceptacion: ";
    printStateSet(nfa.accept_states);
    std::cout << "\n\n";

    std::cout << "Transiciones:\n";

    for (const auto& [state, bySymbol] : nfa.transitions) {
        for (const auto& [symbol, destinations] : bySymbol) {
            std::cout << state << " --" << symbol << "--> ";
            printStateSet(destinations);
            std::cout << "\n";
        }
    }

    for (const auto& [state, destinations] : nfa.epsilon_transitions) {
        std::cout << state << " --epsilon--> ";
        printStateSet(destinations);
        std::cout << "\n";
    }
}

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
        for (char symbol : dfa.alphabet) {
            auto stateIt = dfa.transitions.find(state);

            if (stateIt == dfa.transitions.end()) {
                continue;
            }

            auto symbolIt = stateIt->second.find(symbol);

            if (symbolIt == stateIt->second.end()) {
                continue;
            }

            printStateSet(state);
            std::cout << " --" << symbol << "--> ";
            printStateSet(symbolIt->second);
            std::cout << "\n";
        }
    }
}

#include "nfa.h"

#include <iostream>

int main() {
    NFA nfa;

    // Alfabeto
    nfa.alphabet = {'a', 'b'};

    // Estado inicial
    nfa.start_state = 0;

    // Estado de aceptacion
    nfa.accept_states = {3};

    // Transiciones epsilon
    nfa.epsilon_transitions[0] = {1};
    nfa.epsilon_transitions[1] = {2};
    nfa.epsilon_transitions[2] = {1};

    // Transiciones normales
    nfa.transitions[1]['a'] = {1, 2};
    nfa.transitions[2]['b'] = {3};

    // Mostrar el NFA original
    printNFA(nfa);

    // Prueba de epsilon-closure
    StateSet initial = {0};
    StateSet closure = epsilonClosure(nfa, initial);

    std::cout << "\nPrueba epsilon-closure({0}): ";
    printStateSet(closure);
    std::cout << "\n";

    // Prueba de move
    StateSet moveResult = move(nfa, {1}, 'a');

    std::cout << "Prueba move({1}, a): ";
    printStateSet(moveResult);
    std::cout << "\n";

    // Conversion NFA -> DFA
    DFA dfa = subsetConstruction(nfa);

    // Mostrar el DFA resultante
    printDFA(dfa);

    return 0;
}

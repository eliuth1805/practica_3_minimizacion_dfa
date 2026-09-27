#ifndef NFA_H
#define NFA_H

#include <map>
#include <set>
#include <string>
#include <vector>

using State = int;
using StateSet = std::set<State>;

// ==================================================
// ESTRUCTURA DEL NFA
// ==================================================

struct NFA {
    State start_state;
    StateSet accept_states;
    std::set<char> alphabet;

    std::map<State, std::map<char, StateSet>> transitions;
    std::map<State, StateSet> epsilon_transitions;
};

// ==================================================
// ESTRUCTURA DEL DFA
// ==================================================

struct DFA {
    StateSet start_state;
    std::set<StateSet> states;
    std::set<StateSet> accept_states;
    std::set<char> alphabet;

    std::map<StateSet, std::map<char, StateSet>> transitions;
};

// ==================================================
// OPERACIONES DEL NFA
// ==================================================

StateSet move(
    const NFA& nfa,
    const StateSet& states,
    char symbol
);

StateSet epsilonClosure(
    const NFA& nfa,
    const StateSet& states
);

// ==================================================
// CONSTRUCCION DE SUBCONJUNTOS
// ==================================================

DFA subsetConstruction(const NFA& nfa);

// ==================================================
// FUNCIONES DE IMPRESION
// ==================================================

void printStateSet(const StateSet& states);

void printNFA(const NFA& nfa);

void printDFA(const DFA& dfa);

// Imprime un DFA minimizado utilizando nombres
// canonicos M0, M1, M2, ...
void printMinimizedDFA(const DFA& dfa);

// ==================================================
// PRUEBA DE CADENAS
// ==================================================

// Prueba una cadena sobre un DFA.
// Devuelve true si la cadena es aceptada
// y false si es rechazada.
bool testString(
    const DFA& dfa,
    const std::string& input
);

#endif

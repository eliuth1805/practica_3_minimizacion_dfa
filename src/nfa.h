#ifndef NFA_H
#define NFA_H

#include <map>
#include <set>
#include <string>
#include <vector>

using State = int;
using StateSet = std::set<State>;

struct NFA {
    State start_state;
    StateSet accept_states;
    std::set<char> alphabet;

    std::map<State, std::map<char, StateSet>> transitions;
    std::map<State, StateSet> epsilon_transitions;
};

struct DFA {
    StateSet start_state;
    std::set<StateSet> states;
    std::set<StateSet> accept_states;
    std::set<char> alphabet;

    std::map<StateSet, std::map<char, StateSet>> transitions;
};

StateSet move(const NFA& nfa, const StateSet& states, char symbol);

StateSet epsilonClosure(const NFA& nfa, const StateSet& states);

DFA subsetConstruction(const NFA& nfa);

void printStateSet(const StateSet& states);

void printNFA(const NFA& nfa);

void printDFA(const DFA& dfa);

#endif

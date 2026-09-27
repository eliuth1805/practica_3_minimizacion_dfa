#ifndef MINIMIZER_H
#define MINIMIZER_H

#include "nfa.h"

// Minimiza un DFA usando refinamiento de particiones de Hopcroft.
DFA minimizeDFA(const DFA& dfa);

#endif

#ifndef REGEX_TO_NFA_H
#define REGEX_TO_NFA_H

#include "nfa.h"

#include <string>

// Convierte una expresion regular a un NFA
// utilizando la construccion de Thompson.
NFA regexToNFA(const std::string& regex);

#endif

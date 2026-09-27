#include "regex_to_nfa.h"

#include <stack>
#include <string>
#include <vector>

// ==================================================
// OPERADORES DE EXPRESIONES REGULARES
// ==================================================

static bool isOperator(char c) {
    return c == '|' ||
           c == '.' ||
           c == '*' ||
           c == '+' ||
           c == '?' ||
           c == '(' ||
           c == ')';
}

static int precedence(char c) {
    switch (c) {
        case '*':
        case '+':
        case '?':
            return 3;

        case '.':
            return 2;

        case '|':
            return 1;

        default:
            return 0;
    }
}

// ==================================================
// CONCATENACION EXPLICITA
// ==================================================

static bool needsConcat(char left, char right) {
    bool leftValid =
        !isOperator(left) ||
        left == ')' ||
        left == '*' ||
        left == '+' ||
        left == '?';

    bool rightValid =
        !isOperator(right) ||
        right == '(';

    return leftValid && rightValid;
}

static std::string insertConcat(const std::string& input) {
    std::string result;

    for (std::size_t i = 0; i < input.size(); ++i) {
        result += input[i];

        if (i + 1 < input.size() &&
            needsConcat(input[i], input[i + 1])) {
            result += '.';
        }
    }

    return result;
}

// ==================================================
// REGEX A POSTFIX
// ==================================================

static std::string toPostfix(const std::string& regex) {
    std::string explicitRegex = insertConcat(regex);
    std::string output;
    std::stack<char> operators;

    for (char c : explicitRegex) {
        if (!isOperator(c)) {
            output += c;
        }
        else if (c == '(') {
            operators.push(c);
        }
        else if (c == ')') {
            while (!operators.empty() &&
                   operators.top() != '(') {
                output += operators.top();
                operators.pop();
            }

            if (!operators.empty() &&
                operators.top() == '(') {
                operators.pop();
            }
        }
        else if (c == '*' || c == '+' || c == '?') {
            output += c;
        }
        else {
            while (!operators.empty() &&
                   operators.top() != '(' &&
                   precedence(operators.top()) >= precedence(c)) {
                output += operators.top();
                operators.pop();
            }

            operators.push(c);
        }
    }

    while (!operators.empty()) {
        output += operators.top();
        operators.pop();
    }

    return output;
}

// ==================================================
// FRAGMENTO DE THOMPSON
// ==================================================

struct Fragment {
    State start;
    State accept;
};

static State newState(State& nextState) {
    return nextState++;
}

// ==================================================
// REGEX A NFA - THOMPSON
// ==================================================

NFA regexToNFA(const std::string& regex) {
    NFA nfa;

    std::string postfix = toPostfix(regex);

    std::vector<Fragment> stack;
    State nextState = 0;

    for (char c : postfix) {

        // ------------------------------------------
        // CONCATENACION
        // ------------------------------------------

        if (c == '.') {
            Fragment right = stack.back();
            stack.pop_back();

            Fragment left = stack.back();
            stack.pop_back();

            nfa.epsilon_transitions[left.accept].insert(right.start);

            stack.push_back({left.start, right.accept});
        }

        // ------------------------------------------
        // UNION
        // ------------------------------------------

        else if (c == '|') {
            Fragment right = stack.back();
            stack.pop_back();

            Fragment left = stack.back();
            stack.pop_back();

            State start = newState(nextState);
            State accept = newState(nextState);

            nfa.epsilon_transitions[start].insert(left.start);
            nfa.epsilon_transitions[start].insert(right.start);

            nfa.epsilon_transitions[left.accept].insert(accept);
            nfa.epsilon_transitions[right.accept].insert(accept);

            stack.push_back({start, accept});
        }

        // ------------------------------------------
        // CERRADURA DE KLEENE *
        // ------------------------------------------

        else if (c == '*') {
            Fragment inner = stack.back();
            stack.pop_back();

            State start = newState(nextState);
            State accept = newState(nextState);

            nfa.epsilon_transitions[start].insert(inner.start);
            nfa.epsilon_transitions[start].insert(accept);

            nfa.epsilon_transitions[inner.accept].insert(inner.start);
            nfa.epsilon_transitions[inner.accept].insert(accept);

            stack.push_back({start, accept});
        }

        // ------------------------------------------
        // OPERADOR +
        // ------------------------------------------

        else if (c == '+') {
            Fragment inner = stack.back();
            stack.pop_back();

            State start = newState(nextState);
            State accept = newState(nextState);

            nfa.epsilon_transitions[start].insert(inner.start);
            nfa.epsilon_transitions[inner.accept].insert(inner.start);
            nfa.epsilon_transitions[inner.accept].insert(accept);

            stack.push_back({start, accept});
        }

        // ------------------------------------------
        // OPERADOR ?
        // ------------------------------------------

        else if (c == '?') {
            Fragment inner = stack.back();
            stack.pop_back();

            State start = newState(nextState);
            State accept = newState(nextState);

            nfa.epsilon_transitions[start].insert(inner.start);
            nfa.epsilon_transitions[start].insert(accept);
            nfa.epsilon_transitions[inner.accept].insert(accept);

            stack.push_back({start, accept});
        }

        // ------------------------------------------
        // SIMBOLO NORMAL
        // ------------------------------------------

        else {
            State start = newState(nextState);
            State accept = newState(nextState);

            nfa.transitions[start][c].insert(accept);
            nfa.alphabet.insert(c);

            stack.push_back({start, accept});
        }
    }

    // El fragmento restante representa el NFA completo.
    if (!stack.empty()) {
        Fragment result = stack.back();

        nfa.start_state = result.start;
        nfa.accept_states.insert(result.accept);
    }

    return nfa;
}

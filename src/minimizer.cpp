#include "minimizer.h"

#include <algorithm>
#include <deque>
#include <map>
#include <queue>
#include <set>
#include <vector>

using DFABlock = std::set<StateSet>;
using Partition = std::vector<DFABlock>;

// ==================================================
// ESTADOS ALCANZABLES
// ==================================================

static DFABlock getReachableStates(const DFA& dfa) {
    DFABlock reachable;
    std::queue<StateSet> pending;

    reachable.insert(dfa.start_state);
    pending.push(dfa.start_state);

    while (!pending.empty()) {
        StateSet current = pending.front();
        pending.pop();

        auto stateIt = dfa.transitions.find(current);

        if (stateIt == dfa.transitions.end()) {
            continue;
        }

        for (const auto& [symbol, next] : stateIt->second) {
            (void)symbol;

            if (reachable.insert(next).second) {
                pending.push(next);
            }
        }
    }

    return reachable;
}

// ==================================================
// COMPLETAR EL DFA CON ESTADO SUMIDERO
// ==================================================

static DFA completeDFA(const DFA& input) {
    DFA dfa = input;

    // El conjunto vacio se utiliza como estado sumidero.
    StateSet sink;

    bool needsSink = false;

    // Primero verificamos si existe alguna transicion faltante.
    for (const StateSet& state : dfa.states) {
        for (char symbol : dfa.alphabet) {
            auto stateIt = dfa.transitions.find(state);

            if (stateIt == dfa.transitions.end() ||
                stateIt->second.find(symbol) == stateIt->second.end()) {
                needsSink = true;
            }
        }
    }

    if (!needsSink) {
        return dfa;
    }

    // Agregar el estado sumidero.
    dfa.states.insert(sink);

    // Toda transicion faltante va al sumidero.
    for (const StateSet& state : dfa.states) {
        for (char symbol : dfa.alphabet) {
            auto stateIt = dfa.transitions.find(state);

            if (stateIt == dfa.transitions.end() ||
                stateIt->second.find(symbol) == stateIt->second.end()) {
                dfa.transitions[state][symbol] = sink;
            }
        }
    }

    // El sumidero se queda en si mismo para cualquier simbolo.
    for (char symbol : dfa.alphabet) {
        dfa.transitions[sink][symbol] = sink;
    }

    return dfa;
}

// ==================================================
// BUSCAR EL BLOQUE QUE CONTIENE UN ESTADO
// ==================================================

static int findBlock(
    const Partition& partitions,
    const StateSet& state
) {
    for (std::size_t i = 0; i < partitions.size(); ++i) {
        if (partitions[i].count(state)) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

// ==================================================
// COMPROBAR SI UN BLOQUE ESTA EN W
// ==================================================

static bool containsBlock(
    const std::deque<DFABlock>& work,
    const DFABlock& block
) {
    return std::find(
        work.begin(),
        work.end(),
        block
    ) != work.end();
}

// ==================================================
// REEMPLAZAR UN BLOQUE DE W POR LOS DOS NUEVOS
// ==================================================

static void replaceBlockInWork(
    std::deque<DFABlock>& work,
    const DFABlock& oldBlock,
    const DFABlock& first,
    const DFABlock& second
) {
    for (auto it = work.begin(); it != work.end(); ++it) {
        if (*it == oldBlock) {
            it = work.erase(it);
            work.insert(it, second);
            work.insert(it, first);
            return;
        }
    }
}

// ==================================================
// MINIMIZACION DE DFA - HOPCROFT
// ==================================================

DFA minimizeDFA(const DFA& input) {
    DFA minimized;

    // --------------------------------------------------
    // 1. Completar DFA con estado sumidero si hace falta
    // --------------------------------------------------

    DFA dfa = completeDFA(input);

    minimized.alphabet = dfa.alphabet;

    // --------------------------------------------------
    // 2. Eliminar estados inalcanzables
    // --------------------------------------------------

    DFABlock reachable = getReachableStates(dfa);

    if (reachable.empty()) {
        return minimized;
    }

    // --------------------------------------------------
    // 3. Particion inicial P = {F, Q - F}
    // --------------------------------------------------

    DFABlock accepting;
    DFABlock nonAccepting;

    for (const StateSet& state : reachable) {
        if (dfa.accept_states.count(state)) {
            accepting.insert(state);
        } else {
            nonAccepting.insert(state);
        }
    }

    Partition partitions;

    if (!accepting.empty()) {
        partitions.push_back(accepting);
    }

    if (!nonAccepting.empty()) {
        partitions.push_back(nonAccepting);
    }

    // --------------------------------------------------
    // 4. Inicializar cola de trabajo W
    // --------------------------------------------------

    std::deque<DFABlock> work;

    if (!accepting.empty() && !nonAccepting.empty()) {
        work.push_back(accepting);
        work.push_back(nonAccepting);
    }
    else if (!accepting.empty()) {
        work.push_back(accepting);
    }
    else if (!nonAccepting.empty()) {
        work.push_back(nonAccepting);
    }

    // --------------------------------------------------
    // 5. Refinamiento de particiones
    // --------------------------------------------------

    while (!work.empty()) {
        DFABlock A = work.front();
        work.pop_front();

        for (char symbol : dfa.alphabet) {

            // X = {q | delta(q, symbol) pertenece a A}
            DFABlock X;

            for (const StateSet& state : reachable) {
                const StateSet& destination =
                    dfa.transitions.at(state).at(symbol);

                if (A.count(destination)) {
                    X.insert(state);
                }
            }

            Partition newPartitions;

            for (const DFABlock& Y : partitions) {
                DFABlock intersection;
                DFABlock difference;

                // Y1 = Y interseccion X
                // Y2 = Y - X
                for (const StateSet& state : Y) {
                    if (X.count(state)) {
                        intersection.insert(state);
                    } else {
                        difference.insert(state);
                    }
                }

                // Si ambas partes son no vacias,
                // Y debe dividirse.
                if (!intersection.empty() &&
                    !difference.empty()) {

                    newPartitions.push_back(intersection);
                    newPartitions.push_back(difference);

                    // Si Y estaba en W, se reemplaza por
                    // las dos nuevas particiones.
                    if (containsBlock(work, Y)) {
                        replaceBlockInWork(
                            work,
                            Y,
                            intersection,
                            difference
                        );
                    }
                    else {
                        // Si Y no estaba en W, Hopcroft
                        // agrega solamente el bloque menor.
                        if (intersection.size() <= difference.size()) {
                            work.push_back(intersection);
                        } else {
                            work.push_back(difference);
                        }
                    }
                }
                else {
                    newPartitions.push_back(Y);
                }
            }

            partitions = newPartitions;
        }
    }

    // --------------------------------------------------
    // 6. Crear un representante por cada bloque
    // --------------------------------------------------

    std::vector<StateSet> representatives;

    for (const DFABlock& block : partitions) {
        StateSet representative = *block.begin();

        representatives.push_back(representative);
        minimized.states.insert(representative);

        // Nuevo estado inicial.
        if (block.count(dfa.start_state)) {
            minimized.start_state = representative;
        }

        // Un bloque es de aceptacion si contiene
        // algun estado de aceptacion original.
        for (const StateSet& state : block) {
            if (dfa.accept_states.count(state)) {
                minimized.accept_states.insert(representative);
                break;
            }
        }
    }

    // --------------------------------------------------
    // 7. Construir transiciones del DFA minimizado
    // --------------------------------------------------

    for (std::size_t i = 0; i < partitions.size(); ++i) {
        const DFABlock& block = partitions[i];
        const StateSet& representative = representatives[i];

        // Todos los estados del bloque son equivalentes.
        // Basta utilizar uno para obtener las transiciones.
        const StateSet& originalState = *block.begin();

        for (char symbol : dfa.alphabet) {
            const StateSet& destination =
                dfa.transitions.at(originalState).at(symbol);

            int destinationBlock =
                findBlock(partitions, destination);

            if (destinationBlock != -1) {
                minimized.transitions[representative][symbol] =
                    representatives[destinationBlock];
            }
        }
    }

    return minimized;
}

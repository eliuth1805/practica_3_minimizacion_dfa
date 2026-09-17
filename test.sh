#!/bin/bash

set -e

echo "========================================"
echo "   VALIDACION PRACTICA 2: NFA A DFA"
echo "========================================"
echo

echo "[1] Configurando proyecto con CMake..."
cmake -S . -B build

echo
echo "[2] Compilando..."
cmake --build build

echo
echo "[3] Ejecutando pruebas..."
echo

./build/nfa_to_dfa

echo
echo "========================================"
echo "   VALIDACION FINALIZADA CORRECTAMENTE"
echo "========================================"

#ifndef NFA_H
#define NFA_H

#include "regex.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

/// Maximum number of states in the NFA.
#define MAX_STATES 64

/**
 * Structure representing the alphabet of the NFA, which includes
 * a mapping from characters to column indices for the transition table.
 */
typedef struct {
    char symbols[256];     // Array of symbols in the alphabet (up to 256 for all possible byte values)
    int char_to_col[256];  // Mapping from character to column index in the transition table
    int symbol_count;      // Number of symbols in the alphabet
} alphabet;
typedef alphabet Alphabet;

/**
 * Structure representing a Nondeterministic Finite Automaton (NFA).
 * - start_state: The initial state of the NFA.
 * - accept_states: A bitset representing the set of accept states.
 * - states: The total number of states in the NFA.
 * - nfa_alphabet: The alphabet used by the NFA.
 * - transitions: A transition table where transitions[from_state][symbol_col] is a bitset of destination states.
 *                MAX_STATES x nfa_alphabet.symbol_count matrix.
 * - epsilon_closure_cache: A cache for epsilon closures of states to optimize matching.
 */
typedef struct {
    uint8_t start_state;
    uint64_t accept_states;
    uint8_t states;
    alphabet nfa_alphabet;
    uint64_t** transitions;
    uint64_t* epsilon_closure_cache;
} NFA;
typedef NFA nfa;

NFA regex_to_nfa(const Regex r);
bool match_nfa(nfa automaton, const char *input, size_t input_length);

#endif

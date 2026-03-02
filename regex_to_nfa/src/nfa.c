#include "nfa.h"
#include "regex.h"

// Temporary structures for NFA construction

/**
 * Structure representing a temporary transition during NFA construction.
 * - from_state: The source state of the transition.
 * - symbol: The input symbol that triggers the transition (or EPSILON_VALUE for epsilon transitions).
 * - to_state: The destination state of the transition.
 */
struct temp_transition {
    uint8_t from_state;
    char symbol;
    uint8_t to_state;
};
typedef struct temp_transition t_transition;


/** 
 * Structure representing a temporary NFA during construction.
 * - start: The start state of the temporary NFA.
 * - end: The accept state of the temporary NFA.
 */
struct temp_nfa {
    uint8_t start;
    uint8_t end;
};
typedef struct temp_nfa t_nfa;

/**
 * Structure managing states and transitions during NFA construction.
 * - next_id: The next available state ID.
 * - states: An array of state IDs.
 * - states_count: The number of states currently in the manager.
 * - transitions: An array of temporary transitions.
 * - transitions_count: The number of transitions currently in the manager.
 * - manager_alphabet: The alphabet used in the NFA construction.   
 */
struct states_manager {
    uint8_t next_id;
    uint8_t states[MAX_STATES];
    uint8_t states_count;
    t_transition transitions[MAX_STATES * MAX_STATES];
    uint8_t transitions_count;
    alphabet manager_alphabet;
};
typedef struct states_manager states_manager;


// Helper functions

/**
 * @brief Function to create a new alphabet. 
 * Initializes the alphabet with the EPSILON_SYMBOL and 
 * sets up the char_to_col mapping.
 */
alphabet new_alphabet() {
    alphabet a = {0};
    
    // 1) Initialize char_to_col to -1 and symbols to 0.
    for (int i = 0; i < 256; i++) {
        a.char_to_col[i] = -1;
        a.symbols[i] = 0;
    }

    // 2) Place EPSILON_SYMBOL at symbols[0]
    a.symbols[0] = EPSILON_SYMBOL;

    // 3) Map EPSILON_SYMBOL -> 0 and set symbol_count = 1 
    // (epsilon is the first symbol in the alphabet)
    a.char_to_col[(unsigned char)EPSILON_SYMBOL] = 0;
    a.symbol_count = 1;
    return a;
}

/** 
 * @brief Function to add a symbol to the alphabet.
 * - Rejects the EPSILON_SYMBOL and duplicates.
 * - Appends the symbol to the symbols array and updates char_to_col.
 */
void add_symbol(alphabet *a, char symbol) {
    // 1) Reject EPSILON_SYMBOL
    if (symbol == EPSILON_SYMBOL) {
        return;
    }

   // 1.1) Reject duplicates via char_to_col lookup.
    if ( a->char_to_col[(unsigned char)symbol] != -1){
        return;
    }

    // 2) Append symbol into symbols[symbol_count].
    a->symbols[a->symbol_count] = symbol;

    // 3) Update char_to_col and increment symbol_count.
    a->char_to_col[(unsigned char)symbol] = a->symbol_count;
    a->symbol_count++;
}

/**
 * @brief Function to create a new states manager for NFA construction.
 * Initializes the next_id, states_count, transitions_count, and manager_alphabet
 * with default values.
 */
states_manager new_states_manager() {
    states_manager manager = {0};

    // Intialize the struct with default values.
    manager.next_id = 0; // Start state IDs from 0
    manager.states_count = 0;
    manager.transitions_count = 0;
    manager.manager_alphabet = new_alphabet();
    return manager;
}

/**
 * @brief Function to create a new state in the states manager.
 * - Checks if the maximum number of states has been reached.
 * - Generates a new state ID, adds it to the list of states, and updates counters.
 * - Returns the new state ID or an error value if the limit is exceeded.
 */
uint8_t new_state(states_manager *manager) {
    // 1) Check if maximum number of states has been reached.
    if (manager->states_count >= MAX_STATES) {
        return 255; // Error value (since uint8_t)
    }

    // 2) Generate new state ID to assign to the new state ID
    //    and add it to the states array.
    uint8_t new_id = manager->next_id;

    manager->states[manager->states_count] = new_id;

    // 3) Advance conters for next state ID and states count
    manager->next_id++;
    manager->states_count++;

    return new_id;
}

void add_transition(states_manager *manager, uint8_t from_state, char symbol, uint8_t to_state) {
    // 1) Check if reached maximum transitions
    if (manager->transitions_count >= MAX_STATES * MAX_STATES) {
        return; // No space left
    }

    // 2) Ensure symbol exists in alphabet
    add_symbol(&manager->manager_alphabet, symbol);

    // 3)Append transition to transitions array
    manager->transitions[manager->transitions_count].from_state = from_state;
    manager->transitions[manager->transitions_count].symbol = symbol;
    manager->transitions[manager->transitions_count].to_state = to_state;

    manager->transitions_count++;
}

///////////////////////////////////////////////////
// NFA construction functions:
// 1. Symbol NFA            a
// 2. Concatenation NFA     .
// 3. Union NFA             |
// 4. Positive Closure NFA  +
// 5. Kleene Closure NFA    *
// 6. Optional NFA          ?
///////////////////////////////////////////////////

/** 
 * @brief Function to create a new NFA for a single symbol.
 * - Creates a new start and end state.
 * - Adds a transition from the start state to the end state on the given symbol.
 * - Returns the NFA fragment.
 */
t_nfa symbol_nfa(states_manager *manager, char symbol) {
    t_nfa result = {0};

    // 1) Create start and end states
    uint8_t start = new_state(manager);
    uint8_t end = new_state(manager);

    // 2) Add transition start --symbol--> end
    add_transition(manager, start, symbol, end);

    // 3) Set NFA fragment
    result.start = start;
    result.end = end;

    return result;
}

/**
 * @brief Function to create a new NFA for the concatenation of two NFAs.
 * - The start state is the start of the first NFA.
 * - The end state is the end of the second NFA.
 * - An epsilon transition is added from the end of the first NFA to the start of
 */
t_nfa concat_nfa(states_manager *manager, t_nfa *a, t_nfa *b) {
    t_nfa result = {0};

    // 1) Start state is the start of first NFA
    result.start = a->start;

    // 2) End state is the end of second NFA
    result.end = b->end;

    // 3) Add epsilon transition from a->end to b->start
    add_transition(manager, a->end, EPSILON_SYMBOL, b->start);

    return result;
}


/**
 * @brief Function to create a new NFA for the union of two NFAs.
 * - A new start state and a new end state are created.
 * - Epsilon transitions are added from the new start state to the start states of both NFAs.
 * - Epsilon transitions are added from the end states of both NFAs to the new end state.
 * - Returns the resulting NFA fragment.
 */
t_nfa union_nfa(states_manager *manager, t_nfa *a, t_nfa *b) {
    t_nfa result;

    // 1) Create new start and end states
    uint8_t start = new_state(manager);
    uint8_t end = new_state(manager);

    // 2) Epsilon transitions from new start to both NFAs
    add_transition(manager, start, EPSILON_SYMBOL, a->start);
    add_transition(manager, start, EPSILON_SYMBOL, b->start);

    // 3) Epsilon transitions from both NFAs to new end
    add_transition(manager, a->end, EPSILON_SYMBOL, end);
    add_transition(manager, b->end, EPSILON_SYMBOL, end);

    result.start = start;
    result.end = end;

    return result;
}

/** 
 * @brief Function to create a new NFA for the positive closure of an NFA.
 * - A new start state and a new end state are created.
 * - An epsilon transition is added from the new start state to the start of the NFA.
 * - An epsilon transition is added from the end of the NFA back to its start (loop).
 * - An epsilon transition is added from the end of the NFA to the new end state (exit).
 * - Returns the resulting NFA fragment.
 */
t_nfa positive_closure_nfa(states_manager *manager, t_nfa *a) {
    t_nfa result = {0};

    // 1) Create new start and end
    uint8_t start = new_state(manager);
    uint8_t end = new_state(manager);

    // 2) Epsilon start -> a.start
    add_transition(manager, start, EPSILON_SYMBOL, a->start);

    // 3) Loop: a.end -> a.start
    add_transition(manager, a->end, EPSILON_SYMBOL, a->start);

    // 4) Exit: a.end -> new end
    add_transition(manager, a->end, EPSILON_SYMBOL, end);

    result.start = start;
    result.end = end;

    return result;
}


/**
 * @brief Function to create a new NFA for the Kleene closure of an NFA.
 * - A new start state and a new end state are created.
 * - An epsilon transition is added from the new start state to the start of the NFA.
 * - An epsilon transition is added from the new start state to the new end state (allowing empty string).
 * - An epsilon transition is added from the end of the NFA back to its start (loop).
 * - An epsilon transition is added from the end of the NFA to the new end state (exit).
 * - Returns the resulting NFA fragment.
 */
t_nfa kleene_closure_nfa(states_manager *manager, t_nfa *a) {
    t_nfa result;

    // 1) Create new start and end states
    uint8_t start = new_state(manager);
    uint8_t end   = new_state(manager);

    // 2) Epsilon start -> a.start
    add_transition(manager, start, EPSILON_SYMBOL, a->start);

    // 3) Epsilon start -> end  (allow empty string)
    add_transition(manager, start, EPSILON_SYMBOL, end);

    // 4) Loop: a.end -> a.start
    add_transition(manager, a->end, EPSILON_SYMBOL, a->start);

    // 5) Exit: a.end -> end
    add_transition(manager, a->end, EPSILON_SYMBOL, end);

    result.start = start;
    result.end   = end;

    return result;
}


/** 
 * @brief Function to create a new NFA for the optional operator applied to an NFA.
 * - A new start state and a new end state are created.
 * - An epsilon transition is added from the new start state to the start of the NFA.
 * - An epsilon transition is added from the new start state to the new end state (allowing empty string).
 * - An epsilon transition is added from the end of the NFA to the new end state (exit).
 * - Returns the resulting NFA fragment.
 */
t_nfa optional_nfa(states_manager *manager, t_nfa *a) {
    t_nfa result = {0};

    // 1) Create new start and end states
    uint8_t start = new_state(manager);
    uint8_t end   = new_state(manager);

    // 2) Epsilon start -> a.start
    add_transition(manager, start, EPSILON_SYMBOL, a->start);

    // 3) Epsilon start -> end (skip)
    add_transition(manager, start, EPSILON_SYMBOL, end);

    // 4) Epsilon a.end -> end
    add_transition(manager, a->end, EPSILON_SYMBOL, end);

    result.start = start;
    result.end   = end;

    return result;
}

// End of NFA construction functions

// Forward declarations for internal helper functions
static void epsilon_closure(nfa *automaton, uint8_t state);
static void calculate_epsilon_closure(nfa *automaton);
static nfa  t_nfa_to_nfa(t_nfa temp_nfa, states_manager manager);


/**
 * @brief FUnction to compute the epsilon closure of a given state in the NFA.
 * - Uses an iterative depth-first search (DFS) approach with a work stack to avoid recursion.
 * - The closure is represented as a bitset (uint64_t) where each bit corresponds to a state.
 * - The function updates the epsilon_closure_cache for the given state with the
 */
static void epsilon_closure(nfa *automaton, uint8_t state) {
    // Epsilon is always at column 0 (see new_alphabet())
    int epsilon_col = automaton->nfa_alphabet.char_to_col[(unsigned char)EPSILON_SYMBOL];

    uint64_t closure = (uint64_t)1 << state;   // every state is in its own closure

    // Iterative DFS via a small stack (at most MAX_STATES deep)
    uint8_t work_stack[MAX_STATES];
    int     top = 0;
    work_stack[top++] = state;

    while (top > 0) {
        uint8_t  cur     = work_stack[--top];
        uint64_t targets = automaton->transitions[cur][epsilon_col];

        // Isolate states reachable but not yet in the closure
        uint64_t fresh = targets & ~closure;
        while (fresh) {
            // Extract the lowest set bit
            uint8_t next = (uint8_t)__builtin_ctzll(fresh);
            closure     |= (uint64_t)1 << next;
            work_stack[top++] = next;
            fresh &= fresh - 1;   // clear lowest set bit
        }
    }

    automaton->epsilon_closure_cache[state] = closure;
}


/**
 * @brief Precomputes the epsilon closure for all states in the NFA and stores it in a cache.
*/
static void calculate_epsilon_closure(nfa *automaton) {
    automaton->epsilon_closure_cache = calloc(automaton->states, sizeof(uint64_t));

    for (uint8_t s = 0; s < automaton->states; s++) {
        epsilon_closure(automaton, s);
    }
}


/**
 * @brief Function to convert a temporary NFA (t_nfa) and its associated states manager 
 * into the final NFA structure.
 * - Copies metadata (start state, accept states, number of states, and alphabet) from the temporary structures.
 * - Allocates and populates the transition table based on the transitions recorded in the states manager.
 * - Pre-computes the epsilon closures for all states in the resulting NFA.
 * - Returns the fully constructed NFA ready for matching.
 */
static nfa t_nfa_to_nfa(t_nfa temp_nfa, states_manager manager){
    nfa result = {0};

    // 1) Metadata
    result.start_state  = temp_nfa.start;
    result.accept_states = (uint64_t)1 << temp_nfa.end;   // single accept state
    result.states        = manager.states_count;
    result.nfa_alphabet  = manager.manager_alphabet;

    // 2) Allocate transition table: result.transitions[state][symbol_col]
    int sym_count = manager.manager_alphabet.symbol_count;

    result.transitions = calloc(result.states, sizeof(uint64_t *));
    for (int s = 0; s < result.states; s++) {
        result.transitions[s] = calloc(sym_count, sizeof(uint64_t));
    }

    // 3) Populate transitions from manager records
    for (int i = 0; i < manager.transitions_count; i++) {
        t_transition tr  = manager.transitions[i];
        int          col = manager.manager_alphabet.char_to_col[(unsigned char)tr.symbol];
        if (col >= 0) {
            result.transitions[tr.from_state][col] |= (uint64_t)1 << tr.to_state;
        }
    }

    // 4) Pre-compute epsilon closures
    calculate_epsilon_closure(&result);

    return result;
}


/**
 * @brief Function to convert a parsed regular expression (in postfix form) into an NFA.
 * - Uses a stack-based approach to construct the NFA from the postfix expression.
 * - For each item in the regex:   
 *    - If it's a character, creates a symbol NFA and pushes it onto the stack.
 *    - If it's an operator, pops the necessary operands from the stack, applies the operator to create a new NFA fragment, and pushes the result back on the stack. 
 *    - After processing all items, validates that exactly one NFA fragment remains on the stack, which represents the final NFA for the entire regex.
 * - Returns the constructed NFA.
 */
nfa regex_to_nfa(const Regex r){
    states_manager manager = new_states_manager();
    t_nfa stack[MAX_STATES];
    int   stack_top = 0;

    for (int i = 0; i < r.size; i++) {
        Item item = r.items[i];

        switch (item.type) {

            case ITEM_CHAR: {
                // Operand: push a single-symbol NFA fragment
                add_symbol(&manager.manager_alphabet, item.value);
                t_nfa frag = symbol_nfa(&manager, item.value);
                stack[stack_top++] = frag;
                break;
            }

            case ITEM_CONCAT: {
                // Binary: pop b then a, push concat(a, b)
                t_nfa b = stack[--stack_top];
                t_nfa a = stack[--stack_top];
                stack[stack_top++] = concat_nfa(&manager, &a, &b);
                break;
            }

            case ITEM_OR: {
                // Binary: pop b then a, push union(a, b)
                t_nfa b = stack[--stack_top];
                t_nfa a = stack[--stack_top];
                stack[stack_top++] = union_nfa(&manager, &a, &b);
                break;
            }

            case ITEM_STAR: {
                // Unary: pop a, push kleene*(a)
                t_nfa a = stack[--stack_top];
                stack[stack_top++] = kleene_closure_nfa(&manager, &a);
                break;
            }

            case ITEM_PLUS: {
                // Unary: pop a, push positive+(a)
                t_nfa a = stack[--stack_top];
                stack[stack_top++] = positive_closure_nfa(&manager, &a);
                break;
            }

            case ITEM_QUESTION: {
                // Unary: pop a, push optional?(a)
                t_nfa a = stack[--stack_top];
                stack[stack_top++] = optional_nfa(&manager, &a);
                break;
            }

            default:
                break;   // Ignore LPAREN/RPAREN (shouldn't appear in postfix)
        }
    }

    // Validate: exactly one fragment must remain
    if (stack_top != 1) {
        nfa empty = {0};
        return empty;
    }

    return t_nfa_to_nfa(stack[0], manager);
}





/**
 * @brief SFUnction to match an input string against an NFA.
 * - Starts from the epsilon closure of the start state.
 * - For each input symbol, computes the set of reachable states and then expands them with their epsilon closures.
 * - After processing all input symbols, checks if any of the current states are accept states.
 * - Returns true if the input is accepted by the NFA, false otherwise.
 */
bool match_nfa(nfa automaton, const char *input, size_t input_length){
    // 1) Start from the epsilon closure of the start state
    uint64_t current = automaton.epsilon_closure_cache[automaton.start_state];

    for (size_t i = 0; i < input_length; i++) {
        char c   = input[i];
        int  col = automaton.nfa_alphabet.char_to_col[(unsigned char)c];

        // Symbol not in alphabet → immediate reject
        if (col < 0) {
            return false;
        }

        // 2a) move(current, c): collect all states reachable on symbol c
        uint64_t move_set = 0;
        uint64_t tmp      = current;
        while (tmp) {
            uint8_t s  = (uint8_t)__builtin_ctzll(tmp);
            move_set  |= automaton.transitions[s][col];
            tmp       &= tmp - 1;
        }

        // Early reject if no states are reachable
        if (move_set == 0) {
            return false;
        }

        // 2b) Expand each reached state with its epsilon closure
        current = 0;
        tmp     = move_set;
        while (tmp) {
            uint8_t s  = (uint8_t)__builtin_ctzll(tmp);
            current   |= automaton.epsilon_closure_cache[s];
            tmp       &= tmp - 1;
        }
    }

    // 3) Accept if any current state is an accept state
    return (current & automaton.accept_states) != 0;
}
#ifndef NFA_H
#define NFA_H
#include "regex.h"


typedef struct {
    // TODO: definir estructura del NFA
    int placeholder;
} nfa;

nfa regex_to_nfa(Regex r);

int match_nfa(nfa n, const char *str, int len);

#endif
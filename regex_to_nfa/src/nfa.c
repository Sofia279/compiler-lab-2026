#include "nfa.h"
#include "regex.h"

nfa regex_to_nfa(Regex r)
{
    nfa nfa;
    nfa.placeholder = 0;
    (void)r;  // evita warning de parámetro no usado
    return nfa;
}

int match_nfa(nfa n, const char *str, int len)
{
    (void)n;    // evita warning
    (void)str;
    (void)len;
    return 0;
}
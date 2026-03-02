# Practical Assignment I: From Regular Expressions to NFA
Implementation of a regex engine that converts regular expressions into 
a Nondeterministic Finite Automaton (NFA) and uses it to match input strings.

## Structure

- `src/main.c`        - Entry point: argument parsing and I/O handling
- `src/regex.c`       - Regex parsing pipeline (tokenizer, explicit concatenation, Shunting Yard)
- `src/regex.h`       - Regex types and interface
- `src/nfa.c`         - NFA construction (Thompson's algorithm) and simulation
- `src/nfa.h`         - NFA types and interface
- `test/test_regex.c` - Unit tests for regex parsing functions


## Compilation and Execution

### Validator (Docker)
```bash
docker build -t regex_to_nfa_validator .
docker run --rm regex_to_nfa_validator
```

### Usage

The binary accepts two flags. In both cases the regex is always the first line read from `stdin`.

| Flag | Behavior |
|------|----------|
| `-r` | Reads a regex from `stdin` and prints its postfix form with explicit concatenation |
| `-t` | Reads a regex, then matches each subsequent line, printing `1` if accepted or `0` otherwise |


#### Examples
```bash
# Compile the program
gcc src/main.c src/nfa.c src/regex.c -o regex_to_nfa

echo "(ab)*" | ./regex_to_nfa -r
ab.*

echo "(ab?)|(c*a)(d+c)" | ./regex_to_nfa -r
ab?.c*a.d+c..|

printf '%s\n' "(ab)*" "ab" "aba" "abab" | ./regex_to_nfa -t
101

printf '%s\n' "(ab?)|(c*a)(d+c)" "ab" "a" "abab" "ccccadc" "" | ./regex_to_nfa -t
11010
```

## Tests
```bash
# Compile and run unit tests
gcc test/test_regex.c src/regex.c -I src -o test/test_regex
./test/test_regex
```

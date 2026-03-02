#ifndef REGEX_H
#define REGEX_H

#include <stdlib.h>

/**
 * Structure for a regular expression item, which can be an operator or a character.
 * The type field indicates the type of the item (e.g., operator or character), and the value 
 * field holds the character value for character items.
 */

// Operator characters
#define SYMBOL_OR_CHAR '|'
#define SYMBOL_CONCAT_CHAR '.'
#define SYMBOL_QUESTION_CHAR '?'
#define SYMBOL_PLUS_CHAR '+'
#define SYMBOL_STAR_CHAR '*'

// Hierarchy characters
#define SYMBOL_LPAREN_CHAR '('
#define SYMBOL_RPAREN_CHAR ')'

// Special characters
#define ESCAPE_SYMBOL '\\'
#define EPSILON_SYMBOL 256 // A value outside the ASCII range to represent epsilon transitions

/**
 * @brief Symbol types in precedence order (operators only).
 * SYMBOL_EPSILON is intentionally outside the precedence range.
 */
typedef enum {
    ITEM_OR,          // |  — lowest precedence
    ITEM_CONCAT,      // .
    ITEM_QUESTION,    // ?
    ITEM_PLUS,        // +
    ITEM_STAR,        // *  — highest among operators
    ITEM_CHAR,        // a-z, 0-9, etc.
    ITEM_LPAREN,      // (
    ITEM_RPAREN       // )
} ItemType;


typedef struct {
    ItemType type;
    char value; 
} Item;

typedef struct {
    Item *items;
    int size;
} Regex;


// Helper functions
Item new_item(char value, ItemType type);
ItemType get_item_type(char c);
void  free_regex(Regex *r);

// Helper functions for parsing
Item *itemize_regex(const char *regex_str, int *out_size);
Item *implicit_to_explicit_concatenation(const Item *items, int size, int *out_size);
Item *shunting_yard(const Item *items, int size, int *out_size);

/* Parsing */
Regex parse_regex(const char *regex_str);

#endif
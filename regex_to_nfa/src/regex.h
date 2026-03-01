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
#define EPSILON_VALUE 256 // A value outside the ASCII range to represent epsilon transitions

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
    int value;  // holds the character
} Item;

typedef struct {
    Item *items;
    int size;
} Regex;


/* Parsing */

Regex parse_regex(const char *regex_str);
Regex to_postfix(Regex r);

/**
 * @brief Function to free the memory allocated for a regex struct, including its items array.
 * This function should be called when the regex struct is no longer needed to avoid memory leaks.
 * @param r The regex struct to free
 */
void  free_regex(Regex r);



/*
 * @brief Helper function to create a new item with the given value and operator status.
 *
 * @param value The character value of the item
 * @param type An item_type indicating the type of the operator
 * @return An item struct with the specified value and operator status
 */
Item new_item(char value, ItemType type);

/* Matching */
int regex_match(const char *pattern, const char *text);

#endif
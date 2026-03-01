#include "regex.h"
#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////
// Implementation of regex parsing and conversion to postfix notation //
////////////////////////////////////////////////////////////////////////

// Helper functions

/**
 * @brief Helper function to create a new item with the given value and operator status.
 *
 * @param value The character value of the item
 * @param type An item_type indicating the type of the operator
 * @return An item struct with the specified value and operator status
 */

Item new_item(char value, ItemType type) {
    Item s;
    s.type = type;
    s.value = value;
    return s;
}

/**
 * @brief Helper function to determine the item type based on a character.
 *
 * @param c The character to evaluate
 * @return The corresponding ItemType for the character
 */
ItemType get_item_type(char c) {
    switch (c) {
        case SYMBOL_OR_CHAR: 
            return ITEM_OR;
        case SYMBOL_CONCAT_CHAR: 
            return ITEM_CONCAT;
        case SYMBOL_QUESTION_CHAR: 
            return ITEM_QUESTION;
        case SYMBOL_PLUS_CHAR: 
            return ITEM_PLUS;
        case SYMBOL_STAR_CHAR: 
            return ITEM_STAR;
        case SYMBOL_LPAREN_CHAR: 
            return ITEM_LPAREN;
        case SYMBOL_RPAREN_CHAR: 
            return ITEM_RPAREN;
        default: 
            return ITEM_CHAR;  // Default to character type for any other char
    }
}

/**
 * @brief Helper function to free the memory allocated for a regex struct, including its items array.
 * This function should be called when the regex struct is no longer needed to avoid memory leaks.
 * @param r The regex struct to free
 */

void free_regex(Regex r) {
    if(r.items != NULL) {
        free(r.items);
    }
}

// Main functions

Regex parse_regex(const char *regex_str) {
    Regex r;
    // ....
    return r;
}

// Regex to_postfix(Regex r){
//     return Regex();
// }

Regex to_postfix(Regex r) {
    Regex postfix;

    return postfix;
}
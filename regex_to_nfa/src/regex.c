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

void free_regex(Regex *r) {
    if (r == NULL){
        return; 
    }
    if(r->items != NULL) {
        free(r->items);
        r->items = NULL;
        r->size = 0;
    }
}


/**
 * @brief Helper function for implicit_to_explicit_concatenation()
 * to determine if a concatenation operator is needed between two items.
 * The cases where concatenation is needed are:
 * 1. If the left item is a char, *, +, ?, or a closing parenthesis ")"
 * AND
 * 2. If the right item is a char or an opening parenthesis "("
 * @param left The item type of the left item
 * @param right The item type of the right item
 * @return 1 if a concatenation operator is needed, 0 otherwise
 */
static int needs_concat(ItemType left, ItemType right) {
    int left_ok  = (left == ITEM_CHAR ||
                    left == ITEM_STAR ||
                    left == ITEM_PLUS ||
                    left == ITEM_QUESTION ||
                    left == ITEM_RPAREN);

    int right_ok = (right == ITEM_CHAR ||
                    right == ITEM_LPAREN);

    return left_ok && right_ok;
}

// Helper functions for parsing

/**
 * @brief Function to tokenize a regular expression string into an array of items.
 *
 * @param regex_str The input regular expression as a string
 * @param out_size Pointer to an integer where the size of the output item array will be stored
 * @return A pointer to an array of items representing the tokenized regular expression
 */
Item *itemize_regex(const char *regex_str, int *out_size) {
    
    if (regex_str == NULL) {
        if (out_size) *out_size = 0;
        return NULL;
    }
    // First pass to count the number of items, accounting for escape characters
    int count = 0;
    for (int i = 0; regex_str[i] != '\0'; i++) {
        if (regex_str[i] == ESCAPE_SYMBOL) {
            i++;
        }
        count++;
    }

    if (out_size){
        *out_size = count;
    }

    Item *items = malloc(count * sizeof(Item));

    // Fill the items aaray
    int j = 0;
    for (int i = 0; regex_str[i] != '\0'; i++) {
        char c = regex_str[i];
        if (c == ESCAPE_SYMBOL) {
            // skip the escape character and treat the next character as a literal
            i++;
            items[j++] = new_item(regex_str[i], ITEM_CHAR);
        } else {
            items[j++] = new_item(c, get_item_type(c));
        }
    }

    return items;
}

/** 
 * @brief Function to convert an array of items with implicit concatenation into an array with explicit concatenation operators.
 * For example, the regex "ab" would be converted to "a.b" by inserting a concatenation operator between 'a' and 'b'.
 * @param items The input array of items with implicit concatenation
 * @param size The size of the input items array
 * @param out_size Pointer to an integer where the size of the output item array will be stored
 * @return A pointer to a new array of items with explicit concatenation operators inserted where needed  
 */

Item *implicit_to_explicit_concatenation(const Item *items, int size, int *out_size) {
    if (items == NULL || size <= 0) {
        if (out_size) *out_size = 0;
        return NULL;
    }

    // The orignal size plus the number of concatenation operators that need to be inserted
    int new_size = size;
    for (int i = 0; i < size - 1; i++) {
        if (needs_concat(items[i].type, items[i + 1].type)) {
            new_size++;
        }
    }

    // Store the new size in the output parameter if provided
    if (out_size) {
        *out_size = new_size;
    }

    Item *new_items = malloc(new_size * sizeof(Item));

    //Iterazion to insert concatenation operators where needed
    int j = 0;
    for (int i = 0; i < size - 1; i++) {
        new_items[j++] = items[i];
        if (needs_concat(items[i].type, items[i + 1].type)) {
            new_items[j++] = new_item(SYMBOL_CONCAT_CHAR, ITEM_CONCAT);
        }
    }
    new_items[j++] = items[size - 1];
    return new_items;
}


/**
 * @brief Function to convert an array of items in infix notation to postfix notation using the shunting yard algorithm.
 * @param items The input array of items in infix notation
 * @param size The size of the input items array
 * @param out_size Pointer to an integer where the size of the output item array will be stored
 * @return A pointer to a new array of items in postfix notation, or NULL if there is a mismatched parenthesis
 * Iteration example: a+.c.d (after explicit concatenation insertion)
 *  1. stack: []    queue: [a]
 *  2. stack: [+]   queue: [a]
 *  3. stack: [.]   queue: [a,+] -> pop '+' to output because it has higher precedence than '.' and push '.' to stack
 *  4. stack: [.]   queue: [a,+,c]
 *  5. stack: [.]   queue: [a,+,c,.] -> pop '.' to output because it has equal precedence to '.' and push '.' to stack
 *  6. stack: [.]   queue: [a,+,c,.,d]
 *  7. stack: []    queue: [a,+,c,.,d,.] 
 */
Item *shunting_yard(const Item *items, int size, int *out_size){

    if (items == NULL || size <= 0) {
        if (out_size) *out_size = 0;
        return NULL;
    }

    Item *stack  = malloc(size * sizeof(Item)); // Operator stack
    Item *queue = malloc(size * sizeof(Item)); // Queue for output items

    int out_top = 0;
    int stack_top = 0;

    for (int i = 0; i < size; i++) {
        Item c = items[i]; // forward(c) -> current item

        // 1) Push operands directly to output queue (lines 7-8)
        if (c.type == ITEM_CHAR) {
            queue[out_top++] = c;

        // 2) Push '(' to stack (lines 10-11)
        } else if (c.type == ITEM_LPAREN) {
            stack[stack_top++] = c;

        // 3) Pop operators to output until '(' (lines 13-17)
        } else if (c.type == ITEM_RPAREN) {
            // In case of () or mismatched parentheses, we need to check if there is a '(' in the stack before popping
            if (stack_top > 0 && stack[stack_top - 1].type == ITEM_LPAREN) {
                free(stack); free(queue);
                if (out_size) *out_size = 0;
                return NULL;
            }

            while (stack_top > 0 && stack[stack_top - 1].type != ITEM_LPAREN) {
                queue[out_top++] = stack[--stack_top];  // line 15
            }

            if (stack_top == 0) {  // mismatched ')'
                free(queue);
                free(stack);
                if (out_size) *out_size = 0;
                return NULL;
            }
            stack_top--;  // line 17 -> stack.pop() — discard '('
        } else {
            // 4) For operators, pop higher or equal precedence to output
            while (stack_top > 0 &&
                   stack[stack_top - 1].type != ITEM_LPAREN &&
                   stack[stack_top - 1].type >= c.type) {
                queue[out_top++] = stack[--stack_top];
            }
            stack[stack_top++] = c; 
        }
    }

    // Drain remaining operators to output
    while (stack_top > 0) {
        if (stack[stack_top - 1].type == ITEM_LPAREN) {
            free(queue);
            free(stack);
            if (out_size) *out_size = 0;
            return NULL;
        }
        queue[out_top++] = stack[--stack_top];
    }

    if (out_size) {
        *out_size = out_top;
    }

    // Return comact heap-allocated array of postfix array
    free(stack);
    return queue;
}


/**
 * @brief Function to parse a regular expression string and convert it into a regex struct.
 * 1.- Itemizes the regex string into an array of items
 * 2.- Converts implicit concatenation to explicit concatenation
 * 3.- Converts infix notation to postfix notation using the shunting yard algorithm.
 *
 * @param regex_str The input regular expression as a string
 * @return A regex struct containing the size of the postfix items and the array of postfix items
 */
Regex parse_regex(const char *regex_str) {
    Regex r;

    // 1. Tokenize the regex string into items
    int itemized_size = 0;
    Item *itemized = itemize_regex(regex_str, &itemized_size);
    
    if (itemized == NULL) {
        r.items = NULL;
        r.size = 0;
        return r;
    }

    // 2. Insert explicit concatenation operators
    int explicit_size = 0;
    Item *explicit = implicit_to_explicit_concatenation(itemized, itemized_size, &explicit_size);
    free(itemized);

    if (explicit == NULL) {
        r.items = NULL;
        r.size = 0;
        return r;
    }

    // 3. Convert to postfix notation using shunting yard
    int postfix_size = 0;
    Item *postfix = shunting_yard(explicit, explicit_size, &postfix_size);
    free(explicit);

    if (postfix == NULL) {
        r.items = NULL;
        r.size = 0;
        return r;
    }

    r.items = postfix;
    r.size  = postfix_size;
    return r;
}
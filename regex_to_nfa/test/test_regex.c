#include <stdio.h>
#include <assert.h>
#include "../src/regex.h"

// Helper function to print a regex for debugging
void print_regex(Regex r) {
    printf("Regex (size: %d): ", r.size);
    for (int i = 0; i < r.size; i++) {
        Item item = r.items[i];
        switch (item.type) {
            case ITEM_OR: 
                printf("| ");
                break;
            case ITEM_CONCAT: 
                printf(". ");
                break;
            case ITEM_QUESTION: 
                printf("? ");
                break;
            case ITEM_PLUS: 
                printf("+ ");
                break;
            case ITEM_STAR: 
                printf("* ");
                break;
            case ITEM_CHAR: 
                printf("%c ", item.value);
                break;
            case ITEM_LPAREN: 
                printf("( ");
                break;
            case ITEM_RPAREN: 
                printf(") ");
                break;
        }
    }
    printf("\n");
}

///////////////////////////////////////////
// Test cases for itemize_regex function //
///////////////////////////////////////////

void test_simple_chars() {
    int size;
    Item *items = itemize_regex("ab", &size);
    assert(size == 2);
    assert(items[0].type == ITEM_CHAR && items[0].value == 'a');
    assert(items[1].type == ITEM_CHAR && items[1].value == 'b');
    free(items);
    printf("[OK] test_simple_chars\n");

    items = itemize_regex("abcdeaecdf", &size);
    assert(size == 10);
    for (int i = 0; i < size; i++) {
        assert(items[i].type == ITEM_CHAR);
    }
    free(items);
    printf("[OK] test_simple_chars with 10 chars\n");
}

void test_operators() {
    int size;
    Item *items = itemize_regex("a|b*", &size);
    assert(size == 4);
    assert(items[0].type == ITEM_CHAR);
    assert(items[1].type == ITEM_OR);
    assert(items[2].type == ITEM_CHAR);
    assert(items[3].type == ITEM_STAR);
    free(items);
    printf("[OK] test_operators\n");
}

void test_escape() {
    int size;
    Item *items = itemize_regex("a\\|b", &size); // The regex string is "a|b" with the '|' character escaped
    assert(size == 3);
    assert(items[0].type == ITEM_CHAR && items[0].value == 'a');
    assert(items[1].type == ITEM_CHAR && items[1].value == '|');
    assert(items[2].type == ITEM_CHAR && items[2].value == 'b');
    free(items);
    printf("[OK] test_escape\n");
}

void test_empty() {
    int size;
    Item *items = itemize_regex("", &size);
    assert(size == 0);
    free(items);
    printf("[OK] test_empty\n");
}

void test_parentheses() {
    int size;
    Item *items = itemize_regex("(a|b)*", &size);
    assert(size == 6);
    assert(items[0].type == ITEM_LPAREN);
    assert(items[1].type == ITEM_CHAR && items[1].value == 'a');
    assert(items[2].type == ITEM_OR);
    assert(items[3].type == ITEM_CHAR && items[3].value == 'b');
    assert(items[4].type == ITEM_RPAREN);
    assert(items[5].type == ITEM_STAR);
    free(items);
    printf("[OK] test_parentheses\n");
}


/////////////////////////////////////////////////////////////
// Helper for implicit_to_explicit_concatenation tests     //
/////////////////////////////////////////////////////////////

/**
 * @brief Helper function to check the output of implicit_to_explicit_concatenation against expected results.
 */
static void check_concat(const char *input, const char *expected_chars, const ItemType *expected_types,
                          int expected_size, const char *label) {

    int size, out_size;
    Item *items  = itemize_regex(input, &size);
    Item *result = implicit_to_explicit_concatenation(items, size, &out_size);

    // Check size and content of the result against expected values
    assert(out_size == expected_size);
    for (int i = 0; i < out_size; i++) {
        assert(result[i].type  == expected_types[i]);
        assert(result[i].value == expected_chars[i]);
    }

    free(items);
    free(result);
    printf("[OK] %s\n", label);
}

/////////////////////////////////////////////////////////////
// Test cases for implicit_to_explicit_concatenation       //
/////////////////////////////////////////////////////////////

// Case 1: a|b -> a|b  (no concat inserted)
void test_concat_alternation() {
    const char ec[] = {'a', '|', 'b'};
    const ItemType et[] = {ITEM_CHAR, ITEM_OR, ITEM_CHAR};
    check_concat("a|b", ec, et, 3, "concat: a|b -> a|b");
}

// Case 2: a*b -> a*.b
void test_concat_star_char() {
    const char ec[] = {'a', '*', '.', 'b'};
    const ItemType et[] = {ITEM_CHAR, ITEM_STAR, ITEM_CONCAT, ITEM_CHAR};
    check_concat("a*b", ec, et, 4, "concat: a*b -> a*.b");
}

// Case 3: ab -> a.b
void test_concat_two_chars() {
    const char ec[] = {'a', '.', 'b'};
    const ItemType et[] = {ITEM_CHAR, ITEM_CONCAT, ITEM_CHAR};
    check_concat("ab", ec, et, 3, "concat: ab -> a.b");
}

// Case 4: (a)|(b) -> (a)|(b)  (no concat)
void test_concat_groups_alternation() {
    const char ec[] = {'(', 'a', ')', '|', '(', 'b', ')'};
    const ItemType et[] = {ITEM_LPAREN, ITEM_CHAR, ITEM_RPAREN,
                             ITEM_OR,
                             ITEM_LPAREN, ITEM_CHAR, ITEM_RPAREN};
    check_concat("(a)|(b)", ec, et, 7, "concat: (a)|(b) -> (a)|(b)");
}

// Case 5: (a)(b) -> (a).(b)
void test_concat_two_groups() {
    const char ec[] = {'(', 'a', ')', '.', '(', 'b', ')'};
    const ItemType et[] = {ITEM_LPAREN, ITEM_CHAR, ITEM_RPAREN,
                             ITEM_CONCAT,
                             ITEM_LPAREN, ITEM_CHAR, ITEM_RPAREN};
    check_concat("(a)(b)", ec, et, 7, "concat: (a)(b) -> (a).(b)");
}

// Case 6: (a)*b -> (a)*.b
void test_concat_group_star_char() {
    const char ec[] = {'(', 'a', ')', '*', '.', 'b'};
    const ItemType et[] = {ITEM_LPAREN, ITEM_CHAR, ITEM_RPAREN,
                             ITEM_STAR, ITEM_CONCAT, ITEM_CHAR};
    check_concat("(a)*b", ec, et, 6, "concat: (a)*b -> (a)*.b");
}

// Extra: abc -> a.b.c  (two inserts)
void test_concat_three_chars() {
    const char ec[] = {'a', '.', 'b', '.', 'c'};
    const ItemType et[] = {ITEM_CHAR, ITEM_CONCAT, ITEM_CHAR, ITEM_CONCAT, ITEM_CHAR};
    check_concat("abc", ec, et, 5, "concat: abc -> a.b.c");
}

// Extra: a?b -> a?.b
void test_concat_question_char() {
    const char ec[] = {'a', '?', '.', 'b'};
    const ItemType et[] = {ITEM_CHAR, ITEM_QUESTION, ITEM_CONCAT, ITEM_CHAR};
    check_concat("a?b", ec, et, 4, "concat: a?b -> a?.b");
}

// Extra: a+b -> a+.b
void test_concat_plus_char() {
    const char ec[] = {'a', '+', '.', 'b'};
    const ItemType et[] = {ITEM_CHAR, ITEM_PLUS, ITEM_CONCAT, ITEM_CHAR};
    check_concat("a+b", ec, et, 4, "concat: a+b -> a+.b");
}

// Extra: (a*) -> (a*) 
void test_concat_no_insert_inside_group() {
    const char ec[] = {'(', 'a', '*', ')'};
    const ItemType et[] = {ITEM_LPAREN, ITEM_CHAR, ITEM_STAR, ITEM_RPAREN};
    check_concat("(a*)", ec, et, 4, "concat: (a*) -> (a*)");
}


/////////////////////////////////////////////////////////////
// Helper for shunting_yard tests                          //
/////////////////////////////////////////////////////////////

static void check_postfix(const char *input,
                           const char *expected_chars,
                           const ItemType *expected_types,
                           int expected_size,
                           const char *label) {
    int size, concat_size, out_size;
    Item *items   = itemize_regex(input, &size);
    Item *concat  = implicit_to_explicit_concatenation(items, size, &concat_size);
    Item *result  = shunting_yard(concat, concat_size, &out_size);

    assert(result != NULL);
    assert(out_size == expected_size);
    for (int i = 0; i < out_size; i++) {
        assert(result[i].type  == expected_types[i]);
        assert(result[i].value == expected_chars[i]);
    }

    free(items);
    free(concat);
    free(result);
    printf("[OK] %s\n", label);
}

static void check_postfix_null(const char *input, const char *label) {
    int size, concat_size, out_size;
    Item *items  = itemize_regex(input, &size);
    Item *concat = implicit_to_explicit_concatenation(items, size, &concat_size);
    Item *result = shunting_yard(concat, concat_size, &out_size);

    assert(result == NULL);
    assert(out_size == 0);

    free(items);
    free(concat);
    printf("[OK] %s\n", label);
}

/////////////////////////////////////////////////////////////
// Test cases for shunting_yard                            //
/////////////////////////////////////////////////////////////

// a -> a
void test_postfix_single_char() {
    const char ec[] = {'a'};
    const ItemType et[] = {ITEM_CHAR};
    check_postfix("a", ec, et, 1, "postfix: a -> a");
}

// ab -> ab.
void test_postfix_concat() {
    const char ec[] = {'a', 'b', '.'};
    const ItemType et[] = {ITEM_CHAR, ITEM_CHAR, ITEM_CONCAT};
    check_postfix("ab", ec, et, 3, "postfix: ab -> ab.");
}

// a|b -> ab|
void test_postfix_alternation() {
    const char ec[] = {'a', 'b', '|'};
    const ItemType et[] = {ITEM_CHAR, ITEM_CHAR, ITEM_OR};
    check_postfix("a|b", ec, et, 3, "postfix: a|b -> ab|");
}

// a* -> a*
void test_postfix_star() {
    const char ec[] = {'a', '*'};
    const ItemType et[] = {ITEM_CHAR, ITEM_STAR};
    check_postfix("a*", ec, et, 2, "postfix: a* -> a*");
}

// a+ -> a+
void test_postfix_plus() {
    const char ec[] = {'a', '+'};
    const ItemType et[] = {ITEM_CHAR, ITEM_PLUS};
    check_postfix("a+", ec, et, 2, "postfix: a+ -> a+");
}

// a? -> a?
void test_postfix_question() {
    const char ec[] = {'a', '?'};
    const ItemType et[] = {ITEM_CHAR, ITEM_QUESTION};
    check_postfix("a?", ec, et, 2, "postfix: a? -> a?");
}

// a*b -> a*b.
void test_postfix_star_concat() {
    const char ec[] = {'a', '*', 'b', '.'};
    const ItemType et[] = {ITEM_CHAR, ITEM_STAR, ITEM_CHAR, ITEM_CONCAT};
    check_postfix("a*b", ec, et, 4, "postfix: a*b -> a*b.");
}

// ab|c -> ab.c|
void test_postfix_concat_vs_or() {
    const char ec[] = {'a', 'b', '.', 'c', '|'};
    const ItemType et[] = {ITEM_CHAR, ITEM_CHAR, ITEM_CONCAT, ITEM_CHAR, ITEM_OR};
    check_postfix("ab|c", ec, et, 5, "postfix: ab|c -> ab.c|");
}

// (a|b)* -> ab|*
void test_postfix_group_star() {
    const char ec[] = {'a', 'b', '|', '*'};
    const ItemType et[] = {ITEM_CHAR, ITEM_CHAR, ITEM_OR, ITEM_STAR};
    check_postfix("(a|b)*", ec, et, 4, "postfix: (a|b)* -> ab|*");
}

// (a|b)*c -> ab|*c.
void test_postfix_group_star_concat() {
    const char ec[] = {'a', 'b', '|', '*', 'c', '.'};
    const ItemType et[] = {ITEM_CHAR, ITEM_CHAR, ITEM_OR, ITEM_STAR, ITEM_CHAR, ITEM_CONCAT};
    check_postfix("(a|b)*c", ec, et, 6, "postfix: (a|b)*c -> ab|*c.");
}

// abc -> ab.c.
void test_postfix_three_chars() {
    const char ec[] = {'a', 'b', '.', 'c', '.'};
    const ItemType et[] = {ITEM_CHAR, ITEM_CHAR, ITEM_CONCAT, ITEM_CHAR, ITEM_CONCAT};
    check_postfix("abc", ec, et, 5, "postfix: abc -> ab.c.");
}

// (a)(b) -> ab.
void test_postfix_two_groups() {
    const char ec[] = {'a', 'b', '.'};
    const ItemType et[] = {ITEM_CHAR, ITEM_CHAR, ITEM_CONCAT};
    check_postfix("(a)(b)", ec, et, 3, "postfix: (a)(b) -> ab.");
}

// Mismatched ')' -> NULL
void test_postfix_mismatched_rparen() {
    check_postfix_null("a)b", "postfix: a)b -> NULL (mismatched ')')");
}

// Mismatched '(' -> NULL
void test_postfix_mismatched_lparen() {
    check_postfix_null("(ab", "postfix: (ab -> NULL (mismatched '(')");
}

// EXTRAS
// "hola" -> h o . l . a .
void test_postfix_hola() {
    const char ec[] = {'h', 'o', '.', 'l', '.', 'a', '.'};
    const ItemType et[] = {ITEM_CHAR, ITEM_CHAR, ITEM_CONCAT,
                           ITEM_CHAR, ITEM_CONCAT,
                           ITEM_CHAR, ITEM_CONCAT};
    check_postfix("hola", ec, et, 7, "postfix: hola -> ho.l.a.");
}

// "adios?" -> a d . i . o . s ? .
void test_postfix_adios_question() {
    const char ec[] = {'a', 'd', '.', 'i', '.', 'o', '.', 's', '?', '.'};
    const ItemType et[] = {ITEM_CHAR, ITEM_CHAR, ITEM_CONCAT,
                           ITEM_CHAR, ITEM_CONCAT,
                           ITEM_CHAR, ITEM_CONCAT,
                           ITEM_CHAR, ITEM_QUESTION, ITEM_CONCAT};
    check_postfix("adios?", ec, et, 10, "postfix: adios? -> ad.i.o.s?.");
}

// "a+cd" → explicit concat: a+.c.d → postfix: a+ c . d .
void test_postfix_plus_concat() {
    const char ec[] = {'a', '+', 'c', '.', 'd', '.'};
    const ItemType et[] = {ITEM_CHAR, ITEM_PLUS,
                           ITEM_CHAR, ITEM_CONCAT,
                           ITEM_CHAR, ITEM_CONCAT};
    check_postfix("a+cd", ec, et, 6, "postfix: a+cd -> (a+.c.d) -> a+c.d.");
}


int main(void) {
    char* sep = "================================================\n";
    printf("%s[1] Testing itemize_regex function\n%s", sep, sep);
    test_simple_chars();
    test_operators();
    test_escape();
    test_empty();
    test_parentheses();
    printf("[1] [ALL TESTS PASSED]\n%s", sep);

    printf("%s[2] Testing implicit_to_explicit_concatenation\n%s", sep, sep);
    test_concat_alternation();
    test_concat_star_char();
    test_concat_two_chars();
    test_concat_groups_alternation();
    test_concat_two_groups();
    test_concat_group_star_char();
    test_concat_three_chars();
    test_concat_question_char();
    test_concat_plus_char();
    test_concat_no_insert_inside_group();
    printf("[2] [ALL TESTS PASSED]\n%s", sep);

    printf("%s[3] Testing shunting_yard\n%s", sep, sep);
    test_postfix_single_char();
    test_postfix_concat();
    test_postfix_alternation();
    test_postfix_star();
    test_postfix_plus();
    test_postfix_question();
    test_postfix_star_concat();
    test_postfix_concat_vs_or();
    test_postfix_group_star();
    test_postfix_group_star_concat();
    test_postfix_three_chars();
    test_postfix_two_groups();
    test_postfix_mismatched_rparen();
    test_postfix_mismatched_lparen();
    test_postfix_hola();
    test_postfix_adios_question();
    test_postfix_plus_concat();
    printf("[3] [ALL TESTS PASSED]\n%s", sep);

    return 0;
}
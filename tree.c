#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tree.h"

int get_priority(char c)
{
    switch(c) {
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        case '^': return 3;
    }
    return 20; // Priority of numbers, brackets, variables, etc
}

Tree tree_create(Token *tokens, int idx_left, int idx_right)
{
    Tree t = (Tree)malloc(sizeof(struct _tree));

    if (idx_left > idx_right)
        return NULL;

    if (idx_left == idx_right) {
        t->node  = tokens[idx_left];
        t->left  = NULL;
        t->right = NULL;
        return t;
    }

    int priority = 0;
    int priority_min = get_priority('a');
    int brackets = 0;
    int op_pos = 0;

    for (int i = idx_left; i < idx_right; ++i) {
        if ((tokens[i].type == BRACKET) && tokens[i].data.is_left_bracket) {
            ++brackets;
            continue;
        }
        if ((tokens[i].type == BRACKET) && !tokens[i].data.is_left_bracket) {
            --brackets;
            continue;
        }
        if (brackets > 0)
            continue;

        if (tokens[i].type == OPERATOR) {
            priority = get_priority(tokens[i].data.operator);
            if (priority <= priority_min) {
                priority_min = priority;
                op_pos = i;
            }
        }
    }
    if ((priority_min == get_priority('a'))     &&
        (tokens[idx_left].type == BRACKET)      &&
         tokens[idx_left].data.is_left_bracket  &&
        (tokens[idx_right].type == BRACKET)     &&
        !tokens[idx_right].data.is_left_bracket) {
        free(t);
        return tree_create(tokens, idx_left + 1, idx_right - 1);
    }

    if (tokens[op_pos].data.operator == '^') {
        brackets = 0;
        for (int i = op_pos; i >= idx_left; --i) {
            if ((tokens[i].type == BRACKET) && !(tokens[i].data.is_left_bracket)) {
                ++brackets;
                continue;
            }
            if ((tokens[i].type == BRACKET) && (tokens[i].data.is_left_bracket)) {
                --brackets;
                continue;
            }
            if (brackets > 0) {
                continue;
            }
            if (tokens[i].type == OPERATOR) {
                priority = get_priority(tokens[i].data.operator);
                if (priority == 3) {
                    op_pos = i;
                }
            }
        }
    }

    t->node  = tokens[op_pos];
    t->left  = tree_create(tokens, idx_left, op_pos - 1);
    t->right = tree_create(tokens, op_pos + 1, idx_right);
    if (t->right == NULL) {
        fprintf(stderr, "Error: operator at the expression's end.");
        exit(1);
    }
    return t;
}

void tree_destroy(Tree *t)
{
    if ((*t) != NULL) {
        tree_destroy(&((*t)->left));
        tree_destroy(&((*t)->right));
    }
    free(*t);
    *t = NULL;
}

void tree_infix(Tree t)
{
    if (t != NULL) {
        if (t->left && t->right)
            printf("(");

        tree_infix(t->left);
        token_print(&(t->node));
        tree_infix(t->right);

        if (t->right && t->left)
            printf(")");
    }
}

void tree_print(Tree t, int depth)
{
    if (t != NULL) {
        for (int i = 0; i < depth; ++i) {
            printf("\t");
        }
        token_print(&(t->node));
        printf("\n");
        tree_print(t->left,  depth + 1);
        tree_print(t->right, depth + 1);
    }
}

void tree_simplify(Tree t)
{
    if (t->left) {
        tree_simplify(t->left);
    }
    if (t->right) {
        tree_simplify(t->right);
    }

    if (t->node.type == OPERATOR && t->node.data.operator == '/') {
        if (t->left && t->left->node.type == OPERATOR && t->left->node.data.operator == '/') {
            Tree tree = t->left;
            t->left = t->right;
            t->right = tree;
            t->right->node.data.operator = '*';
            tree = t->left;
            t->left = t->right->left;
            t->right->left = tree;
        } else if (t->right && t->right->node.type == OPERATOR && t->right->node.data.operator == '/') {
            Tree tree = t->left;
            t->node.data.operator = '*';
            t->left = t->right->right;
            t->right->right = t->right->left;
            t->right->left = tree;
        }
    }
}
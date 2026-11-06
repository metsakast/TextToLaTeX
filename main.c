#include <stdio.h>
#include <ctype.h>
#include <string.h>

typedef enum {
    TKN_NUMBER,         // 6, 15, 12.5
    TKN_IDENTIFIER,     // b, ac, sqrt
    TKN_OPERATOR,       // + - * / ^ =
    TKN_LPAREN,         // (
    TKN_RPAREN,         // )
    TKN_END             // marks the end
} TokenType;

typedef struct {
    TokenType kind;     // what token kind
    char text[32];      // the characters themselves
} Token;

int tokenize(const char *equation, Token tokens[]) {
    int count = 0;
    int i = 0;

    while (equation[i] != '\0') {
        char c = equation[i];

        if (isdigit(c)) {
            int j = 0;
            while (isdigit(equation[i])) {
                tokens[count].text[j] = equation[i];
                j++;
                i++;
            }
            tokens[count].text[j] = '\0';
            tokens[count].kind = TKN_NUMBER;
            count++;
        } else if (isalpha(c)) {
            int j = 0;
            while (isalpha(equation[i])) {
                tokens[count].text[j] = equation[i];
                j++;
                i++;
            }
            tokens[count].text[j] = '\0';
            tokens[count].kind = TKN_IDENTIFIER;
            count++;
        } else if (c == '(') {
            tokens[count].text[0] = c;
            tokens[count].text[1] = '\0';
            tokens[count].kind = TKN_LPAREN;
            i++;
            count++;
        } else if (c == ')') {
            tokens[count].text[0] = c;
            tokens[count].text[1] = '\0';
            tokens[count].kind = TKN_RPAREN;
            i++;
            count++;
        } else if (strchr("+-*/^=", c)) {
            tokens[count].text[0] = c;
            tokens[count].text[1] = '\0';
            tokens[count].kind = TKN_OPERATOR;
            i++;
            count++;
        } else if (isspace(c)) i++;
        else {
            printf("Unknown character '%c' at position %d\n", c, i);
            return -1;
        }
    }
    tokens[count].kind = TKN_END;
    tokens[count].text[0] = '\0';
    count++;
    return count;
}

void printTokens(Token tokens[], int start, int end) {
    for (int i = start; i < end; i++) {
        Token t = tokens[i];

        if (t.kind == TKN_OPERATOR && t.text[0] == '*') printf(" \\cdot ");
        else if (t.kind == TKN_OPERATOR && t.text[0] == '^') {
            printf("^{");
            i++;

            if (i < end && tokens[i].kind == TKN_LPAREN) {
                int depth  = 1;
                int j = i + 1;
                while (j < end) {                        // find the matching ')'
                    if (tokens[j].kind == TKN_LPAREN) depth++;
                    else if (tokens[j].kind == TKN_RPAREN) depth--;
                    if (depth == 0) break;
                    j++;
                }
                printTokens(tokens, i + 1, j);  // call the function again, print items inside parenthesis
                i = j;                                   // skip past the ')'
            }
            else if (i < end) {
                printf("%s", tokens[i].text);      // if the exponent is a single token
            }
            printf("}");
        }
        else printf("%s", t.text);                 // tokens which remain identical in latex, + - numbers, identifiers, parenthesis
    }
}

void printLatex(Token tokens[], int count) {
    printf("$");
    printTokens(tokens, 0, count - 1);          // count includes TKN_END
    printf("$\n");
}

int checkParenthesis(Token tokens[]) {                   // check that parenthesis are valid beforehand
    int depth = 0;
    for (int i = 0; tokens[i].kind != TKN_END; i++) {
        if (tokens[i].kind == TKN_LPAREN) depth++;
        if (tokens[i].kind == TKN_RPAREN) depth--;
        if (depth < 0) return 0;
    }
    if (depth == 0) return 1;
    return 0;
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);

    char userChoice;
    char userEquation[256];

    while (1) {
        printf(" *** TEXT TO LATEX BY METS, version:0.2 ***\n1) convert to latex\n2) quit\n");
        scanf(" %c", &userChoice);
        while (getchar() != '\n');

        switch (userChoice) {
            case '2': return 0;
            case '1':
                printf("Enter your equation: ");
                fgets(userEquation, sizeof(userEquation), stdin);
                Token tokens[100];

                int count = tokenize(userEquation, tokens);
                if (count == -1) return 1;
                if (checkParenthesis(tokens) != 1) {
                    printf("Invalid input: Check parenthesis!");
                    return 1;
                }
                /*for (int i = 0; i < count; i++) {
                    printf("kind=%d text='%s'\n", tokens[i].kind, tokens[i].text);
                }*/
                printLatex(tokens, count);
                break;
            default: printf("Invalid command.\n");
                break;
        }
    }
}

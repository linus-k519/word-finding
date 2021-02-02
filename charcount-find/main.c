#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DICT_LEN 190
#define WORD_LEN 64

typedef struct Word {
    unsigned char charcount[26];
    char* text;
} word_t;

word_t* dict[DICT_LEN];

void create_charcount(char* text, unsigned char* charcount_buffer) {
    // Clear buffer
    memset(charcount_buffer, 0, 26);
    // Count occurrences of each char
    for (int i = 0; text[i] != '\0'; i++) {
        // Calculate charcount index
        int charcount_index;
        if (text[i] >= 0x61 && text[i] <= 0x7A) {
            // Char is lowercase
            charcount_index = text[i] - 0x61;
        } else if (text[i] >= 0x41 && text[i] <= 0x5A) {
            // Char is uppercase
            charcount_index = text[i] - 0x41;
        } else {
            fprintf(stderr, "create_charcount: Invalid char\n");
            exit(EXIT_FAILURE);
        }
        charcount_buffer[charcount_index]++;
    }
}

void add_word_to_dict(char* text, int dict_index) {
    word_t *word = malloc(sizeof(word_t));
    if (word == NULL) {
        perror("malloc word");
        exit(EXIT_FAILURE);
    }
    memset(word->charcount, 0, sizeof(word->charcount));

    // Copy text to word_t
    word->text = malloc(sizeof(text));
    if (word->text == NULL) {
        perror("add_word_to_dict: copy text to word: malloc word->text");
        exit(EXIT_FAILURE);
    }
    strcpy(word->text, text);

    // Create charcount and add word to dict
    create_charcount(text, word->charcount);
    dict[dict_index] = word;
}

void read_dict(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("read_dict: fopen");
        exit(EXIT_FAILURE);
    }

    char line[WORD_LEN];
    for (int line_nr = 0; line_nr < DICT_LEN; line_nr++) {
        // Read one line
        if (fgets(line, sizeof(line), file) <= 0) {
            break;
        }

        // Delete newline
        size_t possible_newline = strlen(line) - 1;
        if (possible_newline >= 0 && line[possible_newline] == '\n') {
            line[possible_newline] = '\0';
        }

        add_word_to_dict(line, line_nr);
    }
    fclose(file);
}

word_t* search_charcount_in_dict(unsigned char* search_charcount) {
    for (int i = 0; i < DICT_LEN; i++) {
        // Compare current charcount with search_charcount
        if (memcmp(dict[i]->charcount, search_charcount, 26) == 0) {
            return dict[i];
        }
    }
    return NULL;
}

int main() {
    read_dict("../dict.txt");

    printf("Word: ");
    fflush(stdout);
    char input_word[WORD_LEN];
    if (scanf("%s", input_word) < 0) {
        perror("read user word");
        exit(EXIT_FAILURE);
    }
    unsigned char input_word_charcount[26];
    create_charcount(input_word, input_word_charcount);

    word_t* found_word = search_charcount_in_dict(input_word_charcount);
    if (found_word) {
        // Found word in dict
        printf("Found word %s in dict\n", found_word->text);
    } else {
        // Can't find word in dict
        printf("Could not find word\n");
    }

    return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// The maximum length of a word
#define WORD_LEN 64

typedef struct {
    // The number of occurrences for each char in the alphabet.
    unsigned char charcount[26];
    // The associated word.
    char text[WORD_LEN];
} word_t;

typedef struct {
    // The content of the dict
    word_t* values;
    // The number of entries managed by values.
    int len;
} dict_t;

dict_t dict;

// Creates a charcount for the given text and writes it into the charcount_buffer.
void create_charcount(const char* text, unsigned char* charcount_buffer) {
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
            fprintf(stderr, "create_charcount: Invalid char %d on word %s\n", text[i], text);
            exit(EXIT_FAILURE);
        }
        charcount_buffer[charcount_index]++;
    }
}

// Returns the number of lines in the file.
int get_line_numbers(char* filename) {
    // Open file
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("get_line_numbers: fopen");
        exit(EXIT_FAILURE);
    }

    // Count newlines
    int lines = 0;
    while(!feof(file)) {
        int c = fgetc(file);
        if(c == '\n') {
            lines++;
        }
    }

    // Close file
    if (fclose(file) < 0) {
        perror("get_line_numbers: fclose");
        exit(EXIT_FAILURE);
    }

    return lines;
}

// Reads and parses the file to dict.
void read_dict(char* filename) {
    // Determine number of lines of the dict file
    dict.len = get_line_numbers(filename);

    // Allocate memory for the dict
    dict.values = malloc(sizeof(word_t) * dict.len);
    if (dict.values == NULL) {
        perror("read_dict: malloc Dict");
        exit(EXIT_FAILURE);
    }

    // Open dict file
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("read_dict: fopen");
        exit(EXIT_FAILURE);
    }

    // Read each line from the dict file
    for (int line_nr = 0; line_nr < dict.len; line_nr++) {
        char* line = dict.values[line_nr].text;
        // Read one line
        if (fgets(line, WORD_LEN, file) <= 0) {
            break;
        }

        // Delete newline
        size_t possible_newline = strlen(dict.values[line_nr].text) - 1;
        if (possible_newline >= 0 && line[possible_newline] == '\n') {
            line[possible_newline] = '\0';
        }

        create_charcount(line, dict.values[line_nr].charcount);
    }
    fclose(file);
}

// Searches for a word in the dict
word_t* search_charcount_in_dict(unsigned char* search_charcount) {
    for (int i = 0; i < dict.len; i++) {
        // Compare current charcount with search_charcount
        if (memcmp(dict.values[i].charcount, search_charcount, 26) == 0) {
            return &dict.values[i];
        }
    }
    return NULL;
}

int main() {
    // Load the dict into memory
    clock_t loading_start = clock();
    read_dict("../dict.txt");
    clock_t loading_end = clock();
    double loading_duration = (double)(loading_end - loading_start) / CLOCKS_PER_SEC;
    printf("Loading took %.2fms\n", loading_duration*1000);

    // Read word from user
    printf("Word: ");
    fflush(stdout);
    char input_word[WORD_LEN];
    if (scanf("%s", input_word) < 0) {
        perror("read user word");
        exit(EXIT_FAILURE);
    }
    unsigned char input_word_charcount[26];

    // Search for word in the dict
    clock_t searching_start = clock();
    create_charcount(input_word, input_word_charcount);
    word_t* found_word = search_charcount_in_dict(input_word_charcount);
    clock_t searching_end = clock();
    double searching_duration = (double)(searching_end - searching_start) / CLOCKS_PER_SEC;
    if (found_word) {
        // Found word in dict
        printf("Found word %s in %.2fms\n", found_word->text, searching_duration*1000);
    } else {
        // Can't find word in dict
        printf("Could not find word in %.2fms\n", searching_duration*1000);
    }

    return EXIT_SUCCESS;
}

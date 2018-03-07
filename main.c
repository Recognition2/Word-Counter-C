#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum word size. Words a bove this size do not get accepted into the buffer.
#define BUFLEN 256

struct word_counter {
    char *word;         // Which word_counter are we counting?
    int count;          // How often does this word_counter occur?
    struct word_counter *next;  // To implement linked list
};

struct word_counter* create(char *input, size_t len) {
    // Allocate space for the new element in the list
    struct word_counter *new = malloc(sizeof(struct word_counter));
    new->next = NULL;
    new->count = 1;     // Sensible defaults, here.
    new->word = NULL;

    if (input !=  NULL) {   // Allocate space for the new word, and copy it.
        char *new_word = malloc(sizeof(char) * (len + 1));
        strcpy(new_word, input);
        new->word = new_word;
    }

    return new;
}

int is_char(int c) {
    return (c >= 48 && c <= 57) ||  // Numbers
           (c >= 65 && c <= 90) ||  // Uppercase char
           (c >= 97 && c <= 122);   // Lowercase char
}

void prepend(struct word_counter **head, struct word_counter *insert) {
    insert->next = *head;
    *head = insert;
}

void append(struct word_counter *head, struct word_counter *insert) {
    insert->next = head->next;
    head->next = insert;
}

void handle_word(struct word_counter **head, char *input, size_t len) {
    struct word_counter *current = *head;
    struct word_counter *old = NULL; // Create another element, whose next field points to head.
    int res;

    if (current->word == NULL) {
        // This only happens if the chain is empty and contains zero words.
        // To add a word, we malloc space for the char array and set the count to one.
        current->word = malloc(sizeof(char) * (len+1));
        strncpy(current->word, input, len+1);
        current->count = 1;
        return;
    }

    // Keep iterating until we arrive at the correct block, or we're at the end of the list
    while ((res = strcmp(current->word, input)) < 0 && current->next != NULL) {
        old = current; // Keep a pointer to the old word
        current = current->next;
    }

    // The word in the current block is equal to the considered word, so we can increment the count field.
    if (res == 0) {
        current->count++;
    } else {
        // We need to create a new block, as there is no block containing this word yet.
        struct word_counter *new_block = create(input, len);
        if (res < 0) {
            append(current, new_block);
        } else {
            // Replacing the head is a special case, as there is no old.next value
            // Otherwise, we can use a pointer to old.next
            prepend(current == *head ? head : &old->next, new_block);
        }
    }
}

void print_destroy(struct word_counter *first) {
    struct word_counter *current = first;
    struct word_counter *new = NULL;
    while (current != NULL) {
        // Print statistics, except for the first one
        printf("%s: %d\n", current->word, current->count);
        new = current->next;
        // Free word_counter and then word_counter
        free(current->word);
        free(current);
        current = new;
    }
}

int eof_check(char c) {
    static char possible[6] = {'\0'};
    static int count = 0;

    // New line has started, start looking for EOF.
    if (c == '\n') {
        count = 0;
        return 0;
    }

    possible[count++] = c;

    if (count == 4) {
        if (strncmp(possible, "#EOF", 4) == 0) {
            return 1;
        }
    }
    return 0;
}

int main() {
    char input[BUFLEN];

    // Create head pointer
    struct word_counter *head = create(NULL, 0);

    int c;
    size_t i = 0;
    while ((c = fgetc(stdin)) != EOF) {
        if (i > BUFLEN-2) {
            printf("The input word_counter was too large, we cannot handle this.");
            break;
        }

        if (eof_check((char) c)) {
            break;
        }

        if (is_char(c)) {
            input[i++] = (char) c; // Append character to the input string
        } else if (i > 0) {
            input[i] = '\0'; // Append a zero
            handle_word(&head, input, i);
            i = 0;
        }
    }

    // For printing the contents of the list, and freeing memory
    print_destroy(head);

    return 0;
}

// Implements a dictionary's functionality

#include <stdbool.h>
#include "dictionary.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern FILE *stderr;

#define WORD_SIZE 50
#define HASHTABLE_SIZE 16384
// #define MAX_WORD_LENGTH 45

//track # of words in dictionary
unsigned long num_words = 0;

bool isLoaded = false;

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

typedef struct node *p_node;


// Number of buckets in hash table
const unsigned int N = 1000000; //THIS SHOULD BE >> 1

node *table[HASHTABLE_SIZE];


void free(void *ptr);

p_node createNode(const char *key)
{
    p_node node = malloc(sizeof(struct node)); //create ptr to storage space for node
    strcpy(node->word, key);
    node->next = NULL;
    return node;
}

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    // TODO
    //create char array for word
    char word_cpy[strlen(word) + 1];

    //copy string to char array variable
    strcpy(word_cpy, word);

    for (int i = 0; word_cpy[i] != '\0'; i++)
    {
      word_cpy[i] = tolower(word_cpy[i]);
    }

    //retrieve hash value from hash fxn
    int bucket = hash(word_cpy);


    if (table[bucket] != NULL)
    {
        for (p_node cursor = table[bucket]; cursor != NULL; cursor = cursor->next)
        {
            if (strcmp(cursor->word, word_cpy) == 0)
            {
                return true;
            }
        }
    }
    return false;
}

unsigned int hash(const char *word) //orignal func header
{
    int hash = 0;
    int c;
    int length = strlen(word);

    for (int j = 0; word[j] != '\0'; j++)
    {
        if (isalpha(word[j]) == 0)
        {
            c = word[j] - 'a' + 1;
        }
        else
        {
            c = 27;
        }
        hash = ((hash << 3) + c) % HASHTABLE_SIZE;
    }
    return hash;
}


// Hashes word to a number
// unsigned long hash(const char *word)
// // unsigned long hash(const char *word) //orignal func header
//     // TODO
//     /**************************************************
//     * Title: djb2 hash function code
//     * Author: Bernstein, Dan
//     * Date: 2020
//     * Code version: 1.0
//     * Availability: http://www.cse.yorku.ca/~oz/hash.html
// ***************************************************/

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    FILE* p_in = fopen(dictionary, "r");

    if (p_in == NULL) //check if there is anything in the dictionary file
    {
        return false;
    }

    //initialize ptr for every potential hashtable element
    for (int i = 0; i < HASHTABLE_SIZE; i++)
    {
        table[i] = NULL;
    }

    char buffer[LENGTH + 1]; // create buffer to store every string read in.
    // p_node new_node = createNode(buffer);
    // printf("Start Loading!"); //troubleshooting


    while (fscanf(p_in, "%s", buffer) != EOF )
    {
        num_words++;
        p_node new_node = createNode(buffer);


        if (new_node == NULL)
        {
            free(new_node);
        }


        strcpy(new_node->word, buffer);

        int idx = hash(buffer);

        new_node->next = table[idx];
        table[idx] = new_node;

    }

    printf("Finished Loading!");

    fclose(p_in);
    isLoaded = true;

    // printf("Finished Loading!");
    return true;

    // return false;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    // TODO
    if(!isLoaded)
    {
        return 0;
    }
    return num_words;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    // TODO
    if (!isLoaded)
    {
        return false;
    }
    for (int i = 0; i < HASHTABLE_SIZE; i++)
    {
        if (table[i] != NULL)
        {
            p_node tmp = table[i];
            while (tmp != NULL)
            {
                p_node prev = tmp;
                tmp = prev->next;
                free(prev);
            }
        }
    }
    return true;
}
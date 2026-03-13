// map.c
//
// This is a basic map data structure for key, value pairs of strings.
// Simplicity is favored over efficiency. Map lookups use linear search.
//
//
// Code for Clark University CSCI 140 assignments.
//
// Author: John Magee
// Date: 10/5/2017
// Version 1.0
// 
// Author: Ken Basye
// Date: 2/9/2025
// Version 1.1
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "map.h"

#define MAX_STR_LENGTH 40

// Allocates a map data structure and an array of key,value pairs
// up to the max size provided.
// 
// The memory allocated by this method can be freed by calling the freeMap() method
Map createMap(unsigned int maxSize) {
    // Allocate memory for the main struct and initialize values
    Map newmap =  (Map) malloc(sizeof(mapstruct));
    newmap->maxSize = maxSize;
    newmap->mapSize = 0;

    // allocate memory for the key, value pairs
    newmap->pairs = (stringpair *) malloc(maxSize * sizeof(stringpair));
    
    // Ensure that each key and value is currently NULL
    for(int i = 0; i < maxSize; i++) {
        newmap->pairs[i].key = NULL;
        newmap->pairs[i].value = NULL;
    }

    // return a pointer to the newly allocated map
    return newmap;
}


// freeMap
// Frees the memory allocated by createMap
// Also frees any strings allocated within insertKey
// Note: the map becomes unusable after this call!
void freeMap(Map currentMap) {
    if(currentMap == NULL) {
       return;
    }

    // delete any strings stored within the map
    // CAUTION: Any strings copied by pointer will be destroyed!
    for(int i = 0; i < currentMap->maxSize; i++) {
        // Note: in C, it's OK to call free with a NULL ptr
        free(currentMap->pairs[i].key);
        free(currentMap->pairs[i].value);
    }

    // delete the array of pairs
    free(currentMap->pairs);

    // delete the map itself
    free(currentMap);
}


// containsKey method
// If the key is found, returns its index
// otherwise returns -1 if the key is not found
int containsKey(Map aMap, char *searchKey) {
    // linear search for the key
    for(int i = 0; i < aMap->mapSize; i++) {
        if(strcmp(aMap->pairs[i].key, searchKey) == 0) {
            // key is found at location i
            return i;
        }
    }

    // not found, return -1
    return -1;    
}


// lookupIndex method
// lookupIndex is intended to be used with the containsKey method.
// If an index is already known, the value can be returned directly without
// searching through the whole array.  Return NULL if index is not valid.
char* lookupIndex(Map aMap, int index) {
    if (index < 0 || index >= aMap->mapSize) {
        return NULL;
    }
    return aMap->pairs[index].value;
}


// lookupKey
// If the key is found, returns the associated value string
// otherwise returns NULL if the key is not found
char* lookupKey(Map aMap, char *searchKey) {

    int index = containsKey(aMap, searchKey);
    if(index >= 0 ) {
          return lookupIndex(aMap, index);
    } else {
          return NULL;
    }
}


// insertKey
// Inserts a key into a map.  Returns 1 for successful insert, 0 for failure.
// If key is already in the map, just update the value and return 1
int insertKey(Map aMap, char* key, char *value) {
    int index = containsKey(aMap, key);
    if(index != -1) {
        // found key, just modify the existing value
        assert(strlen(value) < MAX_STR_LENGTH);
	strncpy(aMap->pairs[index].value, value, MAX_STR_LENGTH);
        return 1;
    }
    else if(aMap->mapSize < aMap->maxSize) {
        // Note: In C, sizeof(char) is always 1
        aMap->pairs[aMap->mapSize].key = (char *) malloc(MAX_STR_LENGTH);
        aMap->pairs[aMap->mapSize].value = (char *) malloc(MAX_STR_LENGTH);
        assert(strlen(key) < MAX_STR_LENGTH);
        assert(strlen(value) < MAX_STR_LENGTH);
	strncpy(aMap->pairs[aMap->mapSize].key, key, MAX_STR_LENGTH);
	strncpy(aMap->pairs[aMap->mapSize].value, value, MAX_STR_LENGTH);
        aMap->mapSize++; 
        return 1;
    }
    else {
        fprintf(stderr, "Warning: map full, last insert failed!\n");
        return 0;
    }
}


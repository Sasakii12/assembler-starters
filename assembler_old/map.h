// map.h
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

#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

typedef struct {
    char *key;
    char *value;
} stringpair;

typedef struct {
    unsigned int mapSize;
    unsigned int maxSize;
    stringpair* pairs;
}  mapstruct;

typedef mapstruct* Map;

// function declarations

// Create and return an empty map
Map createMap(unsigned int maxSize);

// Free all the memory owned by a map
// Note: the map becomes unusable after this call!
void freeMap(Map currentMap);

// If the key is found, returns its index
// otherwise returns -1 if the key is not found
int containsKey(Map aMap, char *searchKey);

// Given a valid index, return the value at that index.
// Returns NULL if index is not a valid index for some key/value pair
char* lookupIndex(Map aMap, int index) ;

// If the key is found, returns the associated value string
// otherwise returns NULL if the key is not found
// Note: the map owns the string it returns - don't free or change it!
char* lookupKey(Map aMap, char *searchKey);

// Inserts a key into a map.  Returns 1 for successful insert, 0 for failure.
// If key is already in the map, just update the value and return 1
int insertKey(Map aMap, char* key, char *value);


#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

extern int write_xactions;
extern int read_xactions;

void printHelp(const char * prog);

int logtwo(int number);
struct cache* newCache (int ways, int sets);
int fetch(struct cache* c, char type, uint32_t index, uint32_t tag, int ways, uint32_t address, int assoc);
char* concat(const char *a, const char *b);
int accessedBefore(struct cache* c, uint32_t tag);
void freeEverything(struct cache* c, int sets);
// typedef struct CACHE cache;

#ifndef CACHE
#define CACHE


struct cache{
  int** valid;
  int** dirty;
  int** tags;
  int** data;
  int* counter;

  int tagBit;
  int indexBit;
  int offsetBit;
  int occupiedLines;
  int sets;
  uint32_t * accessedBefore;
  int accessedLength;
  int arrLength;
};






#endif

#ifndef IO_H_DEFINE
#define IO_H_DEFINE

#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER

#include "data.h"
#include <stdio.h>

#define CHUNK 4

char * getLine(FILE * src);
char * getValidatedUserString(char *question);

void clearBuffer(void);

FILE * openFileFromUserPrompt(void);
FILE * saveFileFromUserPrompt(void);

struct world loadWorldFromFile(FILE * ifp);
struct items buildItemsFromFile(FILE * ifp);
struct rooms buildRoomsFromFile(FILE * ifp);
struct combos buildCombosFromFile(FILE * ifp);

void distributeItemsFromFile(FILE * ifp, struct ll_room *worldRooms, struct ll_item *protos);

void saveWorldToFile(FILE * ofp, struct world data);
void saveItemsToFile(FILE * ofp, struct items protos);
void saveRoomsToFile(FILE * ofp, struct rooms protos);
void saveDoorsToFile(FILE * ofp, struct doors protos);
void saveCombosToFile(FILE * ofp, struct combos protos);
void saveDistroToFile(FILE * ofp, struct rooms protos, struct items itemPrototypes);

#endif // IO_H_DEFINE

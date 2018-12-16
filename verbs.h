#ifndef VERBS_H_DEFINE
#define VERBS_H_DEFINE

#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER

#include "data.h"

void handlePlayerVerb(struct world *gamedata);
void printPlayerHelp(void);
void handleGoVerb(struct doors paths, struct rooms worldRooms, int * currentIndex, int target);
void handleInspectVerb(struct items roomItems, struct items bagItems, int target);
void handleTakeVerb(struct items *roomItems, struct items *bagItems, int target);
void handleUseVerb(struct world *gamedata, int target);
void handleCombineVerb(int target, int secondTarget, struct items *roomItems, struct items *bagItems, struct combos recipes, struct items protos);

void handleQuitVerb(int *playing);

void handleEditorVerb(struct world *gamedata);
void printEditorHelp(char mode, char command);

void handleRoomEditorVerb(char command, struct world *gamedata);
void handleDoorEditorVerb(char command, struct world *gamedata);
void handleItemEditorVerb(char command, struct world *gamedata);
void handleComboEditorVerb(char command, struct world *gamedata);

#endif // VERBS_H_DEFINE

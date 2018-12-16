#ifndef ACTIONS_H_DEFINE
#define ACTIONS_H_DEFINE

#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER

#include "data.h"

void handleAction(struct item *src, struct world *gamedata, int srcIndex);
void handleDispenseAction(struct action *srcAction, struct items *inventory, struct items proto);
void handleTransformAction(struct action *srcAction, struct items *inventory, struct items *roomItems, struct items proto, int srcID);
void handleUnlockAction(struct action *srcAction, struct rooms *worldRooms, struct items *inventory, int numRoomItems, int currentRoomId, int srcIndex);

#endif // ACTIONS_H_DEFINE

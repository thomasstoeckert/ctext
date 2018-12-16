/*  Thomas Stoeckert
    COP 3223H - Final Project

    This file handles actions that items can do. When a user uses an item, the following functions
    are called according to what action an item has.

    */

#include "actions.h"
#include <stdio.h>

// ---- Player Function ---- //
// HandleAction takes in a pointer to the original item, a pointer to the world information, and the index of the source
// It takes these and divies up the work to the other action handlers
void handleAction(struct item *src, struct world *gamedata, int srcIndex){
    struct action *srcAction = &src->itemAction;
    struct items *currentRoomItems = &getCurrentRoom(gamedata->worldRooms, gamedata->currentRoomId)->inventory;
    int srcActionType = srcAction->actionType;

    switch(srcActionType){
        case -1:
            printf("This %s doesn't seem to do anything...\n", src->name);
            return;
        case 0:
            handleDispenseAction(srcAction, &gamedata->inventory, gamedata->itemPrototypes);
            return;
        case 1:
            handleTransformAction(srcAction, &gamedata->inventory, currentRoomItems, gamedata->itemPrototypes, srcIndex);
            return;
        case 2:
            handleUnlockAction(srcAction, &gamedata->worldRooms, &gamedata->inventory, currentRoomItems->numItems, gamedata->currentRoomId, srcIndex);
            return;
        case 3:
            // Game over - just print out the game-over condition and set the playing value to zero.
            printf("%s\n", src->itemAction.desc);
            gamedata->playing = 0;
            return;
    }
}

// ---- Player Function ---- //
// handleDispenseAction takes in a pointer to the original action, a pointer to the user's inventory, and a copy of the
//  item prototypes
// A dispense action creates a set amount (or infinite amount) of items, one at a time, in the user's inventory
void handleDispenseAction(struct action *srcAction, struct items *inventory, struct items proto){
    struct item dispensedItem;
    int *quantity = &srcAction->quantity;

    if((*quantity) == 0){
        // We don't dispense if the item is exactly zero. If it's negative, it dispenses infinitely by design.
        printf("The item can't dispense any more.\n");
        return;
    } else if((*quantity) > 0){
        (*quantity)--;
    }

    dispensedItem = iFindItemByIndex(proto.itemData, srcAction->itemTarget)->data;
    printf("%s\n", srcAction->desc);
    inventory->itemData = iAppend(inventory->itemData, dispensedItem);
    inventory->numItems++;
    return;
}

// ---- Player Function ---- //
// a transform action creates the target item, adds it to the appropriate inventory, then deletes the original after
// informing the user about the transformation
void handleTransformAction(struct action *srcAction, struct items *inventory, struct items *roomItems, struct items proto, int srcIndex){
    struct item srcItem;
    struct item newItem;
    int srcId;
    int fromRoom = 0;

    if(srcIndex < roomItems->numItems){
        fromRoom = 1;
        srcItem = iFindItemByIndex(roomItems->itemData, srcIndex)->data;
    } else {
        srcItem = iFindItemByIndex(inventory->itemData, srcIndex - roomItems->numItems)->data;
    }
    srcId = srcItem.id;

    newItem = iFindItemByIndex(proto.itemData, srcAction->itemTarget)->data;
    printf("%s\n", srcAction->desc);
    if(fromRoom){
        roomItems->itemData = iAppend(roomItems->itemData, newItem);
        roomItems->itemData = iDeleteItemByID(roomItems->itemData, srcId);
    } else {
        inventory->itemData = iAppend(inventory->itemData, newItem);
        inventory->itemData = iDeleteItemByID(inventory->itemData, srcId);
    }
    return;
}

// ---- Player Function ---- //
// An unlock action changes the destination of a door in a room to another destination. This can be used
// for unlocking doors, elevator buttons, shifting mazes, pretty much anything
void handleUnlockAction(struct action *srcAction, struct rooms *worldRooms, struct items *inventory, int numRoomItems, int currentRoomID, int srcIndex) {
    struct room *targetRoom;
    struct room *currentRoom;
    struct doors *targetDoors;
    struct door *targetDoor;

    if(currentRoomID != srcAction->roomTarget){
        printf("This key doesn't seem to work in this room...\n");
        return;
    }

    targetRoom = &rFindRoomByID(worldRooms->roomData, srcAction->roomTarget)->data;
    targetDoors = &targetRoom->paths;
    targetDoor = &dFindDoorByIndex(targetDoors->paths, srcAction->doorTarget)->data;

    currentRoom = getCurrentRoom((*worldRooms), currentRoomID);

    targetDoor->dest = srcAction->destTarget;
    printf("%s\n", srcAction->desc);

    // Remove the key
    if(srcIndex >= numRoomItems){
        inventory->itemData = iDeleteItemByIndex(inventory->itemData, srcIndex - numRoomItems);
        inventory->numItems--;
    } else {
        currentRoom->inventory.itemData = iDeleteItemByIndex(currentRoom->inventory.itemData, srcIndex);
        currentRoom->inventory.numItems--;
    }


    return;
}

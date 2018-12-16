/*  Thomas Stoeckert
    COP 3223H - Final Project

    This file holds all the structs and helper functions related to the structs.

    It's quite massive. But anything that deals with an ll object is very similar to each other, since I have to
     repeat it for each type of node.

    I've documented the first set, but it's still a lot

    I looked into null-pointer ll nodes, but decided not to for this project

*/
#include "data.h"
#include "wizards.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Adds the item data to the end of the linked list that starts at the head
struct ll_item * iAppend(struct ll_item *head, struct item data) {
    struct ll_item *newEntry = (struct ll_item*) malloc(sizeof(struct ll_item));
    struct ll_item *helper = head;

    newEntry->data = data;
    newEntry->next = NULL;

    if(head == NULL) {
        return newEntry;
    }

    while(helper->next != NULL) {
        helper = helper->next;
    }

    helper->next = newEntry;

    return head;
};

// Returns a pointer to the item at the index specified
struct ll_item * iFindItemByIndex(struct ll_item *head, int index) {
    struct ll_item *helper = head;
    int currIndex = 0;

    while(helper != NULL) {
        if(currIndex == index) {
            return helper;
        } else {
            helper = helper->next;
            currIndex++;
        }
    }

    return head;
};

// Searches for the item ID instead of the item index in the list
struct ll_item * iFindItemByID(struct ll_item *head, int id) {
    struct ll_item *helper = head;

    while(helper != NULL) {
        if(helper->data.id == id){
            return helper;
        } else {
            helper = helper->next;
        }
    }

    return head;
};

// Deletes the item index in the list by the items index
struct ll_item * iDeleteItemByIndex(struct ll_item *head, int index) {
    struct ll_item * helper = head, *temp;
    int currIndex = 0;

    if(head==NULL) {
        return head;
    }

    // Delete if the index is now
    if(currIndex == index){
        temp = helper;
        helper = helper->next;
        free(temp);
        return helper;
    }

    // Check for middle indexes
    while (helper->next->next != NULL) {
        if((currIndex+1) == index) {
            temp = helper->next;
            helper->next = helper->next->next;
            free(temp);
            return head;
        }
        helper = helper->next;
        currIndex++;
    }

    // Check to see if the index is out of range
    if(index > currIndex + 1){
        return head;
    }

    // At this point, we know the target index is the last point in the list. Delete the end.
    return iDeleteEnd(head);
};

// Same as index, but searching for the ID
struct ll_item * iDeleteItemByID(struct ll_item *head, int id) {
    struct ll_item *helper = head, *temp;

    if(head == NULL){
        return head;
    }

    if(head->data.id == id){
        temp = helper;
        helper = helper->next;
        free(temp);
        return helper;
    }

    while(helper->next->next != NULL){
        if(helper->next->data.id == id){
            temp = helper->next;
            helper->next = helper->next->next;
            free(temp);
            return head;
        }
        helper = helper->next;
    }

    return iDeleteEnd(head);
};

// Removes the last element in the linked list
struct ll_item * iDeleteEnd(struct ll_item *head) {
    struct ll_item *helper = head;

    if(head==NULL) {
        return head;
    }

    if(helper->next == NULL) {
        free(helper);
        return NULL;
    }

    // Loop until we have the second to last item in the list
    while(helper->next->next != NULL) {
        helper = helper->next;
    }

    // Free the last item in the list
    free(helper->next);
    // Remove any reference to it
    helper->next = NULL;

    return head;
};

// Just does end over and over again
struct ll_item * iDeleteAll(struct ll_item *head) {
    while(head!= NULL) {
        head = iDeleteEnd(head);
    }

    return NULL;
};

struct ll_room * rAppend(struct ll_room *head, struct room data) {
    struct ll_room *newEntry = (struct ll_room*) malloc(sizeof(struct ll_room));
    struct ll_room *helper = head;

    newEntry->data = data;
    newEntry->next = NULL;

    if(head == NULL) {
        return newEntry;
    }

    while(helper->next != NULL) {
        helper = helper->next;
    }

    helper->next = newEntry;

    return head;
};

struct ll_room * rFindRoomByIndex(struct ll_room *head, int index) {
    struct ll_room *helper = head;
    int currIndex = 0;

    while(helper != NULL) {
        if(currIndex == index) {
            return helper;
        } else {
            helper = helper->next;
            currIndex++;
        }
    }

    return head;
};

struct ll_room * rFindRoomByID(struct ll_room *head, int id) {
    struct ll_room *helper = head;

    while(helper != NULL) {
        if(helper->data.id == id){
            return helper;
        } else {
            helper = helper->next;
        }
    }

    return NULL;
};

struct ll_room * rDeleteRoomByIndex(struct ll_room *head, int index) {
    struct ll_room * helper = head, *temp;
    int currIndex = 0;

    if(head==NULL) {
        return head;
    }

    // Delete if the index is now
    if(currIndex == index){
        temp = helper;
        helper = helper->next;
        free(temp);
        return helper;
    }

    // Check for middle indexes
    while (helper->next->next != NULL) {
        if((currIndex+1) == index) {
            temp = helper->next;
            helper->next = helper->next->next;
            free(temp);
            return head;
        }
        helper = helper->next;
        currIndex++;
    }

    // Check to see if the index is out of range
    if(index > currIndex + 1){
        return head;
    }

    // At this point, we know the target index is the last point in the list. Delete the end.
    return rDeleteEnd(head);
};

struct ll_room * rDeleteRoomByID(struct ll_room *head, int id) {
    struct ll_room *helper = head, *temp;

    if(head == NULL){
        return head;
    }

    if(head->data.id == id){
        temp = helper;
        helper = helper->next;
        free(temp);
        return helper;
    }

    while(helper->next->next != NULL){
        if(helper->next->data.id == id){
            temp = helper->next;
            helper->next = helper->next->next;
            free(temp);
            return head;
        }
        helper = helper->next;
    }
    return rDeleteEnd(head);
};

struct ll_room * rDeleteEnd(struct ll_room *head) {
    struct ll_room *helper = head;

    if(head==NULL) {
        return head;
    }

    if(helper->next == NULL) {
        free(helper);
        return NULL;
    }

    // Loop until we have the second to last item in the list
    while(helper->next->next != NULL) {
        helper = helper->next;
    }

    // Free the last item in the list
    free(helper->next);
    // Remove any reference to it
    helper->next = NULL;

    return head;
};

struct ll_room * rDeleteAll(struct ll_room *head) {
    while(head!= NULL) {
        head = rDeleteEnd(head);
    }

    return NULL;
};

struct ll_door * dAppend(struct ll_door *head, struct door data) {
    struct ll_door *newEntry = (struct ll_door*) malloc(sizeof(struct ll_door));
    struct ll_door *helper = head;

    newEntry->data = data;
    newEntry->next = NULL;

    if(head == NULL) {
        return newEntry;
    }

    while(helper->next != NULL) {
        helper = helper->next;
    }

    helper->next = newEntry;

    return head;
};

struct ll_door * dFindDoorByIndex(struct ll_door *head, int index) {
    struct ll_door *helper = head;
    int currIndex = 0;

    while(helper != NULL) {
        if(currIndex == index) {
            return helper;
        } else {
            helper = helper->next;
            currIndex++;
        }
    }

    return head;
};

struct ll_door * dDeleteDoorByIndex(struct ll_door *head, int index) {
    struct ll_door * helper = head, *temp;
    int currIndex = 0;


    if(head==NULL) {
        return head;
    }

    // Delete if the index is now
    if(currIndex == index){
        temp = helper;
        helper = helper->next;
        free(temp);
        return helper;
    }

    // Check for middle indexes
    while (helper->next->next != NULL) {
        if((currIndex+1) == index) {
            temp = helper->next;
            helper->next = helper->next->next;
            free(temp);
            return head;
        }
        helper = helper->next;
        currIndex++;
    }

    // Check to see if the index is out of range
    if(index > currIndex + 1){
        return head;
    }

    // At this point, we know the target index is the last point in the list. Delete the end.
    return dDeleteEnd(head);
};

struct ll_door * dDeleteEnd(struct ll_door *head) {
    struct ll_door *helper = head;

    if(head==NULL) {
        return head;
    }

    if(helper->next == NULL) {
        free(helper);
        return NULL;
    }

    // Loop until we have the second to last item in the list
    while(helper->next->next != NULL) {
        helper = helper->next;
    }

    // Free the last item in the list
    free(helper->next);
    // Remove any reference to it
    helper->next = NULL;

    return head;
};

struct ll_door * dDeleteAll(struct ll_door *head) {
    while(head!= NULL) {
        head = dDeleteEnd(head);
    }

    return NULL;
};

struct ll_combo * cAppend(struct ll_combo *head, struct combo data) {
    struct ll_combo *newEntry = (struct ll_combo*) malloc(sizeof(struct ll_combo));
    struct ll_combo *helper = head;

    newEntry->data = data;
    newEntry->next = NULL;

    if(head == NULL) {
        return newEntry;
    }

    while(helper->next != NULL) {
        helper = helper->next;
    }

    helper->next = newEntry;

    return head;
};

struct ll_combo * cFindComboByIndex(struct ll_combo *head, int index) {
    struct ll_combo *helper = head;
    int currIndex = 0;

    while(helper != NULL) {
        if(currIndex == index) {
            return helper;
        } else {
            helper = helper->next;
            currIndex++;
        }
    }

    return head;
};

struct ll_combo * cDeleteComboByIndex(struct ll_combo *head, int index) {
    struct ll_combo * helper = head, *temp;
    int currIndex = 0;

    if(head==NULL) {
        return head;
    }

    // Delete if the index is now
    if(currIndex == index){
        temp = helper;
        helper = helper->next;
        free(temp);
        return helper;
    }

    // Check for middle indexes
    while (helper->next->next != NULL) {
        if((currIndex+1) == index) {
            temp = helper->next;
            helper->next = helper->next->next;
            free(temp);
            return head;
        }
        helper = helper->next;
        currIndex++;
    }

    // Check to see if the index is out of range
    if(index > currIndex + 1){
        return head;
    }

    // At this point, we know the target index is the last point in the list. Delete the end.
    return cDeleteEnd(head);
};

struct ll_combo * cDeleteEnd(struct ll_combo *head) {
    struct ll_combo *helper = head;

    if(head==NULL) {
        return head;
    }

    if(helper->next == NULL) {
        free(helper);
        return NULL;
    }

    // Loop until we have the second to last item in the list
    while(helper->next->next != NULL) {
        helper = helper->next;
    }

    // Free the last item in the list
    free(helper->next);
    // Remove any reference to it
    helper->next = NULL;

    return head;
};

struct ll_combo * cDeleteAll(struct ll_combo *head) {
    while(head!= NULL) {
        head = cDeleteEnd(head);
    }

    return NULL;
};

// This function generates a new blank gameWorld and initializes variables all proper-like
struct world initWorld(void){
    struct world gameWorld;

    // ItemData must be null, otherwise the functions segFault
    gameWorld.inventory.numItems = 0;
    gameWorld.inventory.itemData = NULL;

    // Same here
    gameWorld.itemPrototypes.numItems = 0;
    gameWorld.itemPrototypes.itemData = NULL;

    // Again
    gameWorld.recipes.numCombos = 0;
    gameWorld.recipes.data = NULL;

    gameWorld.worldRooms.numRooms = 0;
    gameWorld.worldRooms.roomData = NULL;

    // Using smartStringCopy to prevent any overflow, memory leaks, or segfaults
    smartStringCopy(&gameWorld.introMessage, "");
    gameWorld.playing = 0;

    gameWorld.currentRoomId = 0;

    return gameWorld;
};

// Generates a new room room object with id id, and initializes variables properly
struct room initRoom(int id){
    struct room newRoom;
    newRoom.id = id;
    // Again here, prevent segmentation faults further down the line
    smartStringCopy(&newRoom.name, "Blank Room");
    smartStringCopy(&newRoom.desc, "Blank");

    newRoom.inventory.itemData = NULL;
    newRoom.inventory.numItems = 0;

    newRoom.paths.paths = NULL;
    newRoom.paths.numDoors = 0;
    return newRoom;
};

// Simple helper to get the current room
struct room * getCurrentRoom(struct rooms worldRooms, int roomId){
    return &rFindRoomByID(worldRooms.roomData, roomId)->data;
};

// This function looks through recipes to find a combo with both target1ID and target2ID and returns it
struct combo *findCombo(struct combos recipes, int target1ID, int target2ID){
    struct ll_combo *helper;

    helper = recipes.data;

    while(helper != NULL){
        if(target1ID == helper->data.primaryID && target2ID == helper->data.secondaryID){
            return &helper->data;
        } else if(target2ID == helper->data.primaryID && target1ID == helper->data.secondaryID){
            return &helper->data;
        }
        helper = helper->next;
    }

    return NULL;
};

// Simple function to print the room and the player's items
void printRoom(struct room currentRoom, struct items playerItems){
    printf("\nYou're in the %s\n%s\n", currentRoom.name, currentRoom.desc);
    printPaths(currentRoom.paths);
    printItems(currentRoom.inventory, playerItems);
    return;
}

// This prints all the paths in the struct doors
void printPaths(struct doors possibleDoors){
    struct ll_door *helper;
    int printIndex = 0;

    if(possibleDoors.numDoors <= 0 || possibleDoors.paths == NULL){
        return;
    }

    helper = possibleDoors.paths;

    printf("You can see the following paths: \n");

    while(helper != NULL){
        printf(" %d) %s\n", printIndex + 1, helper->data.path);
        printIndex++;
        helper = helper->next;
    }

    return;
}

// Prints the items, both the ones in the room and the ones in the user's bag
void printItems(struct items roomItems, struct items bagItems){
    struct ll_item *helper;
    int printIndex = 0;

    // Only print the items if there actually are any
    if(roomItems.numItems > 0 && roomItems.itemData != NULL){
        printf("You can see the following items in the room: \n");
        helper = roomItems.itemData;
        while(helper != NULL){
            printf(" %d) %s\n", printIndex + 1, helper->data.name);
            helper = helper->next;
            printIndex++;
        }
    }

    if(bagItems.numItems > 0 && bagItems.itemData != NULL){
        printf("You have the following items in your bag: \n");
        helper = bagItems.itemData;
        while(helper != NULL){
            printf(" %d) %s\n", printIndex + 1, helper->data.name);
            helper = helper->next;
            printIndex++;
        }
    }
}

// Used for the editor, prints rooms in a kinda debug fancy way
void printRoomVerbose(struct room targetRoom){
    printf("| ID: %10d | Name: %19s |\n", targetRoom.id, targetRoom.name);
    printf("| Num Items: %3d | Num Doors: %14d |\n", targetRoom.inventory.numItems, targetRoom.paths.numDoors);
    printf("| Desc: %-36.36s |\n", targetRoom.desc);

    if(targetRoom.inventory.numItems > 0){
        printf("---------- Room Items -------\n");
        printItemsVerbose(targetRoom.inventory);
    } else {
        printf("----- Room has no items -----\n");
    }

    if(targetRoom.paths.numDoors > 0){
        printf("---------- Room Doors -------\n");
        printPathsVerbose(targetRoom.paths);
    } else {
        printf("----- Room has no doors -----\n");
    }
    return;
}

// Prints all items it is handed in a fancy table
void printItemsVerbose(struct items toPrint){
    struct ll_item *helper;
    if(toPrint.itemData == NULL){
        return;
    }
    printf("| ID  | Name             | Action Type | Desc                              | \n");
    helper = toPrint.itemData;
    while(helper != NULL){
        printf("| %-3d | %-16.16s | %-11d | %-33.33s |\n", helper->data.id, helper->data.name, helper->data.itemAction.actionType, helper->data.desc);
        helper = helper->next;
    }
}

// Prints all paths it is handed in a fancy table
void printPathsVerbose(struct doors toPrint){
    struct ll_door *helper;
    int index = 0;
    if(toPrint.paths == NULL){
        return;
    }
    printf("| Index | Destination | Description                                        |\n");
    helper = toPrint.paths;
    while(helper != NULL){
        printf("| %-5d | %-11d | %-50.50s |\n", index, helper->data.dest, helper->data.path);
        helper = helper->next;
        index++;
    }
}

// Prints all rooms it is handed in a fancy table
void printRoomsVerbose(struct rooms toPrint){
    struct ll_room *helper;
    int index = 0;
    if(toPrint.roomData == NULL){
        return;
    }
    printf("| ID  | NumDoors | Name                                                    |\n");
    helper = toPrint.roomData;
    while(helper != NULL){
        printf("| %-3d | %-8d | %-55.55s |\n", helper->data.id, helper->data.paths.numDoors, helper->data.name);
        helper = helper->next;
        index++;
    }
}

// same but combos
void printCombosVerbose(struct combos toPrint){
    struct ll_combo *helper;
    int index = 0;
    if(toPrint.data == NULL){
        return;
    }
    printf("| Index | Primary | Secondary | Result | Desc                             |\n");
    helper = toPrint.data;
    while(helper != NULL){
        printf("| %-5d | %-7d | %-9d | %-6d | %-32.32s |\n", index, helper->data.primaryID, helper->data.secondaryID, helper->data.resultID, helper->data.desc);
        // on string formatting - the negative prefix and a number mean it needs 32 spaces, with the text left justified
        //                      - the dot prefix and a number means it will shorten the string to a max of 32 spaces while printing
        helper = helper->next;
        index++;
    }
}

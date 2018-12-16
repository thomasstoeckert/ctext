/*  Thomas Stoeckert
    COP 3223H - Final Project

    IO.c handles all file dealings, as well as some utility string functions like getLine.
    This file is responsible mostly for loading and saving worlds. This is used - understandably - quite regularly
*/

#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// ---- Editor & Player Function ---- //
// This function will return a dynamically allocated array of characters from src, which can be any FILE pointer
// This will go until a newline or EOF is found.
// stdin is a file pointer, so this can be used with user input directly
char * getLine(FILE * src){
    char *line = NULL, *temp = NULL;
    size_t size = 0, index = 0;
    int inChar = EOF, hasReadText = 0;

    // I use 256 as a flag to end the loop since it doesn't show up as an ascii character (0-255)
    while(inChar != 256) {
        inChar = getc(src);

        if(inChar == EOF || inChar == '\n') {
            inChar = 256;
        }

        // Ignore any spaces until we have read a character. This prevents any extra spacing from the front
        if(inChar == ' ' && !hasReadText) {
            continue;
        } else if (inChar != ' ' && !hasReadText) {
            hasReadText = 1;
        }

        if(size <= index) {
            size += CHUNK;
            temp = realloc(line, size);
            if(temp == NULL){
                free(line);
                line = NULL;
                break;
            }
            line = temp;
        }

        line[index] = inChar;
        index++;
    }

    return line;
};

// ---- Editor & Player Function ---- //
// Pester the user over and over until they give us a string that they agree with
char * getValidatedUserString(char *question){
    char *string;
    int validated = 0;
    char userChoice;

    printf(question);
    while(!validated){
        string = getLine(stdin);
        printf("You entered: \"%s\". Is this what you want (y/n)?\n", string);
        userChoice = tolower(getchar());
        clearBuffer();
        if(userChoice == 'y'){
            validated = 1;
        } else {
            printf("Please enter your string again.\n");
        }
    }
    return string;
}

// ----- Editor & Player Function ---- //
// Very simple function - eats any and all characters until we hit a newline
void clearBuffer(void){
    while ((getchar()) != '\n');
}

int isInIntArray(int * toSearch, int toFind, int toSearchLength){
    for(int i = 0; i < toSearchLength; i++){
        if(toFind == toSearch[i]){
            return 1;
        }
    }
    return 0;
}

// ---- Editor & Player Function ---- //
// Requests the user for the filename then opens it into a file pointer. Repeats until the file pointer is valid.
FILE * openFileFromUserPrompt(void){
    FILE * fp;
    char *filename;

    printf("What is the name of the world file you'd like to open?\n");
    filename = getLine(stdin);
    fp = fopen(filename, "r");

    while(fp == NULL) {
        printf("File not able to be read. Please try again.\n");
        filename = getLine(stdin);
        fp = fopen(filename, "r");
    }
    return fp;
};

// ---- Editor Function ---- //
// Same as openFile, but with different words
FILE * saveFileFromUserPrompt(void){
    FILE * fp;
    char *filename;

    printf("What would you like to name the saved file?\n");
    filename = getLine(stdin);
    fp = fopen(filename, "w");
    while(fp == NULL){
        printf("File not able to be opened. Please try again.\n");
        filename = getLine(stdin);
        fp = fopen(filename, "w");
    }
    return fp;
}

// ---- Editor & Player Function ---- //
// This takes in a file pointer then dissects the file to generate the proper world
// It returns a (hopefully) valid world object with all the information from the file
struct world loadWorldFromFile(FILE * ifp){
    struct world world = initWorld();

    world.introMessage = getLine(ifp);

    world.itemPrototypes = buildItemsFromFile(ifp);

    world.worldRooms = buildRoomsFromFile(ifp);

    distributeItemsFromFile(ifp, world.worldRooms.roomData, world.itemPrototypes.itemData);

    world.recipes = buildCombosFromFile(ifp);

    world.playing = 1;

    printf("World Loaded.\n");
    return world;
};

// ---- Player & Editor File ---- //
// This takes in a file pointer and scans through it, building items as it goes,
//  returning that after it is completed
struct items buildItemsFromFile(FILE * ifp){
    int actionType, newId, targetItem, targetRoom, targetDoor, targetDest, targetQuantity, i = 0, pickable;
    struct items newItems;

    newItems.itemData = NULL;
    newItems.numItems = 0;

    fscanf(ifp, "%d", &newId);

    while(newId>= 0){
        struct item currentItem;
        fscanf(ifp, "%d", &pickable);
        fscanf(ifp, "%d", &actionType);

        currentItem.id = newId;
        currentItem.pickable = pickable;
        currentItem.name = getLine(ifp);
        currentItem.desc = getLine(ifp);

        currentItem.itemAction.actionType = actionType;
        switch(actionType){
            case -1:
                // -1 does nothing.
                break;
            case 0:
                fscanf(ifp, "%d %d", &targetItem, &targetQuantity);
                currentItem.itemAction.itemTarget = targetItem;
                currentItem.itemAction.quantity = targetQuantity;
                currentItem.itemAction.desc = getLine(ifp);
                break;
            case 1:
                fscanf(ifp, "%d", &targetItem);
                currentItem.itemAction.itemTarget = targetItem;
                currentItem.itemAction.desc = getLine(ifp);
                break;
            case 2:
                fscanf(ifp, "%d %d %d", &targetRoom, &targetDoor, &targetDest);
                currentItem.itemAction.roomTarget = targetRoom;
                currentItem.itemAction.doorTarget = targetDoor;
                currentItem.itemAction.destTarget = targetDest;
                currentItem.itemAction.desc = getLine(ifp);
                break;
            case 3:
                currentItem.itemAction.desc = getLine(ifp);
                break;
        }
        i++;
        newItems.nextID = newId + 1;
        newItems.itemData = iAppend(newItems.itemData, currentItem);
        fscanf(ifp, "%d", &newId);
    }
    newItems.numItems = i;
    printf("%d items loaded.\n", newItems.numItems);
    return newItems;
};

// ---- Player & Editor Function ---- //
// This function takes in a pointer to the input file and returns the rooms generated from it
struct rooms buildRoomsFromFile(FILE * ifp) {
    int roomId, numDoors, pathDest, i = 0;
    struct rooms newRooms;
    struct room newRoom;
    struct door newDoor;

    newRooms.roomData = NULL;

    fscanf(ifp, "%d", &roomId); // Again, used as flag and data

    while(roomId >= 0){
        // It's important to establish the data pointers as null
        newRoom.inventory.itemData = NULL;
        newRoom.inventory.numItems = 0;

        fscanf(ifp, "%d", &numDoors);
        newRoom.paths.paths = NULL;
        newRoom.paths.numDoors = numDoors;

        newRoom.name = getLine(ifp);
        newRoom.desc = getLine(ifp);
        newRoom.id = roomId;

        for(int j = 0; j < numDoors; j++) {
            fscanf(ifp, "%d", &pathDest);
            newDoor.dest = pathDest;

            newDoor.path = getLine(ifp);
            newRoom.paths.paths = dAppend(newRoom.paths.paths, newDoor);
        }

        newRooms.roomData = rAppend(newRooms.roomData, newRoom);
        i++;
        newRooms.nextID = roomId + 1;
        fscanf(ifp, "%d", &roomId);
    }
    newRooms.numRooms = i;
    printf("All %d rooms loaded with nextID as %d\n", i, newRooms.nextID);
    return newRooms;
};

// ---- Player & Editor Function ---- //
// This function takes in the input file pointer and returns a fully defined combos struct
struct combos buildCombosFromFile(FILE * ifp) {
    int primaryID, secondaryID, resultID, numCombos = 0;
    struct combos newCombos;
    struct combo temp;

    newCombos.data = NULL;
    fscanf(ifp, "%d", &primaryID);

    while(primaryID >= 0){
        fscanf(ifp, "%d %d", &secondaryID, &resultID);

        temp.primaryID = primaryID;
        temp.secondaryID = secondaryID;
        temp.resultID = resultID;
        temp.desc = getLine(ifp);

        newCombos.data = cAppend(newCombos.data, temp);

        fscanf(ifp, "%d", &primaryID);
        numCombos++;
    }

    newCombos.numCombos = numCombos;
    return newCombos;
};

// ---- Player & Editor Function ---- //
// This function takes in the parameters from the file to copy an item from the protos to the appropriate room
void distributeItemsFromFile(FILE * ifp, struct ll_room *worldRooms, struct ll_item *protos){
    int targetItemID, targetRoomId, itemCount = 0;
    struct item newItem; // Items are clones, since they're prototypes
    struct room *targetRoom; // Room are pointers since they're supposed to be maintained, not cloned.

    fscanf(ifp, "%d", &targetItemID); // Functions both as the targetItemID and a flag for reading i/o

    while(targetItemID >= 0) {
        fscanf(ifp, "%d", &targetRoomId);

        newItem = iFindItemByID(protos, targetItemID)->data;
        targetRoom = &rFindRoomByID(worldRooms, targetRoomId)->data;

        // Assign item to the inventory
        targetRoom->inventory.itemData = iAppend(targetRoom->inventory.itemData, newItem);
        targetRoom->inventory.numItems++;

        fscanf(ifp, "%d", &targetItemID);

        itemCount++;
    }
    printf("Distributed %d Items\n", itemCount);
    return;
};

// ---- Editor Function ---- //
// This is the inverse function of loadWorldFromFile, taking the world data and writing it to a file pointer
void saveWorldToFile(FILE * ofp, struct world data){
    //First part of any of these files is the printing of the welcome message.
    fprintf(ofp, "%s\n", data.introMessage);

    //Next segment: Items
    saveItemsToFile(ofp, data.itemPrototypes);

    saveRoomsToFile(ofp, data.worldRooms);

    // Item distribution isn't stored in the game directly, so item distribution must be calculated using the rooms
    saveDistroToFile(ofp, data.worldRooms, data.itemPrototypes);

    saveCombosToFile(ofp, data.recipes);

    return;
}

void saveItemsToFile(FILE * ofp, struct items protos){
    struct ll_item *helper;
    struct item currentItem;
    struct action currentAction;

    if(protos.itemData == NULL){
        fprintf(ofp, "%d/n", -1); // Sentinels are still printed, so rooms lacking one or more sections can still be loaded
        return;
    }

    helper = protos.itemData;

    while(helper != NULL){

        currentItem = helper->data;
        currentAction = currentItem.itemAction;

        fprintf(ofp, "%d %d %d %s\n", currentItem.id, currentItem.pickable, currentAction.actionType, currentItem.name);
        fprintf(ofp, "%s\n", currentItem.desc);
        switch(currentAction.actionType){
            case -1:
                break;
            case 0:
                fprintf(ofp, "%d %d %s\n", currentAction.itemTarget, currentAction.quantity, currentAction.desc);
                break;
            case 1:
                fprintf(ofp, "%d %s\n", currentAction.itemTarget, currentAction.desc);
                break;
            case 2:
                fprintf(ofp, "%d %d %d %s\n", currentAction.roomTarget, currentAction.doorTarget, currentAction.destTarget, currentAction.desc);
                break;
            case 3:
                fprintf(ofp, "%s\n", currentAction.desc);
                break;
        }
        helper = helper->next;
    }

    fprintf(ofp, "%d\n", -1); // All sections are ended by a -1

    printf("Saved items.\n");
    return;
}

// Most of the following save functions are similar - iterate through a linked-list, printing
//  variables in their expected file format along the way

void saveRoomsToFile(FILE * ofp, struct rooms protos){
    struct ll_room *helper;
    struct room currentRoom;

    if(protos.roomData == NULL){
        fprintf(ofp, "%d\n", -1);
        printf("No rooms to save.\n");
        return;
    }

    helper = protos.roomData;

    while(helper != NULL){
        currentRoom = helper->data;
        fprintf(ofp, "%d %d %s\n", currentRoom.id, currentRoom.paths.numDoors, currentRoom.name);
        fprintf(ofp, "%s\n", currentRoom.desc);
        saveDoorsToFile(ofp, currentRoom.paths);
        helper = helper->next;
    }

    fprintf(ofp, "%d\n", -1);
    printf("Rooms have been saved.\n");
    return;
}

void saveDoorsToFile(FILE *ofp, struct doors protos){
    struct ll_door *helper;
    struct door currentDoor;

    if(protos.paths == NULL){
        printf("No doors to save.\n");
        return;
    }

    helper = protos.paths;

    while(helper != NULL){
        currentDoor = helper->data;
        fprintf(ofp, "%d %s\n", currentDoor.dest, currentDoor.path);
        helper = helper->next;
    }

    return;
}

void saveCombosToFile(FILE * ofp, struct combos protos){
    struct ll_combo *helper;
    struct combo currentCombo;

    if(protos.data == NULL){
        fprintf(ofp, "%d\n", -1);
        printf("No combos to save.\n");
        return;
    }

    helper = protos.data;

    while(helper != NULL) {
        currentCombo = helper->data;
        fprintf(ofp, "%d %d %d %s\n", currentCombo.primaryID, currentCombo.secondaryID, currentCombo.resultID, currentCombo.desc);
        helper = helper->next;
    }

    fprintf(ofp, "%d\n", -1);
    printf("Saved combos to file.\n");
    return;
}

// Item distribution is weird. Distribution is not stored in the game data, so it is reliant entirely upon actually
//  digging through the rooms and saving the ID and location of each item and the room it is in.
void saveDistroToFile(FILE * ofp, struct rooms protos, struct items itemPrototypes){
    struct ll_room *helper;
    struct ll_item *itemHelper;
    struct room currentRoom;
    struct item currentItem;

    if(protos.roomData == NULL){
        fprintf(ofp, "%d\n", -1); // We still print our sentinel, since games without items distributed still matter <3
        printf("No rooms to save distributions from.\n");
        return;
    }

    // Establish our list of valid IDs
    int *validIDs = (int *) malloc(sizeof(int) * itemPrototypes.numItems);
    itemHelper = itemPrototypes.itemData;
    for(int i = 0; i < itemPrototypes.numItems; i++){
        validIDs[i] = itemHelper->data.id;
        itemHelper = itemHelper->next;
    }

    helper = protos.roomData;
    while(helper!=NULL){ // Loop through all rooms
        currentRoom = helper->data;

        if(currentRoom.inventory.itemData == NULL){
            helper = helper->next;
            continue;
        }

        itemHelper = currentRoom.inventory.itemData;

        while(itemHelper != NULL){ // Loop through all items in the room
            currentItem = itemHelper->data;
            // Make sure the item ID is valid
            if(!isInIntArray(validIDs, currentItem.id, itemPrototypes.numItems)){
                itemHelper = itemHelper->next;
                continue;
            }
            fprintf(ofp, "%d %d\n", currentItem.id, currentRoom.id);
            itemHelper = itemHelper->next;
        }
        helper = helper->next;
    }
    fprintf(ofp, "%d\n", -1);
    printf("Item Distributions saved.\n");
}

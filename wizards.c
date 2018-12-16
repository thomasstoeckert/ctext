#include "wizards.h"
#include "io.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

// Helper function, goes through and tries to get the int from user input. It will maintain the
//  original value if the string is null
void smartIntReplace(int *toAlter, int original){
    char *inString, *overflow;

    inString = getLine(stdin);
    if(inString[0] == '\0'){
        (*toAlter) = original;
    } else {
        (*toAlter) = strtol(inString, &overflow, 10);
    }
    free(inString);
    free(overflow);
    return;
}

// This guy frees the dest string, allocates the appropriate amount of space, and copies it over
void smartStringCopy(char **dest, char *src){
    int numChar = strlen(src) + 1;
    size_t srcSize = sizeof(char) * numChar;
    free((*dest));
    (*dest) = (char *) malloc(srcSize);
    strcpy((*dest), src);
}

// This function prints an item with all its variables
void wizardItemPrint(struct item* targetItem){
    printf("Your current item, named %s, is ", targetItem->name);
    if(targetItem->pickable){
        printf("pickable ");
    } else {
        printf("unpickable ");
    }
    printf("by the user, has an action type of %d, and has the following description: \n%s\n", targetItem->itemAction.actionType, targetItem->desc);
    printf("\nThe Item has the following action variables: \n");
    switch(targetItem->itemAction.actionType){
        case -1:
            printf(" - The item does nothing, and has no action variables - \n");
            break;
        case 0:
            printf(" Dispense ID: %d\n Dispense Quantity: %d\n Action Description: %s\n", targetItem->itemAction.itemTarget, targetItem->itemAction.quantity, targetItem->itemAction.desc);
            break;
        case 1:
            printf(" Transform ID: %d\n Transform Description: %s\n", targetItem->itemAction.itemTarget, targetItem->itemAction.desc);
            break;
        case 2:
            printf(" Room Target: %d\n Door Target: %d\n New Destination: %d\n Action Description: %s\n", targetItem->itemAction.roomTarget, targetItem->itemAction.doorTarget, targetItem->itemAction.destTarget, targetItem->itemAction.desc);
            break;
        case 3:
            printf(" End-Game Description: %s\n", targetItem->itemAction.desc);
            break;
    }
}

// Print all the action types
void wizardActionTypePrint(void){
    printf("\nAction Types: \n");
    printf(" -1 - No action. Item does nothing.\n");
    printf("  0 - Dispense another item upon use.\n");
    printf("  1 - Transform item into another upon use.\n");
    printf("  2 - Change a door's destination upon use.\n");
    printf("  3 - End the game upon use.\n");
}

// This wizard creates a new room based upon user prompts
struct room createRoomWizard(int id){
    struct room newRoom = initRoom(id);
    char choice;
    system("cls");
    clearBuffer();

    printf("Welcome to the room creation wizard!\n");
    printf("You're creating a room with the ID: %d\n", id);

    printf("Room Name: ");
    newRoom.name = getLine(stdin);

    printf("Room Description: ");
    newRoom.desc = getLine(stdin);

    // Validate to user their inputs
    printf("\n\nYour room has id %d and name %s. It has the following description: %s\n", newRoom.id, newRoom.name, newRoom.desc);
    printf("Do you want to keep this room(y/n)\n");
    choice = tolower(getchar());
    clearBuffer();
    if(choice == 'y'){
        return newRoom;
    } else {
        newRoom.id = -1;
        return newRoom;
    }
};

// Same but room
void editRoomWizard(struct room *targetRoom){
    char *tempName, *tempDesc;
    char choice;

    printf("Currently room %s with ID %d has the description of \n\n%s\n\n", targetRoom->name, targetRoom->id, targetRoom->desc);

    printf("Enter the new name (or enter nothing to preserve the old name): ");
    tempName = getLine(stdin);

    printf("Enter the new description (or enter nothing to preserve the old name): ");
    tempDesc = getLine(stdin);

    if(tempName[0] == '\0'){
        smartStringCopy(&tempName, targetRoom->name);
    }
    if(tempDesc[0] == '\0'){
        smartStringCopy(&tempDesc, targetRoom->desc);
    }

    printf("\nYour new data is \nname: %s\ndesc: %s\nWould you like to keep this change(y/n)?\n", tempName, tempDesc);
    scanf(" %c", &choice);
    choice = tolower(choice);
    clearBuffer();
    if(choice == 'y'){
        smartStringCopy(&targetRoom->name, tempName);
        smartStringCopy(&targetRoom->desc, tempDesc);
    }
    free(tempName);
    free(tempDesc);
    return;
}

// This creates a door
struct door createDoorWizard(){
    struct door newDoor;
    char confirm;

    printf("\n\n");
    printf("Enter the destination room's ID for this door (or -1 if you want it blocked): ");
    scanf("%d", &newDoor.dest);
    clearBuffer();
    printf("\nEnter a description of the door's path:\n");
    newDoor.path = getLine(stdin);

    printf("\nThis door has a dest of %d and a path of \"%s\". Do you want to keep this? (y/n)", newDoor.dest, newDoor.path);
    confirm = tolower(getchar());
    clearBuffer();
    if(confirm == 'y'){
        return newDoor;
    } else {
        newDoor.dest = DOOR_INVALID_FLAG;
        return newDoor;
    }
};

// Door editing
void editDoorWizard(struct door *targetDoor){
    char *tempPath;
    char confirm;

    int tempDest;
    printf("\n\nYou are editing a door with a destination ID of %d and a path description of \"%s\"\n\n", targetDoor->dest, targetDoor->path);
    printf("Enter the ID for the new door destination (or leave it blank if you want to maintain the old value): ");
    smartIntReplace(&tempDest, targetDoor->dest);

    printf("\n\nEnter the string for the new path description (or leave it blank if you want to maintain the old value)\n");
    tempPath = getLine(stdin);

    if(tempPath[0] == '\0'){
        // User wants to maintain the old path
        smartStringCopy(&tempPath, targetDoor->path);
    }

    printf("\n\nThe door now has a destination of %d and a path description of \"%s\". Do you want to keep these changes? (y/n)\n", tempDest, tempPath);
    confirm = tolower(getchar());
    clearBuffer();

    if(confirm == 'y'){
        targetDoor->dest = tempDest;
        smartStringCopy(&targetDoor->path, tempPath);
        return;
    }
    return;
}

// Item creation
struct item createItemWizard(int id){
    struct item newItem;
    char confirm;

    newItem.id = id;

    printf("Enter the name of the item: ");
    newItem.name = getLine(stdin);

    printf("Enter the description of the item: \n");
    newItem.desc = getLine(stdin);

    printf("\nIs the item able to be picked up? (y/n): ");
    confirm = tolower(getchar());
    clearBuffer();
    if(confirm == 'y'){
        newItem.pickable = 1;
    } else {
        newItem.pickable = 0;
    }

    wizardActionTypePrint();
    printf("\nEnter the item's action type: ");
    scanf("%d", &newItem.itemAction.actionType);
    clearBuffer();
    printf("\n");
    switch(newItem.itemAction.actionType){
        case -1:
            // Item has no action - no more action definitions needed
            break;
        case 0:
            // Item needs to get dispense definition
            printf("Enter the ID of the item to be dispensed: ");
            scanf("%d", &newItem.itemAction.itemTarget);
            clearBuffer();

            printf("Enter the quantity of the item to be dispensed, or -1 for unlimited dispenses: ");
            scanf("%d", &newItem.itemAction.quantity);
            clearBuffer();

            printf("Enter the description of what happens when the user interacts with the item: ");
            newItem.itemAction.desc = getLine(stdin);
            break;
        case 1:
            // Item needs to transform upon use
            printf("Enter the ID of the item that the original item transforms into: ");
            scanf("%d", &newItem.itemAction.itemTarget);
            clearBuffer();

            printf("Enter the description of what happens when the user interacts with the item.\n");
            newItem.itemAction.desc = getLine(stdin);
            break;
        case 2:
            // Item needs to change a door's destination upon use
            printf("Enter the ID of the room to target: ");
            scanf("%d", &newItem.itemAction.roomTarget);
            clearBuffer();

            printf("Enter the index of the door to target: ");
            scanf("%d", &newItem.itemAction.doorTarget);
            clearBuffer();

            printf("Enter the ID of the new destination: ");
            scanf("%d", &newItem.itemAction.destTarget);
            clearBuffer();

            printf("Enter the description of what happens when the user interacts with the item.\n");
            newItem.itemAction.desc = getLine(stdin);
            break;
        case 3:
            // Item ends the game upon use
            printf("Enter the description of what happens when the user interacts with the item.\n");
            newItem.itemAction.desc = getLine(stdin);
            break;
    }

    wizardItemPrint(&newItem);

    printf("\nDo you want to keep this item (y/n)? ");
    scanf(" %c", &confirm);
    clearBuffer();
    if(confirm == 'y'){
        return newItem;
    } else {
        newItem.id = -1;
        return newItem;
    }
}

// Edit Items
void editItemWizard(struct item *targetItem){
    char *tempName, *tempDesc, *tempActionDesc;
    char choice;

    int tempPickable;
    int tempAction, tempItemTarget, tempRoomTarget, tempDoorTarget, tempDestTarget, tempQuantity;

    wizardItemPrint(targetItem);

    printf("Enter the new name (or leave it blank to maintain the old name): ");
    tempName = getLine(stdin);
    if(tempName[0] == '\0'){
        smartStringCopy(&tempName, targetItem->name);
    }

    printf("Is the item pickable (leave blank to maintain the original value) (y/n): ");
    choice = getchar();
    clearBuffer();
    choice = tolower(choice);
    if(choice == 'y'){
        tempPickable = 1;
    } else if(choice == 'n'){
        tempPickable = 0;
    } else {
        tempPickable = targetItem->pickable;
    }

    printf("Enter the new description (or leave it blank to maintain the old name): ");
    tempDesc = getLine(stdin);
    if(tempDesc[0] == '\0'){
        smartStringCopy(&tempDesc, targetItem->desc);
    }

    wizardActionTypePrint();
    printf("Enter the new action type (or leave blank to maintain): ");
    smartIntReplace(&tempAction, targetItem->itemAction.actionType);

    switch(tempAction){
        case -1:
            break;
        case 0:
            printf("Enter the ID of the item to dispense (or leave blank to maintain): ");
            smartIntReplace(&tempItemTarget, targetItem->itemAction.itemTarget);

            printf("Enter the Quantity of the item to dispense (or leave blank to maintain): ");
            smartIntReplace(&tempQuantity, targetItem->itemAction.quantity);

            printf("Enter the description of the action (or leave blank to maintain): ");
            tempActionDesc = getLine(stdin);
            if(tempActionDesc[0] == '\0'){
                smartStringCopy(&tempActionDesc, targetItem->itemAction.desc);
            }
            break;
        case 1:
            printf("Enter the ID of the item to transform into (or leave blank to maintain): ");
            smartIntReplace(&tempItemTarget, targetItem->itemAction.itemTarget);

            printf("Enter the description of the action (or leave blank to maintain): ");
            tempActionDesc = getLine(stdin);
            if(tempActionDesc[0] == '\0'){
                smartStringCopy(&tempActionDesc, targetItem->itemAction.desc);
            }
            break;
        case 2:
            printf("Enter the ID of the targeted room (or leave blank to maintain): ");
            smartIntReplace(&tempRoomTarget, targetItem->itemAction.roomTarget);

            printf("Enter the index of the targeted door (or leave blank to maintain): ");
            smartIntReplace(&tempDoorTarget, targetItem->itemAction.doorTarget);

            printf("Enter the new destination of the targeted door (or leave blank to maintain): ");
            smartIntReplace(&tempDestTarget, targetItem->itemAction.destTarget);

            printf("Enter the description of the action (or leave blank to maintain): ");
            tempActionDesc = getLine(stdin);
            if(tempActionDesc[0] == '\0'){
                smartStringCopy(&tempActionDesc, targetItem->itemAction.desc);
            }
            break;
        case 3:
            printf("Enter the game-over description (or leave blank to maintain): ");
            tempActionDesc = getLine(stdin);
            if(tempActionDesc[0] == '\0'){
                smartStringCopy(&tempActionDesc, targetItem->itemAction.desc);
            }
            break;
    }

    printf("Review your entries. Do you want to keep these changes(y/n): ");
    scanf(" %c", &choice);
    choice = tolower(choice);
    clearBuffer();
    if(choice == 'y'){
        smartStringCopy(&targetItem->name, tempName);
        smartStringCopy(&targetItem->desc, tempDesc);
        targetItem->pickable = tempPickable;
        targetItem->itemAction.actionType = tempAction;
        switch(tempAction){
            case 0:
                targetItem->itemAction.quantity = tempQuantity;
            case 1:
                targetItem->itemAction.itemTarget = tempItemTarget;
                smartStringCopy(&targetItem->itemAction.desc, tempActionDesc);
                break;
            case 2:
                targetItem->itemAction.roomTarget = tempRoomTarget;
                targetItem->itemAction.doorTarget = tempDoorTarget;
                targetItem->itemAction.destTarget = tempDestTarget;
            case 3:
                smartStringCopy(&targetItem->itemAction.desc, tempActionDesc);
                break;
        }
        return;
    }
}

// Create combo
struct combo createComboWizard(){
    struct combo newCombo;
    char confirm;

    printf("Enter the ID of the first ingredient: ");
    scanf("%d", &newCombo.primaryID);

    printf("Enter the ID of the second ingredient: ");
    scanf("%d", &newCombo.secondaryID);

    printf("Enter the ID of the resultant product: ");
    scanf("%d", &newCombo.resultID);
    clearBuffer();

    printf("Enter the description of what happens during the combination: ");
    newCombo.desc = getLine(stdin);

    printf("New combo takes ID %d and ID %d to produce %d. It's description is %s.\nDo you want to keep this (y/n)? ", newCombo.primaryID, newCombo.secondaryID, newCombo.resultID, newCombo.desc);
    scanf(" %c", &confirm);
    confirm = tolower(confirm);
    clearBuffer();

    if(confirm == 'y'){
        return newCombo;
    } else {
        newCombo.resultID = -1;
        return newCombo;
    }
};

// Editing combos
void editComboWizard(struct combo *targetCombo){
    int tempPrimary, tempSecondary, tempResult;
    char *tempDesc;
    char confirm;

    printf("The combo currently takes ID %d and ID %d to produce %d. Its description is \"%s\"\n", targetCombo->primaryID, targetCombo->secondaryID, targetCombo->resultID, targetCombo->desc);
    printf("\nEnter the new primary ID (or leave it blank to maintain): ");
    smartIntReplace(&tempPrimary, targetCombo->primaryID);

    printf("Enter the new secondary ID (or leave it blank to maintain): ");
    smartIntReplace(&tempSecondary, targetCombo->secondaryID);

    printf("Enter the resultant ID (or leave it blank to maintain): ");
    smartIntReplace(&tempResult, targetCombo->resultID);

    printf("Enter the description of the combination (or leave it blank to maintain): ");
    tempDesc = getLine(stdin);
    if(tempDesc[0] == '\0'){
        strcpy(tempDesc, targetCombo->desc);
    }

    printf("Review what you have entered. Do you want to keep these changes? (y/n) ");
    scanf(" %c", &confirm);
    confirm = tolower(confirm);
    clearBuffer();

    if(confirm == 'y'){
        targetCombo->primaryID = tempPrimary;
        targetCombo->secondaryID = tempSecondary;
        targetCombo->resultID = tempResult;
        smartStringCopy(&targetCombo->desc, tempDesc);
        free(tempDesc);
        return;
    }
}

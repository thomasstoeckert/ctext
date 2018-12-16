/*  Thomas Stoeckert
    COP 3223H - Final Project

    Verbs handles pretty much all user input. This file is used both by the "Player" and the "Editor"
    As such, functions used exclusively by one will be called out in commments.

    This file is mostly entered through handlePlayerVerb or handleEditorVerb, with those functions
     then dealing out data and requests to the other verbs in the file. In some simple cases, such as
     ending the game or quitting, it can be done without the other verbs files, but it's not common.

    */



#include "verbs.h"
#include "io.h"
#include "actions.h"
#include "wizards.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Used by the Player mode.
// This function takes in a pointer to the game data, and then prompts the user for their commands
//  The behavior after the user inputs their command varies from command to command, but it handles (safely)
//  all logic regarding that, only stopping when the command has been fully fulfilled.
void handlePlayerVerb(struct world *gamedata){
    char command;
    int target, secondTarget; // target is used for pretty much any interactive verb. second is only used for combine.

    struct doors currentRoomPaths; // When we're navigating rooms, we don't alter the doors. It's safer to use a copy.
    struct items *currentRoomItems; // Items in a room will change, so we point back to the original.

    printf("Enter a command: ");      // This block right here is used quite a bit throughout the program.
    scanf(" %c", &command);           //  Easy entry, easy formatting, then clearing the screen to help clean it all up.
    command = tolower(command);
    system("cls");

    switch(command){
        default:
            // If the user has no idea what's going on, help them.
            printf("Unknown Command: %c\n", command);
            printPlayerHelp();
            clearBuffer();
            break;
        case 'g':
            // Go - the user is going somewhere. They point to the index of the door/path, then handleGoVerb attempts
            //       to traverse said path.
            currentRoomPaths = getCurrentRoom(gamedata->worldRooms, gamedata->currentRoomId)->paths;
            scanf("%d", &target);
            target--; // Room & item indexes are 1-based when printed, but 0-based in code. Fixed here.
            handleGoVerb(currentRoomPaths, gamedata->worldRooms, &gamedata->currentRoomId, target);
            break;
        case 'i':
            // Inspect - the user is looking at something. They point to the index of an item in the room or world, then
            //            handleInspectVerb looks more into it.
            currentRoomItems = &getCurrentRoom(gamedata->worldRooms, gamedata->currentRoomId)->inventory;
            scanf("%d", &target);
            target--;
            handleInspectVerb((*currentRoomItems), gamedata->inventory, target);
            break;
        case 't':
            // Take - the user attempts to take an item from the room and place it into their inventory. Handled by
            //         handleTakeVerb
            currentRoomItems = &getCurrentRoom(gamedata->worldRooms, gamedata->currentRoomId)->inventory;
            scanf("%d", &target);
            target--;
            handleTakeVerb(currentRoomItems, &gamedata->inventory, target);
            break;
        case 'u':
            // Use - the user is attempting to use an item. Items have a few different behaviors, all laid out in
            //        actions.c. However, it is handleUserVerb's job to prepare everything for the action functions.
            scanf("%d", &target);
            target--;
            clearBuffer();
            handleUseVerb(gamedata, target);
            break;
        case 'c':
            // Combine - the user is combining items together to make a new one. It's a simple crafting system, really.
            //          It's also the most powerful tool in the engine, story-wise. It lets you use one item on another,
            //          or make a puzzle solvable by putting parts together. It's done with both room and inventory items.
            currentRoomItems = &getCurrentRoom(gamedata->worldRooms, gamedata->currentRoomId)->inventory;
            scanf("%d %d", &target, &secondTarget);
            target--;
            secondTarget--;
            handleCombineVerb(target, secondTarget, currentRoomItems, &gamedata->inventory, gamedata->recipes, gamedata->itemPrototypes);
            break;
        case 'q':
            // Quit - very simple confirm/deny quit function.
            clearBuffer();
            handleQuitVerb(&gamedata->playing);
            break;
    }
}

// Simple function to organize all the printing for user help
void printPlayerHelp(void){
    printf("------ Player Commands ------\n");
    printf(" g <door index> - Go through the door you select\n");
    printf(" i <item index> - inspect the item you select\n");
    printf(" t <item index> - take an item from the world and put it in your inventory\n");
    printf(" u <item index> - use an item from the world or your inventory\n");
    printf(" c <item index> <item index> - combine two items into a result\n");
    printf(" q - quits the game\n");
    return;
}



// ---- Player function ---- //
// This function helps organize the logic regarding handling the go command
// This picks the path at the target, and updates the pointer to the value of the
// destination if it's a valid location
void handleGoVerb(struct doors paths, struct rooms worldRooms, int * currentIndex, int target){
    struct ll_door *helper;
    int destination;

    if(paths.paths == NULL){
        printf("No Paths found when goVerb was called.\n");
        return;
    }

    helper = dFindDoorByIndex(paths.paths, target);

    if(helper == NULL){
        printf("Invalid Choice: %d\n", target);
        return;
    }

    destination = helper->data.dest;

    // This checks to see if the room destination is valid by checking to see if the room destination exists
    if(rFindRoomByID(worldRooms.roomData, destination) == NULL){
        printf("You attempt to go down this path, but it's blocked!\n");
        return;
    }

    (*currentIndex) = destination;
    return;
}

// ---- Player function ---- //
// This function finds the appropriate item from either the room or the bag and prints its
//  description to the screen
void handleInspectVerb(struct items roomItems, struct items bagItems, int target){
    struct item *targetItem;
    int sumItems = roomItems.numItems + bagItems.numItems;

    if(target >= sumItems || target < 0){
        printf("Invalid Item Selection");
        return;
    }

    if(target >= roomItems.numItems){
        targetItem = &iFindItemByIndex(bagItems.itemData, target - roomItems.numItems)->data;
    } else {
        targetItem = &iFindItemByIndex(roomItems.itemData, target)->data;
    }

    printf("%s\n", targetItem->desc);
    return;
}

// ---- Player function ---- //
// This function finds the appropriate item from the room, copies it to the user's inventory,
//  and deletes the original. Basically cloning. Ethically iffy with living things.
void handleTakeVerb(struct items *roomItems, struct items *bagItems, int target){
    struct item targetItem;

    if(target >= roomItems->numItems || target < 0){
        printf("Invalid Item Selection.\n");
        return;
    }

    targetItem = iFindItemByIndex(roomItems->itemData, target)->data;

    if(!targetItem.pickable) {
        printf("You seem to be unable to pick up the %s.\n", targetItem.name);
        return;
    }

    bagItems->itemData = iAppend(bagItems->itemData, targetItem);
    bagItems->numItems++;
    roomItems->itemData = iDeleteItemByIndex(roomItems->itemData, target);
    roomItems->numItems--;

    return;
}

// ---- Player function ---- //
// The 'Use' verb handles a ton of data related to using items. It's much simpler to pass it the game's data
//  instead of a half dozen variables.
// What this function does is it attempts to 'use' an item the user selects from the world according to
//  the defined action of the item
void handleUseVerb(struct world *gamedata, int target){
    struct item *targetItem;
    struct room *currentRoom = getCurrentRoom(gamedata->worldRooms, gamedata->currentRoomId);
    int sumItems = currentRoom->inventory.numItems + gamedata->inventory.numItems;

    if(target >= sumItems || target < 0){
        printf("Invalid Item Selection.\n");
        return;
    }

    if(target >= currentRoom->inventory.numItems){
        targetItem = &iFindItemByIndex(gamedata->inventory.itemData, target - currentRoom->inventory.numItems)->data;
    } else {
        targetItem = &iFindItemByIndex(currentRoom->inventory.itemData, target)->data;
    }

    handleAction(targetItem, gamedata, target);
    return;
}

// ---- Player function ---- //
// This function takes in two targets, all the items the user can interact with, and the combinations in the world
// These are then used to see if the user can make a combo with these two items (after validating their existence)
//  and then the result is placed in the right location.
void handleCombineVerb(int target, int secondTarget, struct items *roomItems, struct items *bagItems, struct combos recipes, struct items protos){
    int target1ID, target2ID, resultID;
    int target1InBag, target2InBag;
    int sumItems = roomItems->numItems + bagItems->numItems;
    struct combo *foundCombo;
    struct item resultItem;

    // Checking to see if the user has made a valid combination choice

    // Fist step of that - checking the item ranges
    int firstInvalid = 0, secondInvalid = 0;
    firstInvalid = (target > sumItems || target < 0);
    secondInvalid = (secondTarget > sumItems || target < 0);

    // Both items must be valid
    if(firstInvalid || secondInvalid){
        printf("Invalid items selection.\n");
        return;
    }

    // Second step of validation - checking if the user tried to add it to itself
    if(target == secondTarget){
        printf("You can't combine an item with itself!\n");
        return;
    }

    // Next part of combining an item is checking the origin of the two targets
    // We want to intelligently remove and add items despite their source - this helps us do that

    // Checking for the first item's location, and getting the ID of the item while we're at it
    if(target >= roomItems->numItems){
        // The first item is in the user's bag
        target1InBag = 1;
        target1ID = iFindItemByIndex(bagItems->itemData, target - roomItems->numItems)->data.id;
    } else {
        target1InBag = 0;
        target1ID = iFindItemByIndex(roomItems->itemData, target)->data.id;
    }

    if(secondTarget >= roomItems->numItems){
        target2InBag = 1;
        target2ID = iFindItemByIndex(bagItems->itemData, secondTarget - roomItems->numItems)->data.id;
    } else {
        target2InBag = 0;
        target2ID = iFindItemByIndex(roomItems->itemData, secondTarget)->data.id;
    }

    // Combinations store their data as IDs - we needed to convert our indexes to the item IDs before we passed it over
    foundCombo = findCombo(recipes, target1ID, target2ID);

    // findCombo returns null if there's no associated recipe
    if(foundCombo == NULL){
        printf("These two items don't seem to combine...\n");
        return;
    }

    // Getting our result information
    resultID = foundCombo->resultID;
    resultItem = iFindItemByID(protos.itemData, resultID)->data;

    // Informing the user...
    printf("%s\n", foundCombo->desc);

    // If both of the source items were in the user's bag, we put the result in the user's bag
    if(target1InBag && target2InBag){
        bagItems->itemData = iAppend(bagItems->itemData, resultItem);
        bagItems->numItems++;
    } else {
        // Otherwise, the result goes in the room
        roomItems->itemData = iAppend(roomItems->itemData, resultItem);
        roomItems->numItems++;
    }

    //Now we need to remove the source items according to their sources

    if(target1InBag){
        bagItems->itemData = iDeleteItemByID(bagItems->itemData, target1ID);
        bagItems->numItems--;
    } else {
        roomItems->itemData = iDeleteItemByID(roomItems->itemData, target1ID);
        roomItems->numItems--;
    }

    if(target2InBag){
        bagItems->itemData = iDeleteItemByID(bagItems->itemData, target2ID);
        bagItems->numItems--;
    } else {
        roomItems->itemData = iDeleteItemByID(roomItems->itemData, target2ID);
        roomItems->numItems--;
    }

    return;
}


// ---- Player & Editor function ---- //
// Very simple function. Just a double-check to see if the user is doing what they really want.
void handleQuitVerb(int *playing){
    char choice;
    printf("Are you sure you want to quit? (y/n) ");
    scanf(" %c", &choice);
    choice = tolower(choice);
    clearBuffer();
    if(choice == 'y'){
        // ONLY end if the user is certain.
        (*playing) = 0;
    }
    return;
}

// ---- Editor Function ---- //
// This function is the same as the player function, but it deals with multiple modes
// if the editor is in a different mode than the base mode, it shunts its responsibilities off
// to them instead.
void handleEditorVerb(struct world *gamedata){
    char mode = gamedata->mode;
    char *filename;
    char *tempText;
    char command, choice;

    printf("Enter a command: "); // Well aren't you familiar
    command = tolower(getchar());
    system("cls");


    // Again, if the user is in one of these modes we need to send their data to them instead
    switch(mode){
        case 'r':
            handleRoomEditorVerb(command, gamedata);
            return;
        case 'd':
            handleDoorEditorVerb(command, gamedata);
            return;
        case 'i':
            handleItemEditorVerb(command, gamedata);
            return;
        case 'c':
            handleComboEditorVerb(command, gamedata);
            return;
    }

    // At this point, we know we're in the main menu. We can now parse verbs as normal
    switch(command){
        // Here through 'c' are just verbs to enter other modes
        case 'r':
            printf("Entering Room Mode...\n");
            gamedata->mode = 'r';
            clearBuffer();
            return;
        case 'd':
            printf("Entering Door Mode...\n");
            gamedata->mode = 'd';
            clearBuffer();
            return;
        case 'i':
            printf("Entering Item Mode...\n");
            gamedata->mode = 'i';
            clearBuffer();
            return;
        case 'c':
            printf("Entering Combo mode...\n");
            gamedata->mode = 'c';
            clearBuffer();
            return;
        // Asks for filename, validates input, then saves it with io.h's saveWorldToFile
        case 's':
            clearBuffer();
            printf("Saving file...\n");

            // getValidatedUserString prompts the users with the string you pass, then loops until they agree with
            //  their selection. It's very serious and will not let up, ever. We always get a string back
            filename = getValidatedUserString("What would you like the files name to be?\n");
            FILE * ofp = fopen(filename, "w");
            saveWorldToFile(ofp, (*gamedata));

            // Release memory back into the wild
            fclose(ofp);
            free(filename);
            printf("File saved.\n");
            return;
        // Alter the text of the world - basically just the intro text. We do validate it, but it's akin to a wizard
        //  as seen in wizards.h
        // (this was a very last-minute add)
        case 't':
            // We're altering the intro text for the world file
            clearBuffer();
            system("cls");

            printf("The current intro text is: \n\t%s\n", gamedata->introMessage);
            printf("Enter new intro text to replace it:\n");
            tempText = getLine(stdin);
            if(tempText[0] == '\0'){
                smartStringCopy(&tempText, gamedata->introMessage);
            }

            printf("\nYour new intro text is: \n\t%s\n", tempText);
            printf("Do you want to keep it (y/n) ");

            scanf(" %c", &choice);
            choice = tolower(choice);
            clearBuffer();

            if(choice == 'y'){
                smartStringCopy(&gamedata->introMessage, tempText);
            }
            return;
        // Exactly the same as the playerVerb
        case 'q':
            clearBuffer();
            handleQuitVerb(&gamedata->playing);
            return;
        // This one's at the bottom for some reason
        default:
            clearBuffer();
            printEditorHelp(mode, command);
            return;
    }
}

// ---- Editor Function ---- //
// This function takes in the mode and command, then informs the user all possible
//  commands used in the specific mode that they are in.
void printEditorHelp(char mode, char command){
    printf("Unknown Command: %c\n", command);
    printf("---------- Commands for mode %c --------\n", mode);
    switch(mode){
        case 'r':
            // Room edit mode
            printf(" c - create a new room\n");
            printf(" e <room id> - edit an existing room\n");
            printf(" d <room id> - delete an existing room\n");
            printf(" j <room id> - jump to another room\n");
            printf(" q - quit the current mode and go back to the main mode.\n");
            break;
        case 'd':
            // Door Edit mode
            printf(" c - create a new door\n");
            printf(" e <door index> - edit an existing door\n");
            printf(" d <door index> - delete an existing door\n");
            printf(" q - quit the current mode and go back to the main mode.\n");
            break;
        case 'i':
            // Item edit mode
            printf(" c - create a new item\n");
            printf(" e <item id> - edit an existing item\n");
            printf(" d <item id> - delete an existing item\n");
            printf(" p <item id> - place an item into the current room\n");
            printf(" r <item id> - remove an item from the current room\n");
            printf(" q - quit the current mode and go back to the main mode.\n");
            break;
        case 'c':
            // Combo Edit mode
            printf(" c - create new combo\n");
            printf(" e <combo index> - edit an existing combo\n");
            printf(" d <combo index> - delete existing combo\n");
            printf(" q - quit the current mode and return to the main mode.\n");
            break;
        default:
            // Main menu
            printf(" r - enter room mode\n");
            printf(" d - enter door mode\n");
            printf(" i - enter item mode\n");
            printf(" c - enter combination mode\n");
            printf(" t - edit the welcome message\n");
            printf(" s - save the current world\n");
            printf(" q - quit the editor\n");
            break;
    }

}

// ---- Editor Function ---- //
// This function handles all the behavior of editing rooms. Most of the actual editing responsibilities
// lie upon wizards.h, but this is done to organize it and present it to the user.
void handleRoomEditorVerb(char command, struct world *gamedata){
    struct ll_room *targetRoom;
    struct room newRoom;
    char confirm;
    int target;

    switch(command){
        case 'c':
            clearBuffer();
            // User is creating a new room. Send them to the room wizard.
            newRoom = createRoomWizard(gamedata->worldRooms.nextID);
            if(newRoom.id == -1){
                // User ultimately didn't create a room
                return;
            }
            gamedata->worldRooms.roomData = rAppend(gamedata->worldRooms.roomData, newRoom);
            gamedata->worldRooms.numRooms++;
            gamedata->worldRooms.nextID++;
            return;
        case 'e':
            // Edit the room with the target ID
            scanf("%d", &target);
            clearBuffer();

            targetRoom = rFindRoomByID(gamedata->worldRooms.roomData, target);
            if(targetRoom == NULL){
                printf("Room with id %d not found.\n", target);
                return;
            }
            editRoomWizard(&targetRoom->data);
            return;
        case 'd':
            // Delete the room with the target ID
            scanf("%d", &target);
            clearBuffer();

            if(gamedata->worldRooms.numRooms <= 1){
                printf("You cannot delete the only room in the world.\n");
                return;
            }

            targetRoom = rFindRoomByID(gamedata->worldRooms.roomData, target);

            if(targetRoom == NULL){
                printf("Room with id %d not found.\n", target);
                return;
            }
            printf("You want to delete room %s with id %d. This cannot be undone.\n Are you sure (y/n)?\n", targetRoom->data.name, targetRoom->data.id);
            confirm = tolower(getchar());
            clearBuffer();
            if(confirm == 'y'){
                gamedata->worldRooms.roomData = rDeleteRoomByID(gamedata->worldRooms.roomData, target);
                gamedata->worldRooms.numRooms--;
                printf("The room was deleted.\n");
            }
            return;
        case 'j':
            // This is unique to the room edit mode, since we can now jump to rooms without doors
            // User is jumping to another room
            scanf("%d", &target);
            clearBuffer();
            targetRoom = rFindRoomByID(gamedata->worldRooms.roomData, target);
            if(targetRoom == NULL){
                printf("Room with id %d not found.\n", target);
                return;
            }
            gamedata->currentRoomId = target;
            printf("%d", gamedata->currentRoomId);
            return;
        case 'q':
            // Quitting these modes is less important than the player or main editor mode, so we don't verify here
            gamedata->mode = 'm';
            clearBuffer();
            return;
        default:
            clearBuffer();
            printEditorHelp(gamedata->mode, command);
            return;
    }
}

// ---- Editor function ---- //
// Handles the commands related to door editing
void handleDoorEditorVerb(char command, struct world *gamedata){
    struct ll_door *targetdoor;
    struct room *currentRoom;
    struct door newDoor;

    int target;
    char confirm;

    currentRoom = getCurrentRoom(gamedata->worldRooms, gamedata->currentRoomId);

    switch(command){
        default:
            clearBuffer();
            printEditorHelp(gamedata->mode, command);
            return;
        case 'c':
            clearBuffer();
            // User is creating a new door/path
            newDoor = createDoorWizard();
            if(newDoor.dest == DOOR_INVALID_FLAG){
                // User ultimately didn't want to make a door
                return;
            }
            currentRoom->paths.paths = dAppend(currentRoom->paths.paths, newDoor);
            currentRoom->paths.numDoors++;
            return;
        case 'e':
            scanf("%d", &target);

            clearBuffer();
            // User is editing an existing door/path
            targetdoor = dFindDoorByIndex(currentRoom->paths.paths, target);
            if(targetdoor == NULL){
                printf("Invalid door selection.\n");
                return;
            }
            editDoorWizard(&targetdoor->data);
            return;
        case 'd':
            // Establish the door we're looking for
            scanf("%d", &target);

            targetdoor = dFindDoorByIndex(currentRoom->paths.paths, target);
            if(targetdoor == NULL){
                printf("Invalid door selection.\n");
                return;
            }

            printf("You want to delete the door to dest %d with path \"%s\". Are you certain? This cannot be undone. (y/n) \n", targetdoor->data.dest, targetdoor->data.path);
            // For some reason the tolower(getchar()) pattern I've used on other editors didn't work here. This one did.
            // Odd.
            scanf(" %c", &confirm);
            confirm = tolower(confirm);
            clearBuffer();

            if(confirm == 'y'){
                currentRoom->paths.paths = dDeleteDoorByIndex(currentRoom->paths.paths, target);
                currentRoom->paths.numDoors--;
                printf("The path was deleted.\n");
            } else {
                printf("Confirm was not y");
            }
            return;
        case 'q':
            gamedata->mode = 'm';
            clearBuffer();
            return;
    }
}


// ---- Editor function ---- //
// Pretty much the same thing but with ITEMS WOO
void handleItemEditorVerb(char command, struct world* gamedata){
    struct ll_item *targetItem;
    struct room *currentRoom;
    struct item newItem;

    int target;
    char choice;

    currentRoom = getCurrentRoom(gamedata->worldRooms, gamedata->currentRoomId);

    switch(command){
        default:
            clearBuffer();
            printEditorHelp(gamedata->mode, command);
            return;
        case 'c':
            // User is creating a new item
            clearBuffer();
            newItem = createItemWizard(gamedata->itemPrototypes.nextID);
            if(newItem.id == -1){
                // User didn't want to keep the item they created. Discard it.
                printf("User has discarded the item");
                return;
            }
            gamedata->itemPrototypes.itemData = iAppend(gamedata->itemPrototypes.itemData, newItem);
            gamedata->itemPrototypes.numItems++;
            gamedata->itemPrototypes.nextID++;
            return;
        case 'e':
            // User is editing an existing item prototype
            scanf("%d", &target);
            clearBuffer();

            targetItem = iFindItemByID(gamedata->itemPrototypes.itemData, target);

            if(targetItem == NULL){
                printf("Invalid item ID\n");
                return;
            }

            editItemWizard(&targetItem->data);
            return;
        case 'd':
            // Delete an existing item prototype
            scanf("%d", &target);
            clearBuffer();

            targetItem = iFindItemByID(gamedata->itemPrototypes.itemData, target);

            if(targetItem == NULL){
                printf("Invalid item ID\n");
                return;
            }

            printf("You want to delete the item %s with id %d. Are you sure? Do note that the item prototypes placed already will not be deleted from the world until you save and load the file again. It may cause issues with combinations and other items actions. (y/n): ",
                   targetItem->data.name, targetItem->data.id);
            scanf(" %c", &choice);
            choice = tolower(choice);
            if(choice == 'y'){
                gamedata->itemPrototypes.itemData = iDeleteItemByID(gamedata->itemPrototypes.itemData, target);
                gamedata->itemPrototypes.numItems--;
            }
            return;
        case 'p':
            // Place a copy of an item prototype into the current room
            // NOTE: Changes made to an item prototype will not propagate to its clones until the file is saved and
            //  opened again. It's a bit odd, but it's a one-week game engine.
            scanf("%d", &target);
            clearBuffer();

            targetItem = iFindItemByID(gamedata->itemPrototypes.itemData, target);

            if(targetItem == NULL){
                printf("Invalid item ID\n");
                return;
            }

            currentRoom->inventory.itemData = iAppend(currentRoom->inventory.itemData, targetItem->data);
            currentRoom->inventory.numItems++;
            return;
        case 'r':
            // Remove a copy of an item from the current room
            scanf("%d", &target);
            clearBuffer();

            targetItem = iFindItemByID(currentRoom->inventory.itemData, target);

            if(targetItem == NULL){
                printf("Invalid item ID\n");
                return;
            }

            currentRoom->inventory.itemData = iDeleteItemByID(currentRoom->inventory.itemData, target);
            currentRoom->inventory.numItems--;
            return;
        case 'q':
            gamedata->mode = 'm';
            clearBuffer();
            return;
    }
}

// ---- Editor function ---- //
// Simplest of all modes for editing
void handleComboEditorVerb(char command, struct world *gamedata){
    struct ll_combo *targetCombo;
    struct combo newCombo;
    int target;

    switch(command){
        case 'c':
            // User is creating a new combo
            newCombo = createComboWizard();

            if(newCombo.resultID == -1){
                // User abandoned the combo. Do nothing
                return;
            }

            gamedata->recipes.data = cAppend(gamedata->recipes.data, newCombo);
            gamedata->recipes.numCombos++;
            break;
        case 'e':
            // User is editing an existing combo
            scanf("%d", &target);
            clearBuffer();

            targetCombo = cFindComboByIndex(gamedata->recipes.data, target);

            if(targetCombo == NULL){
                printf("Invalid combination index.\n");
                return;
            }

            editComboWizard(&targetCombo->data);
            break;;
        case 'd':
            // User is deleting an existing combo
            scanf("%d", &target);
            clearBuffer();
            targetCombo = cFindComboByIndex(gamedata->recipes.data, target);

            if(targetCombo == NULL){
                printf("Invalid combination index.\n");
                return;
            }

            gamedata->recipes.data = cDeleteComboByIndex(gamedata->recipes.data, target);
            gamedata->recipes.numCombos--;
            break;
        case 'q':
            gamedata->mode = 'm';
            clearBuffer();
            break;
    }
}

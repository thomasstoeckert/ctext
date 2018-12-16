/*  Thomas Stoeckert
    COP 3223H - Final Project

    Editor.c contains the main loop for the editor mode
*/

#include "editor.h"
#include "data.h"
#include "io.h"
#include "verbs.h"
#include <ctype.h>
#include <stdlib.h>

void editorLoop(void){
    struct room * currentRoom;
    struct world gamedata;
    char choice;

    // Prompt user for either a new or an old file
    printf("Welcome to the CText Editor. Would you like to open an existing world or create a new one?\n(e/n): ");
    scanf("%c", &choice);
    choice = tolower(choice);
    clearBuffer();

    while(choice != 'e' && choice != 'n'){
        printf("Invalid choice. Please try again.\n");
        scanf("%c", &choice);
        choice = tolower(choice);
        clearBuffer();
    }

    if(choice == 'e'){
        // User is opening an existing world. Behave much like a player.
        FILE * ifp = openFileFromUserPrompt();
        gamedata = loadWorldFromFile(ifp);
    } else {
        struct room basicRoom = initRoom(0);
        gamedata = initWorld();
        gamedata.worldRooms.roomData = rAppend(gamedata.worldRooms.roomData, basicRoom);
        gamedata.worldRooms.numRooms++;
    }
    gamedata.playing = 1;
    gamedata.mode = 'm';

    system("cls");

    while(gamedata.playing){
        printf("\n----- Editor (mode: %c) ------\n", gamedata.mode);
        currentRoom = getCurrentRoom(gamedata.worldRooms, gamedata.currentRoomId);
        switch(gamedata.mode){
            default:
                printRoomVerbose((*currentRoom));
                break;
            case 'r':
                printf("Currently in room ID %d\n", currentRoom->id);
                printRoomsVerbose(gamedata.worldRooms);
                break;
            case 'i':
                printf("Item Prototypes: \n");
                printItemsVerbose(gamedata.itemPrototypes);
                printf("Items in current room: \n");
                printItemsVerbose(currentRoom->inventory);
                break;
            case 'd':
                printPathsVerbose(currentRoom->paths);
                break;
            case 'c':
                printCombosVerbose(gamedata.recipes);
                break;
        }
        handleEditorVerb(&gamedata);
        //gamedata.playing = 0;
    }
}

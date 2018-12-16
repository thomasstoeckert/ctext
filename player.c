/*  Thomas Stoeckert
    COP 3223H - Final Project

    This file is the core of the Player mode, and consists of two major steps: Setting up the game
     and information from a file, and then the actual game loop itself. The game loop consists of
     informing the user of where they are, what they can do, and then dealing with their choices.

    */

#include "player.h"
#include "io.h"
#include "verbs.h"

#include <stdio.h>
#include <stdlib.h>

void playerLoop(void){
    FILE * ifp = NULL;
    struct world gamedata; // This holds pretty much everything in the game. This lets us pass information back and forth if need be.
    struct room *currentRoom; // A helper to help organize everything

    ifp = openFileFromUserPrompt();

    gamedata = loadWorldFromFile(ifp);

    // I made a decision (and assumption) that you'd be grading on a windows machine. As such, system("cls") is the best
    //  option that I knew of to clear the screen.
    // We really don't need to clear the screen - it just makes it look nice.
    system("cls");

    // Begin Game Loop
    gamedata.playing = 1; // This can be accessed by the game logic itself - that's what ends everything.
    while(gamedata.playing){

        currentRoom = getCurrentRoom(gamedata.worldRooms, gamedata.currentRoomId);

        printRoom((*currentRoom), gamedata.inventory);

        // This helps organize and deal with all player input.
        handlePlayerVerb(&gamedata);
    }
}



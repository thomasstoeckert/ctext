/* Thomas Stoeckert's CText Engine
    COP 3223H - Final Project
   This project consists of two parts, the editor and the player. The former creates files to be used by the latter.
   These two parts are intertwined with each other in these files, sharing verb handling, i/o, and data structures to maintain
    feature parity and compatibility. A change in one side equals the same change in the other.

   However, this can be quite confusing for people who aren't familiar with the project.
   To help with this, each function will be labeled with what it is used in as well as more information about it

   This main file simply splits the editor and the player. The user selects which one they use, then this
    file's job is done.

   I'd start looking at the Player, since it s far simpler to understand.
   */

#include "player.h"
#include "editor.h"

#include <stdio.h>
#include <ctype.h>

int main()
{
    char choice;
    int valid = 0;

    printf("Thomas Stoeckert's CText Adventure Engine\n");
    printf("This program consists of two parts, an editor to create files describing the game world and a player to play those files.\n");

    printf("What would you like to open? The (E)ditor or the (P)layer: ");
    scanf(" %c", &choice);

    while(!valid){
        // Clear the buffer, so extra characters don't gunk up future choices
        while((getchar()) != '\n');
        choice = tolower(choice);

        if(choice == 'e'){
            valid = 1;
            editorLoop();
        } else if (choice == 'p'){
            valid = 1;
            playerLoop();
        } else {
            // User has made an invalid choice. Inform them.
            printf("Unknown selection. Please try again.\n");
            scanf(" %c", &choice);
        }
    }

    printf("Thanks for using CText! Press enter to exit.\n");
    getchar(); // Just to keep the exe from closing on exit.

    return 0;
}

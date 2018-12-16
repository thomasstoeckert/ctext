#ifndef WIZARDS_H_DEFINE
#define WIZARDS_H_DEFINE

#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER

#include "data.h"

void smartIntReplace(int *toAlter, int original);
void smartStringCopy(char **dest, char *src);
void wizardItemPrint(struct item *targetItem);
void wizardActionTypePrint(void);

struct room createRoomWizard(int id);
void editRoomWizard(struct room *targetRoom);

struct door createDoorWizard();
void editDoorWizard(struct door *targetDoor);

struct item createItemWizard(int id);
void editItemWizard(struct item *targetItem);

struct combo createComboWizard();
void editComboWizard(struct combo *targetCombo);

#endif // WIZARDS_H_DEFIE

#ifndef DATA_H_DEFINE
#define DATA_H_DEFINE

#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER

#define DOOR_INVALID_FLAG -17

struct door{
    int dest;
    char *path;
};

struct ll_door{
    struct door data;
    struct ll_door *next;
};

struct doors{
    struct ll_door *paths;
    int numDoors;
};

struct action{
    int actionType;
    int itemTarget;
    int quantity;
    char *desc;
    int roomTarget;
    int doorTarget;
    int destTarget;
};

struct item{
    char *name;
    char *desc;
    struct action itemAction;
    int pickable;
    int id;
};

struct ll_item{
    struct ll_item *next;
    struct item data;
};

struct items{
    struct ll_item *itemData;
    int numItems;
    int nextID;
};

struct room{
    struct doors paths;
    struct items inventory;
    char *name;
    char *desc;
    int id;
};

struct ll_room{
    struct ll_room *next;
    struct room data;
};

struct rooms{
    struct ll_room *roomData;
    int numRooms;
    int nextID;
};

struct combo{
    int primaryID;
    int secondaryID;
    int resultID;
    char *desc;
};

struct ll_combo{
    struct combo data;
    struct ll_combo *next;
};

struct combos{
    struct ll_combo *data;
    int numCombos;
};

struct world{
    struct items inventory;
    struct items itemPrototypes;
    struct rooms worldRooms;
    struct combos recipes;
    char *introMessage;
    char mode;
    int currentRoomIndex;
    int currentRoomId;
    int playing;
};

struct ll_item * iAppend(struct ll_item *head, struct item data);
struct ll_item * iFindItemByIndex(struct ll_item *head, int index);
struct ll_item * iFindItemByID(struct ll_item *head, int id);
struct ll_item * iDeleteItemByIndex(struct ll_item *head, int index);
struct ll_item * iDeleteItemByID(struct ll_item *head, int id);
struct ll_item * iDeleteEnd(struct ll_item *head);
struct ll_item * iDeleteAll(struct ll_item *head);

struct ll_room * rAppend(struct ll_room *head, struct room data);
struct ll_room * rFindRoomByIndex(struct ll_room *head, int index);
struct ll_room * rFindRoomByID(struct ll_room *head, int id);
struct ll_room * rDeleteRoomByIndex(struct ll_room *head, int index);
struct ll_room * rDeleteRoomByID(struct ll_room *head, int id);
struct ll_room * rDeleteEnd(struct ll_room *head);
struct ll_room * rDeleteAll(struct ll_room *head);

struct ll_door * dAppend(struct ll_door *head, struct door data);
struct ll_door * dFindDoorByIndex(struct ll_door *head, int index);
struct ll_door * dDeleteDoorByIndex(struct ll_door *head, int index);
struct ll_door * dDeleteEnd(struct ll_door *head);
struct ll_door * dDeleteAll(struct ll_door *head);

struct ll_combo * cAppend(struct ll_combo *head, struct combo data);
struct ll_combo * cFindComboByIndex(struct ll_combo *head, int index);
struct ll_combo * cDeleteComboByIndex(struct ll_combo *head, int index);
struct ll_combo * cDeleteEnd(struct ll_combo *head);
struct ll_combo * cDeleteAll(struct ll_combo *head);

struct world initWorld(void);
struct room initRoom(int id);

struct room * getCurrentRoom(struct rooms worldRooms, int roomId);
struct combo * findCombo(struct combos recipes, int target1ID, int target2ID);

void printRoom(struct room currentRoom, struct items playerItems);
void printPaths(struct doors possibleDoors);
void printItems(struct items roomItems, struct items bagItems);

void printRoomVerbose(struct room currentRoom);
void printRoomsVerbose(struct rooms toPrint);
void printItemsVerbose(struct items roomItems);
void printPathsVerbose(struct doors toPrint);
void printCombosVerbose(struct combos toPrint);

#endif // DATA_H_DEFINE

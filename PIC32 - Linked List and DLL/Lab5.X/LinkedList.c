/*
 * File:   LinkedList.c
 * Author: Brian
 *
 * Created on May 7, 2017, 6:37 PM
 */

// **** Include libraries here ****
// Standard libraries
#include <stdio.h>
#include <string.h>

//CMPE13 Support Library
#include "LinkedList.h"
#include "BOARD.h"

/**
 * This function starts a new linked list. Given an allocated pointer to data it will return a
 * pointer for a malloc()ed ListItem struct. If malloc() fails for any reason, then this function
 * returns NULL otherwise it should return a pointer to this new list item. data can be NULL.
 *
 * @param data The data to be stored in the first ListItem in this new list. Can be any valid 
 *             pointer value.
 * @return A pointer to the malloc()'d ListItem. May be NULL if an error occured.
 */
ListItem *LinkedListNew(char *data)
{
    ListItem *nList = malloc(sizeof (ListItem));
    nList->data = data;
    if (nList == NULL) {
        return NULL;
    } else {
        return nList;
    }
}

/**
 * This function will remove a list item from the linked list and free() the memory that the
 * ListItem struct was using. It doesn't, however, free() the data pointer and instead returns it
 * so that the calling code can manage it.  If passed a pointer to NULL, LinkedListRemove() should
 * return NULL to signal an error.
 *
 * @param item The ListItem to remove from the list.
 * @return The data pointer from the removed item. May be NULL.
 */
char *LinkedListRemove(ListItem *item)
{
    if (item == NULL) {
        return NULL;
    } else {
        ListItem *remove = item;
        char *rData = remove->data;
        if (remove->previousItem == NULL && remove->nextItem == NULL) {
            free(remove);
        } else if (remove->nextItem == NULL) {
            remove->previousItem->nextItem = NULL;
            free(remove);
        } else if (remove->previousItem == NULL) {
            remove->nextItem->previousItem = NULL;
            free(remove);
        } else {
            remove->nextItem->previousItem = remove->previousItem;
            remove->previousItem->nextItem = remove->nextItem;
            free(remove);
        }
        return rData;
    }
}

/**
 * This function returns the total size of the linked list. This means that even if it is passed a
 * ListItem that is not at the head of the list, it should still return the total number of
 * ListItems in the list. A NULL argument will result in 0 being returned.
 *
 * @param list An item in the list to be sized.
 * @return The number of ListItems in the list (0 if `list` was NULL).
 */
int LinkedListSize(ListItem *list)
{
    if (list == NULL) {
        return NULL;
    } else {
        int temp = 0;
        ListItem *size = LinkedListGetFirst(list);
        while (size != NULL) {
            temp++;
            size = size->nextItem;
        }
        return temp;
    }
}

/**
 * This function returns the head of a list given some element in the list. If it is passed NULL,
 * it will just return NULL. If given the head of the list it will just return the pointer to the
 * head anyways for consistency.
 *
 * @param list An element in a list.
 * @return The first element in the list. Or NULL if provided an invalid list.
 */
ListItem *LinkedListGetFirst(ListItem *list)
{
    if (list == NULL) {
        return NULL;
    } else {
        ListItem *first;
        first = list;
        while (first->previousItem != NULL) {
            first = first->previousItem;
        }
        return first;
    }
}

/**
 * This function allocates a new ListItem containing data and inserts it into the list directly
 * after item. It rearranges the pointers of other elements in the list to make this happen. If
 * passed a NULL item, InsertAfter() should still create a new ListItem, just with no previousItem.
 * It returns NULL if it can't malloc() a new ListItem, otherwise it returns a pointer to the new
 * item. The data parameter is also allowed to be NULL.
 *
 * @param item The ListItem that will be before the newly-created ListItem.
 * @param data The data the new ListItem will point to.
 * @return A pointer to the newly-malloc()'d ListItem.
 */
ListItem *LinkedListCreateAfter(ListItem *item, char *data)
{
    ListItem *next = LinkedListNew(data);
    if (next == NULL) {
        return NULL;
    } else if (item->nextItem == NULL) {
        next->nextItem = NULL;
        next->previousItem = item;
        item->nextItem = next;
    } else {
        next->previousItem = item;
        next->nextItem = item->nextItem;
        item->nextItem->previousItem = next;
        item->nextItem = next;
    }
    return next;
}

/**
 * LinkedListSwapData() switches the data pointers of the two provided ListItems. This is most
 * useful when trying to reorder ListItems but when you want to preserve their location. It is used
 * within LinkedListSort() for swapping items, but probably isn't too useful otherwise. This
 * function should return STANDARD_ERROR if either arguments are NULL, otherwise it should return
 * SUCCESS. If one or both of the data pointers are NULL in the given ListItems, it still does
 * perform the swap and returns SUCCESS.
 *
 * @param firstItem One of the items whose data will be swapped.
 * @param secondItem Another item whose data will be swapped.
 * @return SUCCESS if the swap worked or STANDARD_ERROR if it failed.
 */
int LinkedListSwapData(ListItem *firstItem, ListItem *secondItem)
{
    if (firstItem == NULL || secondItem == NULL) {
        return STANDARD_ERROR;
    } else {
        char *temp = NULL;
        temp = firstItem->data;
        firstItem->data = secondItem->data;
        secondItem->data = temp;
        return SUCCESS;
    }
}

/**
 * LinkedListSort() performs a selection sort on list to sort the elements into decending order. It
 * makes no guarantees of the addresses of the list items after sorting, so any ListItem referenced
 * before a call to LinkedListSort() and after may contain different data as only the data pointers
 * for the ListItems in the list are swapped. This function sorts the strings in ascending order
 * first by size (with NULL data pointers counting as 0-length strings) and then alphabetically
 * ascending order. So the list [dog, cat, duck, goat, NULL] will be sorted to [NULL, cat, dog,
 * duck, goat]. LinkedListSort() returns SUCCESS if sorting was possible. If passed a NULL pointer
 * for either argument, it will do nothing and return STANDARD_ERROR.
 *
 * @param list Any element in the list to sort.
 * @return SUCCESS if successful or STANDARD_ERROR is passed NULL pointers.
 */
int LinkedListSort(ListItem *list)
{
    if (list == NULL) {
        return STANDARD_ERROR;
    } else {
        ListItem *i, *j;
        int tempI, tempJ;
        for (i = list; i->nextItem != NULL; i = i->nextItem) {
            for (j = i->nextItem; j != NULL; j = j->nextItem) {

                if (i->data == NULL) {
                    tempI = 0;
                } else {
                    tempI = strlen(i->data);
                }
                if (j->data == NULL) {
                    tempJ = 0;
                } else {
                    tempJ = strlen(j->data);
                }

                if (tempI > tempJ) {
                    LinkedListSwapData(i, j);
                } else if (tempI == tempJ) {
                    if (tempI == 0 && tempJ == 0) {
                        continue;
                    } else if (strcmp(i->data, j->data) > 0) {
                        LinkedListSwapData(i, j);
                    }
                }
            }
        }
        return SUCCESS;
    }
}

/**
 * LinkedListPrint() prints out the complete list to stdout. This function prints out the given
 * list, starting at the head if the provided pointer is not the head of the list, like "[STRING1,
 * STRING2, ... ]" If LinkedListPrint() is called with a NULL list it does nothing, returning
 * STANDARD_ERROR. If passed a valid pointer, prints the list and returns SUCCESS.
 *
 * @param list Any element in the list to print.
 * @return SUCCESS or STANDARD_ERROR if passed NULL pointers.
 */
int LinkedListPrint(ListItem * list)
{
    if (list == NULL) {
        return STANDARD_ERROR;
    } else {
        ListItem *print = LinkedListGetFirst(list);
        printf("[");
        if (print->data == NULL) {
            printf(" (NULL)");
        } else {
            printf(" %s", print->data);
        }
        print = print->nextItem;
        while (print != NULL) {
            if (print->data == NULL) {
                printf(", (NULL)");
            } else {
                printf(", %s", print->data);
            }
            print = print->nextItem;
        }
        printf(" ]\n");
        return SUCCESS;
    }
}

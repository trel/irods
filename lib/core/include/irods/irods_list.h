/* For copyright information please refer to files in the COPYRIGHT directory
 */
#ifndef LIST_H__
#define LIST_H__
#include "irods/region.h"

typedef struct listNode ListNode;
/// \brief Node in a singly linked list.
struct listNode {
    ListNode *next; ///< Next node in the list.
    void *value; ///< Value stored in the node.
};

/// \brief Singly linked list container.
typedef struct list {
    int size; ///< Number of elements in the list.
    ListNode *head; ///< First node in the list.
    ListNode *tail; ///< Last node in the list.
} List;

#ifdef __cplusplus
extern "C" {
#endif
List *newList( Region *r );
List *newListNoRegion();
ListNode *newListNode( void *value, Region *r );
ListNode *newListNodeNoRegion( void *value );

void listRemoveNoRegion2( List *l, void *v );
void listAppendNoRegion( List *list, void *value );
void listAppend( List *list, void *value, Region *r );

void listAppendToNode( List *list, ListNode *node, void *value, Region *r );
void listRemove( List *list, ListNode *node );
void listRemoveNoRegion( List *list, ListNode *node );
void deleteListNoRegion( List *list );
void clearListNoRegion( List *list );

#ifdef __cplusplus
}
#endif

#endif // LIST_H__

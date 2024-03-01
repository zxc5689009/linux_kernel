#include "queue.h"
#include <list.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// 0
/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new = malloc(sizeof(struct list_head));
    if (!new)
        return NULL;
    INIT_LIST_HEAD(new);
    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list) {
        list_del(&entry->list);
        q_release_element(entry);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;
    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;
    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }
    list_add_tail(&new->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head)
        return NULL;
    struct list_head *first = head->next;
    element_t *element = list_entry(first, element_t, list);
    if (sp && bufsize) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(first);
    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->prev == head)
        return NULL;
    struct list_head *last = head->prev;
    element_t *element = list_entry(last, element_t, list);
    list_del(last);
    if (sp && bufsize) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || head->next == head || head->next->next == head)
        return false;
    struct list_head *slow = head->next;
    for (struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next) {
        slow = slow->next;
    }

    element_t *middle_element = list_entry(slow, element_t, list);
    list_del(&middle_element->list);
    free(middle_element->value);
    free(middle_element);
    //  q_release_element(middle_element);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || head->next == head)
        return false;

    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *current = head->next;
    struct list_head *next = current->next;
    while (current != head && next != head) {
        current->prev->next = next;
        current->next = next->next;
        next->next->prev = current;
        next->next = current;
        next->prev = current->prev;
        current->prev = next;
        current = current->next;
        next = current->next;
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || head->next == head)
        return;
    struct list_head *current = head;
    struct list_head *next = current->next;
    do {
        current->next = current->prev;
        current->prev = next;
        current = next;
        next = next->next;
    } while (current != head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    /*    if (!head || head->next == head)
            return;
        struct list_head *current = head->next;
        struct list_head *next = current->next;
        while (current != head) {
            for (i = 0; i < k; i++) {*/
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

static struct list_head *mergeTwoLists(struct list_head *List1,
                                       struct list_head *List2,
                                       bool descend)
{
    struct list_head *head = NULL;
    struct list_head **ptr = &head;
    if (descend == true) {
        for (; List1 && List2; ptr = &(*ptr)->next) {
            element_t *l1 = list_entry(List1, element_t, list);
            element_t *l2 = list_entry(List2, element_t, list);
            if (strcmp(l1->value, l2->value) > 0) {
                *ptr = List1;
                List1 = List1->next;
            } else {
                *ptr = List2;
                List2 = List2->next;
            }
        }
    } else {
        for (; List1 && List2; ptr = &(*ptr)->next) {
            element_t *l1 = list_entry(List1, element_t, list);
            element_t *l2 = list_entry(List2, element_t, list);
            if (strcmp(l1->value, l2->value) < 0) {
                *ptr = List1;
                List1 = List1->next;
            } else {
                *ptr = List2;
                List2 = List2->next;
            }
        }
    }
    *ptr = (struct list_head *) ((uintptr_t) List1 | (uintptr_t) List2);
    return head;
}

struct list_head *mergesort_list(struct list_head *head, bool descend)
{
    if (!head || head->next == head)
        return head;
    struct list_head *fast = head, *slow = head;
    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    slow->prev->next = NULL;
    struct list_head *l1 = mergesort_list(head, descend);
    struct list_head *l2 = mergesort_list(slow, descend);
    return mergeTwoLists(l1, l2, descend);
}
/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || head->next == head)
        return;
    head->prev->next = NULL;
    head->next = mergesort_list(head->next, descend);
    head->next->prev = head;
    struct list_head *tmp = head;
    while (tmp->next) {
        tmp->next->prev = tmp;
        tmp = tmp->next;
    }
    head->prev = tmp;
    tmp->next = head;
}

/* Remove every node which has a node with a strictly less value
 * anywhere to the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value
 * anywhere to the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in
 * ascending/descending order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}

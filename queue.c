#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"


#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new_queue = malloc(sizeof(struct list_head));
    if (!new_queue) {
        return NULL;
    }
    INIT_LIST_HEAD(new_queue);
    return new_queue;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        list_del(&entry->list);
        q_release_element(entry);  // free entry
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *new_node = (element_t *) malloc(sizeof(element_t));
    if (!new_node) {
        return false;
    }
    new_node->value = strdup(s);
    if (!new_node->value) {
        free(new_node);
        return false;
    }
    list_add(&new_node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *new_node = malloc(sizeof(element_t));
    if (!new_node) {
        return false;
    }
    new_node->value = strdup(s);
    if (!new_node->value) {
        free(new_node);
        return false;
    }
    list_add_tail(&new_node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head) {

        return NULL;
    }
    // copy data
    element_t *first_node = list_first_entry(head, element_t, list);
    //這一行的作用是獲取鏈結串列中第一個element_t的指標，並將其賦值給 first_node

    if (sp != NULL && bufsize > 0) {
        strncpy(sp, first_node->value, bufsize - 1);  // string copy
        sp[bufsize - 1] = '\0';
    }

    list_del_init(head->next);

    return first_node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{

    if (!head || head->prev == head) {
        return NULL;
    }

    element_t *last_node = list_last_entry(head, element_t, list);

    if (sp != NULL && bufsize > 0) {
        strncpy(sp, last_node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del_init(head->prev);


    return last_node;
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

    if (!head || list_empty(head) || head->next->next == head) {
        // 空鏈結串列或僅有一個節點時，無法刪除中間節點
        return false;
    }

    struct list_head *slow = head->next;
    struct list_head *fast = head->next;

    // 使用快慢指標法定位中間節點
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 此時 slow 停在中間節點
    struct list_head *mid = slow;

    list_del(mid);

    // 若需要釋放節點所佔用的記憶體（依需求處理）
    element_t *mid_node = list_entry(mid, element_t, list);

    q_release_element(mid_node);

    return true;

    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head) || head->next == head->prev) {
        return false;
    }
    element_t *current, *next;
    bool dup = false;

    list_for_each_entry_safe (current, next, head, list) {
        if (&next->list != head && strcmp(current->value, next->value) == 0) {
            list_del(&current->list);
            q_release_element(current);
            dup = true;
        } else if (dup) {
            list_del(&current->list);
            q_release_element(current);
            dup = false;
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    q_reverseK(head, 2);
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *current, *next;
    list_for_each_safe (current, next, head) {
        struct list_head *temp = current->prev;
        current->prev = current->next;
        current->next = temp;
    }

    struct list_head *temp = head->prev;
    head->prev = head->next;
    head->next = temp;

}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    struct list_head *current, *safe, *cut;
    int count = k;
    cut = head;
    list_for_each_safe (current, safe, head) {
        if (--count != 0) {
            continue;
        }
        LIST_HEAD(temp);
        count = k;
        list_cut_position(&temp, cut, current);
        q_reverse(&temp);
        list_splice(&temp, cut);
        cut = safe->prev;
    }
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

void q_merge_two(struct list_head *list1, struct list_head *list2, bool descend)
{
    if (list_empty(list1) || list_empty(list2)) {
        return;
    }
    struct list_head temp;
    INIT_LIST_HEAD(&temp);
    element_t *node;
    if (descend) {
        while (!list_empty(list1) && !list_empty(list2)) {
            element_t *node1 = list_first_entry(list1, element_t, list);
            element_t *node2 = list_first_entry(list2, element_t, list);
            node = (strcmp(node1->value, node2->value) > 0) ? node1 : node2;
            list_move_tail(&node->list, &temp);
        }

    } else {
        while (!list_empty(list1) && !list_empty(list2)) {
            element_t *node1 = list_first_entry(list1, element_t, list);
            element_t *node2 = list_first_entry(list2, element_t, list);
            node = (strcmp(node1->value, node2->value) < 0) ? node1 : node2;
            list_move_tail(&node->list, &temp);

        }
    }
    struct list_head *residual = (list_empty(list1)) ? list2 : list1;
    list_splice_tail_init(residual, &temp);
    list_splice(&temp, list1);
}





/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || (head->next == head) || (head->next == head->prev)) {
        return;
    }

    struct list_head left;
    INIT_LIST_HEAD(&left);
    struct list_head *mid = head;
    // find middle point
    for (struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next) {
        mid = mid->next;
    }

    list_cut_position(&left, head, mid);
    // head = right side
    q_sort(&left, descend);
    q_sort(head, descend);
    q_merge_two(head, &left, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || head->next == head) {
        return 0;
    }

    element_t *current = list_entry(head->prev, element_t, list);

    while (current->list.prev != head) {
        element_t *prev = list_entry(current->list.prev, element_t, list);
        if (strcmp(current->value, prev->value) <= 0) {
            list_del(&prev->list);
            q_release_element(prev);
        } else {
            current = prev;
        }
    }
    return q_size(head);

    // https://leetcode.com/problems/remove-nodes-from-linked-list/
}


/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || head->next == head) {
        return 0;
    }

    element_t *current = list_entry(head->prev, element_t, list);

    while (current->list.prev != head) {
        element_t *prev = list_entry(current->list.prev, element_t, list);
        if (strcmp(current->value, prev->value) >= 0) {
            list_del(&prev->list);
            q_release_element(prev);
        } else {
            current = prev;
        }
    }
    return q_size(head);


    // https://leetcode.com/problems/remove-nodes-from-linked-list/

}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    //第二個到最後一個隊列中的元素合併到第一個隊列中
    if (!head || list_empty(head)) {
        return 0;
    }
    if (head->next == head->prev) {
        return q_size(list_first_entry(head, queue_contex_t, chain)->q);
    }

    queue_contex_t *first = list_first_entry(head, queue_contex_t, chain);
    queue_contex_t *current, *safe;

    list_for_each_entry_safe (current, safe, head, chain) {
        if (first != current) {
            list_splice_tail_init(current->q, first->q);
            // current -> q = NULL;
        }
    }
    q_sort(first->q, descend);
    return q_size(first->q);
    // https://leetcode.com/problems/merge-k-sorted-lists/

}


// static struct list_head *merge(void *priv,
//                                list_cmp_func_t cmp,
//                                struct list_head *a,
//                                struct list_head *b)
// {
//     struct list_head *head = NULL, **tail = &head;

//     for (;;) {
//         /* if equal, take 'a' -- important for sort stability */
//         if (cmp(priv, a, b) <= 0) {
//             *tail = a;
//             tail = &a->next;
//             a = a->next;
//             if (!a) {
//                 *tail = b;
//                 break;
//             }
//         } else {
//             *tail = b;
//             tail = &b->next;
//             b = b->next;
//             if (!b) {
//                 *tail = a;
//                 break;
//             }
//         }
//     }
//     return head;
// }

// static void merge_final(void *priv,
//                         list_cmp_func_t cmp,
//                         struct list_head *head,
//                         struct list_head *a,
//                         struct list_head *b)
// {
//     struct list_head *tail = head;
//     unsigned int count = 0;

//     for (;;) {
//         /* if equal, take 'a' -- important for sort stability */
//         if (cmp(priv, a, b) <= 0) {
//             tail->next = a;
//             a->prev = tail;
//             tail = a;
//             a = a->next;
//             if (!a)
//                 break;
//         } else {
//             tail->next = b;
//             b->prev = tail;
//             tail = b;
//             b = b->next;
//             if (!b) {
//                 b = a;
//                 break;
//             }
//         }
//     }

//     /* Finish linking remainder of list b on to tail */
//     tail->next = b;
//     do {
//         if (unlikely(!++count))
//             cmp(priv, b, b);
//         b->prev = tail;
//         tail = b;
//         b = b->next;
//     } while (b);

//     /* And the final links to make a circular doubly-linked list */
//     tail->next = head;
//     head->prev = tail;
// }

// void list_sort(void *priv,
//                struct list_head *head,
//                bool descend,
//                list_cmp_func_t cmp)
// {
//     struct list_head *list = head->next, *pending = NULL;
//     size_t count = 0; /* Count of pending */

//     if (head == head->prev) /* Zero or one elements */
//         return;
//     /* Convert to a null-terminated singly-linked list. */
//     head->prev->next = NULL;

//     do {
//         size_t bits;
//         struct list_head **tail = &pending;

//         /* Find the least-significant clear bit in count */
//         for (bits = count; bits & 1; bits >>= 1)
//             tail = &(*tail)->prev;
//         /* Do the indicated merge */
//         if (likely(bits)) {
//             struct list_head *a = *tail, *b = a->prev;

//             a = merge(priv, cmp, b, a);
//             /* Install the merged result in place of the inputs */
//             a->prev = b->prev;
//             *tail = a;
//         }

//         /* Move one element from input list to pending */
//         list->prev = pending;
//         pending = list;
//         list = list->next;
//         pending->next = NULL;
//         count++;
//     } while (list);

//     /* End of input; merge together all the pending lists. */
//     list = pending;
//     pending = pending->prev;
//     for (;;) {
//         struct list_head *next = pending->prev;

//         if (!next)
//             break;
//         list = merge(priv, cmp, pending, list);
//         pending = next;
//     }
//     /* The final merge, rebuilding prev links */
//     merge_final(priv, cmp, head, pending, list);
//     if (descend)
//         q_reverse(head);
// }
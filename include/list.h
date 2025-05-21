
#ifndef LIST_H
#define LIST_H
#include <stdbool.h>
#include <stddef.h>
#include <pthread.h>
#ifdef __cplusplus
extern "C" {
#endif

#define container_of(ptr, type, member) ({                      \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);        \
    (type *)( (char *)__mptr - offsetof(type,member) );         \
})

/* sl_list */

typedef struct sl_node {
    struct sl_node *next;
} sl_node_t;

typedef struct sl_head {
    sl_node_t *first;
} sl_head_t;


#define SL_ENTRY(ptr, type, member) (container_of(ptr, type, member))

static inline void sl_init(sl_head_t *list);
static inline void sl_init_node(sl_node_t *node);
static inline bool sl_is_empty(const sl_head_t *list);
static inline sl_node_t *sl_first(const sl_head_t *list);
static inline sl_node_t *sl_next(const sl_node_t *node);
static inline void sl_add_head(sl_head_t *list, sl_node_t *node);
static inline void sl_add_after(sl_head_t *list, sl_node_t *prev, sl_node_t *insert);
static inline sl_node_t *sl_del_after(sl_head_t *list, sl_node_t *prev);
static inline sl_node_t *sl_del_head(sl_head_t *list);
static inline void sl_del(sl_head_t *list, const sl_node_t *node);
static inline void sl_append(sl_head_t *dest_list, sl_head_t *src_list);
static inline void sl_free_all(sl_head_t *list, void (*free_node)(void *));

static inline void sl_init(sl_head_t *list)
{
    list->first = (sl_node_t *)NULL;
    return;
}

static inline void sl_init_node(sl_node_t *node)
{
    node->next = (sl_node_t *)NULL;
    return;
}

static inline bool sl_is_empty(const sl_head_t *list)
{
    return (list->first == NULL);
}

static inline sl_node_t *sl_first(const sl_head_t *list)
{
    return (list->first);
}

static inline sl_node_t *sl_next(const sl_node_t *node)
{
    return (node->next);
}

static inline void sl_add_head(sl_head_t *list, sl_node_t *node)
{
    node->next = list->first;
    list->first = node;
    return;
}

static inline sl_node_t *sl_del_head(sl_head_t *list)
{
    sl_node_t *node = list->first;
    if (NULL != node) {
        list->first = node->next;
    }
    return node;
}

static inline void sl_add_after(sl_head_t *list, sl_node_t *prev, sl_node_t *insert)
{
    if (NULL == prev) {
        sl_add_head(list, insert);
    } else {
        insert->next = prev->next;
        prev->next = insert;
    }
    return;
}

static inline sl_node_t *sl_del_after(sl_head_t *list, sl_node_t *prev)
{
    sl_node_t *node;
    if (NULL == prev) {
        node = sl_del_head(list);
    } else {
        node = prev->next;
        if (NULL != node) {
            prev->next = node->next;
        }
    }
    return node;
}

#define SL_FOREACH(list, node) \
    for ((node) = sl_first(list); \
         NULL != (node); \
         (node) = sl_next(node))

#define SL_FOREACH_SAFE(list, node, next) \
    for ((node) = sl_first((list)); \
         (NULL != (node)) && ({(next) = sl_next(node); true;}); \
         (node) = (next))
#define SL_FOREACH_PREVPTR(list, node, prev) \
    for ((node) = sl_first(list), (prev) = (sl_node_t *)NULL; \
         NULL != (node); \
         (void)({(prev) = (node); (node) = sl_next(node);}))
#define SL_ENTRY_FIRST(list, type, member) \
    (sl_is_empty(list)? NULL : SL_ENTRY(sl_first(list), type, member))
#define SL_ENTRY_NEXT(entry, member) \
    (NULL == (entry)? NULL : \
     (NULL == sl_next(&(entry)->member)? NULL : \
      SL_ENTRY(sl_next(&(entry)->member), typeof(*(entry)), member)))
#define SL_FOREACH_ENTRY(list, entry, member) \
    for ((entry) = SL_ENTRY_FIRST(list, typeof(*(entry)), member); \
         NULL != (entry); \
         (entry) = SL_ENTRY_NEXT(entry, member))
#define SL_FOREACH_ENTRY_SAFE(list, entry, next_entry, member) \
    for ((entry) = SL_ENTRY_FIRST(list, typeof(*(entry)), member); \
         (NULL != (entry)) && \
         ({(next_entry) = SL_ENTRY_NEXT(entry, member); true;}); \
         (entry) = (next_entry))
#define SL_FOREACH_ENTRY_PREVPTR(list, entry, prev_entry, member) \
    for ((entry) = SL_ENTRY_FIRST(list, typeof(*(entry)), member), \
         (prev_entry) = NULL; \
         NULL != (entry); \
         (void)({(prev_entry) = (entry); \
                  (entry) = SL_ENTRY_NEXT(entry, member);}))

static inline void sl_del(sl_head_t *list, const sl_node_t *node)
{
    sl_node_t *pstCur, *prev;
    SL_FOREACH_PREVPTR(list, pstCur, prev) {
        if (pstCur == node) {
            (void)sl_del_after(list, prev);
            break;
        }
    }
    return;
}

static inline void sl_append(sl_head_t *dest_list, sl_head_t *src_list)
{
    sl_node_t *node, *prev;
    if (true != sl_is_empty(src_list)) {
        SL_FOREACH_PREVPTR(dest_list, node, prev) {
            ;
        }
        if (NULL == prev) {
            dest_list->first = sl_first(src_list);
        } else {
            prev->next = sl_first(src_list);
        }
        sl_init(src_list);
    }
    return;
}
static inline void sl_free_all(sl_head_t *list, void (*free_node)(void *))
{
    sl_node_t *pstCurNode = NULL;
    sl_node_t *nextNode = NULL;
    SL_FOREACH_SAFE(list, pstCurNode, nextNode) {
        free_node(pstCurNode);
    }
    sl_init(list);
    return;
}

/* stq_list */
typedef struct stq_node {
    struct stq_node *next;
} stq_node_t;

typedef struct stq_head {
    stq_node_t *first;
    stq_node_t *last;
} stq_head_t;

#define STQ_ENTRY(ptr, type, member) (container_of(ptr, type, member))

static inline void stq_init(stq_head_t *list);
static inline void stq_init_node(stq_node_t *node);
static inline bool stq_is_empty(const stq_head_t *list);
static inline stq_node_t *stq_first(const stq_head_t *list);
static inline stq_node_t *stq_last(const stq_head_t *list);
static inline stq_node_t *stq_next(const stq_node_t *node);
static inline void stq_add_head(stq_head_t *list, stq_node_t *node);
static inline stq_node_t *stq_del_head(stq_head_t *list);
static inline void stq_add_tail(stq_head_t *list, stq_node_t *node);
static inline void stq_add_after(stq_head_t *list, stq_node_t *prev, stq_node_t *insert);
static inline stq_node_t *stq_del_after(stq_head_t *list, stq_node_t *prev);
static inline void stq_del(stq_head_t *list, const stq_node_t *node);
static inline stq_node_t *stq_del_after(stq_head_t *list, stq_node_t *prev);

static inline void stq_append(stq_head_t *dest_list, stq_head_t *src_list);
static inline void stq_free_all(stq_head_t *list, void (*free_node)(void *));

static inline void stq_init(stq_head_t *list)
{
    list->first = (stq_node_t *)NULL;
    list->last = (stq_node_t *)NULL;
    return;
}
static inline void stq_init_node(stq_node_t *node)
{
    node->next = (stq_node_t *)NULL;
    return;
}
static inline bool stq_is_empty(const stq_head_t *list)
{
    return (list->first == NULL);
}
static inline stq_node_t *stq_first(const stq_head_t *list)
{
    return list->first;
}
static inline stq_node_t *stq_last(const stq_head_t *list)
{
    return list->last;
}
static inline stq_node_t *stq_next(const stq_node_t *node)
{
    return node->next;
};

static inline void stq_add_head(stq_head_t *list, stq_node_t *node)
{
    node->next = list->first;
    list->first = node;
    if (NULL == list->last) {
        list->last = node;
    }
    return;
}
static inline stq_node_t *stq_del_head(stq_head_t *list)
{
    stq_node_t *node = list->first;
    if (NULL != node) {
        list->first = node->next;
    }
    if (NULL == list->first) {
        list->last = (stq_node_t *)NULL;
    }
    return node;
}
static inline void stq_add_tail(stq_head_t *list, stq_node_t *node)
{
    node->next = (stq_node_t *)NULL;
    if (NULL != list->last) {
        list->last->next = node;
        list->last = node;
    } else {
        list->last = node;
        list->first = node;
    }
    return;
}
static inline void stq_add_after(stq_head_t *list,
                                 stq_node_t *prev,
                                 stq_node_t *insert)
{
    if (NULL == prev) {
        stq_add_head(list, insert);
    } else {
        insert->next = prev->next;
        prev->next = insert;
        if (list->last == prev) {
            list->last = insert;
        }
    }
    return;
}
static inline stq_node_t *stq_del_after(stq_head_t *list,
                                        stq_node_t *prev)
{
    stq_node_t *node;
    if (NULL == prev) {
        node = stq_del_head(list);
    } else {
        node = prev->next;
        if (NULL != node) {
            prev->next = node->next;
        }
        if (list->last == node) {
            list->last = prev;
        }
    }
    return node;
}

#define STQ_FOREACH(list, node) \
    for ((node) = stq_first(list); \
         NULL != (node); \
         (node) = stq_next(node))
#define STQ_FOREACH_SAFE(list, node, next) \
    for ((node) = stq_first((list)); \
         (NULL != (node)) && ({(next) = stq_next(node); true;}); \
         (node) = (next))
#define STQ_FOREACH_PREVPTR(list, node, prev) \
    for ((node) = stq_first(list), (prev) = (stq_node_t *)NULL; \
         NULL != (node); \
         (void)({(prev) = (node); (node) = stq_next(node);}))
#define STQ_ENTRY_FIRST(list, type, member) \
    (stq_is_empty(list)? NULL : STQ_ENTRY(stq_first(list), type, member))
#define STQ_ENTRY_LAST(list, type, member) \
    (stq_is_empty(list)? NULL : STQ_ENTRY(stq_last(list), type, member))
#define STQ_ENTRY_NEXT(entry, member) \
    (NULL == (entry)? NULL : \
     (NULL == stq_next(&(entry)->member)? NULL : \
      STQ_ENTRY(stq_next(&(entry)->member), typeof(*(entry)), member)))
#define STQ_FOREACH_ENTRY(list, entry, member) \
    for ((entry) = STQ_ENTRY_FIRST(list, typeof(*(entry)), member); \
         NULL != (entry); \
         (entry) = STQ_ENTRY_NEXT(entry, member))
#define STQ_FOREACH_ENTRY_SAFE(list, entry, next_entry, member) \
    for ((entry) = STQ_ENTRY_FIRST(list, typeof(*(entry)), member); \
         (NULL != (entry)) && \
         ({(next_entry) = STQ_ENTRY_NEXT(entry, member); true;}); \
         (entry) = (next_entry))
#define STQ_FOREACH_ENTRY_PREVPTR(list, entry, prev_entry, member) \
    for ((entry) = STQ_ENTRY_FIRST(list, typeof(*(entry)), member), \
         (prev_entry) = NULL; \
         NULL != (entry); \
         (void)({(prev_entry) = (entry); \
                  (entry) = STQ_ENTRY_NEXT(entry, member);}))

                  
static inline void stq_del(stq_head_t *list, const stq_node_t *node)
{
    stq_node_t *cur, *prev;
    STQ_FOREACH_PREVPTR(list, cur, prev) {
        if (cur == node) {
            (void)stq_del_after(list, prev);
            break;
        }
    }
    return;
}
static inline void stq_append(stq_head_t *dest_list, stq_head_t *src_list)
{
    stq_node_t *node, *prev;
    if (true != stq_is_empty(src_list)) {
        STQ_FOREACH_PREVPTR(dest_list, node, prev) {
            ;
        }
        if (NULL == prev) {
            dest_list->first = stq_first(src_list);
            dest_list->last = stq_last(src_list);
        } else {
            prev->next = stq_first(src_list);
            dest_list->last = stq_last(src_list);
        }
        stq_init(src_list);
    }
    return;
}
static inline void stq_free_all(stq_head_t *list, void (*free_node)(void *))
{
    stq_node_t *pstCurNode = NULL;
    stq_node_t *nextNode = NULL;
    STQ_FOREACH_SAFE(list, pstCurNode, nextNode) {
        free_node(pstCurNode);
    }
    stq_init(list);
    return;
}


/* dl_list */

typedef struct dl_node {
    struct dl_node *next;
    struct dl_node **prev;
} dl_node_t;

typedef struct dl_head {
    dl_node_t *first;
} dl_head_t;

#define DL_ENTRY(ptr, type, member) container_of(ptr, type, member)
#define DL_NODE_FROM_PPRE(prev) container_of((prev), struct dl_node, next)
#define DL_ENTRY_FROM_PPRE(prev, type, member) DL_ENTRY(DL_NODE_FROM_PPRE(prev), type, member)

static inline void dl_init(dl_head_t *list);
static inline void dl_init_node(dl_node_t *node);
static inline bool dl_is_empty(const dl_head_t *list);
static inline dl_node_t *dl_first(const dl_head_t *list);
static inline dl_node_t *dl_next(const dl_node_t *node);
static inline dl_node_t *dl_prev(const dl_node_t *node);
static inline void dl_add_head(dl_head_t *list, dl_node_t *node);
static inline void dl_add_after(dl_node_t *prev, dl_node_t *insert);
static inline void dl_add_after_prevptr(dl_node_t **prev, dl_node_t *insert);
static inline void dl_add_before(dl_node_t *next, dl_node_t *insert);
static inline void dl_append(dl_head_t *dest_list, dl_head_t *src_list);
static inline void dl_del(dl_node_t *node);
static inline void dl_del_and_init_node(dl_node_t *node);
static inline dl_node_t *dl_del_head_and_init_node(const dl_head_t *list);
static inline dl_node_t *dl_del_head(const dl_head_t *list);
static inline void dl_free_all(dl_head_t *list, void (*free_node)(void *));

static inline void dl_init(dl_head_t *list)
{
    list->first = (dl_node_t *)NULL;
    return;
}
static inline void dl_init_node(dl_node_t *node)
{
    node->next = (dl_node_t *)NULL;
    node->prev = (dl_node_t *)NULL;
    return;
}
static inline bool dl_is_empty(const dl_head_t *list)
{
    return (list->first == NULL);
}
static inline dl_node_t *dl_first(const dl_head_t *list)
{
    return (list->first);
}
static inline dl_node_t *dl_next(const dl_node_t *node)
{
    return node->next;
}
static inline dl_node_t *dl_prev(const dl_node_t *node)
{
    return DL_NODE_FROM_PPRE(node->prev);
}
static inline void dl_del(dl_node_t *node)
{
    if (NULL != node->prev) {
        *node->prev = node->next;
    }
    if (NULL != node->next) {
        node->next->prev = node->prev;
    }
    return;
}
static inline void dl_del_and_init_node(dl_node_t *node)
{
    dl_del(node);
    dl_init_node(node);
}
static inline dl_node_t *dl_del_head_and_init_node(const dl_head_t *list)
{
    dl_node_t *node = dl_first(list);
    if (dl_next(node) == NULL) {
        node = (dl_node_t *)NULL;
    } else {
        dl_del_and_init_node(node);
    }
    return node;
}
static inline void dl_add_head(dl_head_t *list, dl_node_t *node)
{
    node->prev = &list->first;
    node->next = list->first;
    if (NULL != node->next) {
        node->next->prev = &node->next;
    }
    list->first = node;
    return;
}
static inline dl_node_t *dl_del_head(const dl_head_t *list)
{
    dl_node_t *node = dl_first(list);
    if (NULL != node) {
        dl_del(node);
    }
    return node;
}
static inline void dl_add_after(dl_node_t *prev, dl_node_t *insert)
{
    insert->prev = &prev->next;
    insert->next = prev->next;
    prev->next = insert;
    if (NULL != insert->next) {
        insert->next->prev = &insert->next;
    }
    return;
}
static inline void dl_add_after_prevptr(dl_node_t **prev, dl_node_t *insert)
{
    insert->prev = *prev;
    insert->next = *prev;
    *prev = insert;
    if (NULL != insert->next) {
        insert->next->prev = &insert->next;
    }
    return;
}
static inline void dl_add_before(dl_node_t *next, dl_node_t *insert)
{
    insert->prev = next->prev;
    insert->next = next;
    if (NULL != insert->prev) {
        *insert->prev = insert;
    }
    insert->next->prev = &insert->next;
    return;
}
#define DL_FOREACH(list, node) \
    for ((node) = dl_first((list)); \
         NULL != (node); \
         (node) = dl_next(node))
#define DL_FOREACH_SAFE(list, node, next) \
    for ((node) = dl_first((list)); \
         (NULL != (node)) && ({(next) = dl_next(node); true;}); \
         (node) = (next))
#define DL_FOREACH_PREVPTR(list, node, prev) \
    for ((node) = dl_first((list)), (prev) = &((list)->first); \
         NULL != (node); \
         (void)({(prev) = &((node)->next); \
                  (node) = dl_next(node);}))
#define DL_ENTRY_FIRST(list, type, member) \
    (dl_is_empty(list)? NULL : DL_ENTRY(dl_first(list), type, member))
#define DL_ENTRY_NEXT(entry, member) \
    (NULL == (entry)? NULL : \
     (NULL == dl_next(&((entry)->member))? NULL : \
      DL_ENTRY(dl_next(&((entry)->member)), typeof(*(entry)), member)))
#define DL_ENTRY_PREV(entry, member) \
    (NULL == (entry)? NULL : \
     (NULL == dl_prev(&((entry)->member))? NULL : \
      DL_ENTRY(dl_prev(&((entry)->member)), typeof(*(entry)), member)))
#define DL_FOREACH_ENTRY(list, entry, member) \
    for ((entry) = DL_ENTRY_FIRST(list, typeof(*(entry)), member); \
         NULL != (entry); \
         (entry) = DL_ENTRY_NEXT(entry, member))
#define DL_FOREACH_ENTRY_SAFE(list, entry, next_entry, member) \
    for ((entry) = DL_ENTRY_FIRST(list, typeof(*(entry)), member); \
         (NULL != (entry)) && \
         ({(next_entry) = DL_ENTRY_NEXT(entry, member); true;}); \
         (entry) = (next_entry))
#define DL_FOREACH_ENTRY_PREVPTR(list, entry, prev, member) \
    for ((entry) = DL_ENTRY_FIRST(list, typeof(*(entry)), member), \
         (prev) = &((list)->first); \
         NULL != (entry); \
         (void)({(prev) = &((entry)->member.next); \
                  (entry) = DL_ENTRY_NEXT(entry, member);}))

static inline void dl_append(dl_head_t *dest_list, dl_head_t *src_list)
{
    dl_node_t *node, **prev;
    if (true != dl_is_empty(src_list)) {
        DL_FOREACH_PREVPTR(dest_list, node, prev) {
            ;
        }
        *prev = src_list->first;
        src_list->first->prev = prev;
        dl_init(src_list);
    }
    return;
}
static inline void dl_free_all(dl_head_t *list, void (*free_node)(void *))
{
    dl_node_t *pstCurNode = NULL;
    dl_node_t *nextNode = NULL;
    DL_FOREACH_SAFE(list, pstCurNode, nextNode) {
        free_node(pstCurNode);
    }
    dl_init(list);
    return;
}

/* dtq list */

typedef struct dtq_node {
    struct dtq_node *prev;
    struct dtq_node *next;
} dtq_node_t;
#define DTQ_ENTRY(ptr, type, member) (container_of(ptr, type, member))
typedef struct dtq_head {
    dtq_node_t head;
} dtq_head_t;

static inline void dtq_init(dtq_head_t *list);
static inline void dtq_init_node(dtq_node_t *node);
static inline bool dtq_is_empty(const dtq_head_t *list);
static inline dtq_node_t *dtq_first(const dtq_head_t *list);
static inline dtq_node_t *dtq_last(const dtq_head_t *list);
static inline bool dtq_is_end(const dtq_head_t *list, const dtq_node_t *node);
static inline dtq_node_t *dtq_prev(const dtq_node_t *node);
static inline dtq_node_t *dtq_next(const dtq_node_t *node);
static inline void dtq_add_after(dtq_node_t *prev, dtq_node_t *insert);
static inline void dtq_add_before(dtq_node_t *next, dtq_node_t *insert);
static inline void dtq_del(const dtq_node_t *node);
static inline void dtq_add_head(dtq_head_t *list, dtq_node_t *node);
static inline dtq_node_t *dta_del_head(const dtq_head_t *list);
static inline void dtq_add_tail(dtq_head_t *list, dtq_node_t *node);
static inline dtq_node_t *dtq_del_tail(const dtq_head_t *list);
static inline void dtq_append(dtq_head_t *dest_list, dtq_head_t *src_list);
static inline void dtq_free_all(dtq_head_t *list, void (*free_node)(void *));

static inline void dtq_add_head(dtq_head_t *list, dtq_node_t *node)
{
    dtq_add_after(&list->head, node);
    return;
}
static inline dtq_node_t *dta_del_head(const dtq_head_t *list)
{
    dtq_node_t *node = dtq_first(list);
    if (dtq_is_end(list, node)) {
        node = (dtq_node_t *)NULL;
    } else {
        dtq_del(node);
    }
    return node;
}
static inline void dtq_add_tail(dtq_head_t *list, dtq_node_t *node)
{
    dtq_add_before(&list->head, node);
    return;
}
static inline dtq_node_t *dtq_del_tail(const dtq_head_t *list)
{
    dtq_node_t *node = dtq_last(list);
    if (dtq_is_end(list, node)) {
        node = (dtq_node_t *)NULL;
    } else {
        dtq_del(node);
    }
    return node;
}
static inline bool dtq_node_is_linked(dtq_node_t *node)
{
    return (NULL != node->next);
}
static inline void dtq_del_and_init_node(dtq_node_t *node)
{
    if (dtq_node_is_linked(node)) {
        dtq_del(node);
    }
    dtq_init_node(node);
}
static inline dtq_node_t *dtq_del_head_and_init_node(const dtq_head_t *list)
{
    dtq_node_t *node = dtq_first(list);
    if (dtq_is_end(list, node)) {
        node = (dtq_node_t *)NULL;
    } else {
        dtq_del_and_init_node(node);
    }
    return node;
}
static inline void dtq_move_and_add_tail(dtq_head_t *head, dtq_node_t *node)
{
    if (dtq_node_is_linked(node)) {
        dtq_del(node);
    }
    dtq_add_tail(head, node);
}
#define DTQ_FOREACH(list, node) \
    for ((node) = (list)->head.next; \
         ((node) != &((list)->head)); \
         (node) = dtq_next(node))
#define DTQ_FOREACH_SAFE(list, node, nextNode) \
    for ((node) = (list)->head.next; \
         ((node) != &((list)->head)) && \
         ({(nextNode) = dtq_next(node); true;}); \
         (node) = (nextNode))
#define DTQ_FOREACH_REVERSE(list, node) \
    for ((node) = dtq_last(list); \
         (true != dtq_is_end(list, node)); \
         (node) = dtq_prev(node))
#define DTQ_FOREACH_REVERSE_SAFE(list, node, prev) \
    for ((node) = dtq_last(list); \
         (true != dtq_is_end(list, node)) && \
         ({(prev) = dtq_prev(node); true;}); \
         (node) = (prev))
#define DTQ_ENTRY_FIRST(list, type, member) \
    ({dtq_node_t *node__Tmp__Mx = dtq_first(list); \
      NULL == node__Tmp__Mx? NULL : DTQ_ENTRY(node__Tmp__Mx, type, member)})
#define DTQ_ENTRY_LAST(list, type, member) \
    ({dtq_node_t *node__Tmp__Mx = dtq_last(list); \
      NULL == node__Tmp__Mx? NULL : DTQ_ENTRY(node__Tmp__Mx, type, member)})
#define DTQ_ENTRY_NEXT(list, entry, member) \
    (dtq_is_end(list, NULL == (entry)? NULL : dtq_next(&((entry)->member))) \
       ? NULL : \
        DTQ_ENTRY(dtq_next(&((entry)->member)), typeof(*(entry)), member))
#define DTQ_ENTRY_PREV(list, entry, member) \
    (dtq_is_end(list, NULL == (entry)? NULL : dtq_prev(&((entry)->member))) \
       ? NULL : \
        DTQ_ENTRY(dtq_prev(&((entry)->member)), typeof(*(entry)), member))
#define DTQ_FOREACH_ENTRY(list, entry, member) \
    for ((entry) = DTQ_ENTRY_FIRST(list, typeof(*(entry)), member); \
         ((&(entry)->member != &(list)->head) || ((entry) = NULL, false)); \
         (entry) = DTQ_ENTRY_NEXT(list, entry, member))
#define DTQ_FOREACH_ENTRY_SAFE(list, entry, next_entry, member) \
    for ((entry) = DTQ_ENTRY_FIRST(list, typeof(*(entry)), member); \
         ((&(entry)->member != &(list)->head) && \
          ({(next_entry) = DTQ_ENTRY_NEXT(list, entry, member); true;})); \
         (entry) = (next_entry))

static inline void dtq_init(dtq_head_t *list)
{
    list->head.next = &list->head;
    list->head.prev = &list->head;
    return;
}
static inline void dtq_init_node(dtq_node_t *node)
{
    node->next = (dtq_node_t *)NULL;
    node->prev = (dtq_node_t *)NULL;
    return;
}
static inline bool dtq_is_empty(const dtq_head_t *list)
{
    return (list->head.next == &list->head);
}
static inline dtq_node_t *dtq_first(const dtq_head_t *list)
{
    dtq_node_t *node = list->head.next;
    if (node == &list->head) {
        return NULL;
    }
    return node;
}
static inline dtq_node_t *dtq_last(const dtq_head_t *list)
{
    dtq_node_t *node = list->head.prev;
    if (node == &list->head) {
        return NULL;
    }
    return node;
}
static inline bool dtq_is_end(const dtq_head_t *list, const dtq_node_t *node)
{
    if (dtq_is_empty(list)) {
        return true;
    }
    if (NULL == node) {
        return true;
    }
    return (node == &list->head);
}
static inline dtq_node_t *dtq_prev(const dtq_node_t *node)
{
    return (node->prev);
}
static inline dtq_node_t *dtq_next(const dtq_node_t *node)
{
    return (node->next);
}
static inline void dtq_add_after(dtq_node_t *prev, dtq_node_t *insert)
{
    insert->prev = prev;
    insert->next = prev->next;
    prev->next = insert;
    insert->next->prev = insert;
    return;
}
static inline void dtq_add_before(dtq_node_t *next, dtq_node_t *insert)
{
    insert->prev = next->prev;
    insert->next = next;
    insert->prev->next = insert;
    insert->next->prev = insert;
    return;
}
static inline void dtq_del(const dtq_node_t *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    return;
}
static inline void dtq_append(dtq_head_t *dest_list, dtq_head_t *src_list)
{
    if (true != dtq_is_empty(src_list)) {
        src_list->head.next->prev = dest_list->head.prev;
        src_list->head.prev->next = dest_list->head.prev->next;
        dest_list->head.prev->next = src_list->head.next;
        dest_list->head.prev = src_list->head.prev;
        dtq_init(src_list);
    }
    return;
}
static inline void dtq_free_all(dtq_head_t *list, void (*free_node)(void *))
{
    dtq_node_t *pstCurNode = NULL;
    dtq_node_t *nextNode = NULL;
    DTQ_FOREACH_SAFE(list, pstCurNode, nextNode) {
        free_node(pstCurNode);
    }
    dtq_init(list);
    return;
}

/* rwstq list */

typedef struct rwstq_head {
    stq_head_t head;
    pthread_rwlock_t rwlock;
} rwstq_head_t;

static inline void rwstq_read_lock(rwstq_head_t *node);
static inline bool rwstq_read_trylock(rwstq_head_t *node);
static inline void rwstq_read_unlock(rwstq_head_t *node);
static inline void rwstq_write_lock(rwstq_head_t *node);
static inline bool rwstq_write_trylock(rwstq_head_t *node);
static inline void rwstq_write_unlock(rwstq_head_t *node);
static inline void rwstq_deinit(rwstq_head_t *node);
static inline void rwstq_init(rwstq_head_t *list);
static inline bool rwstq_is_empty(rwstq_head_t *list);
static inline void rwstq_add_head(rwstq_head_t *list, stq_node_t *node);
static inline stq_node_t *rwstq_del_head(rwstq_head_t *list);
static inline void rwstq_add_tail(rwstq_head_t *list, stq_node_t *node);
static inline void rwstq_del(rwstq_head_t *list, const stq_node_t *node);
static inline void rwstq_free_all(rwstq_head_t *list, void (*free_node)(void *));

static inline void rwstq_read_lock(rwstq_head_t *node)
{
    (void)pthread_rwlock_rdlock(&node->rwlock);
    return;
}
static inline bool rwstq_read_trylock(rwstq_head_t *node)
{
    return (0 == pthread_rwlock_tryrdlock(&node->rwlock));
}
static inline void rwstq_read_unlock(rwstq_head_t *node)
{
    (void)pthread_rwlock_unlock(&node->rwlock);
    return;
}
static inline void rwstq_write_lock(rwstq_head_t *node)
{
    (void)pthread_rwlock_wrlock(&node->rwlock);
    return;
}
static inline bool rwstq_write_trylock(rwstq_head_t *node)
{
    return (0 == pthread_rwlock_trywrlock(&node->rwlock));
}
static inline void rwstq_write_unlock(rwstq_head_t *node)
{
    (void)pthread_rwlock_unlock(&node->rwlock);
    return;
}
static inline void rwstq_deinit(rwstq_head_t *node)
{
    (void)pthread_rwlock_destroy(&node->rwlock);
    return;
}
static inline void rwstq_init(rwstq_head_t *list)
{
    stq_init(&list->head);
    (void)pthread_rwlock_init(&list->rwlock, NULL);
    return;
}
static inline bool rwstq_is_empty(rwstq_head_t *list)
{
    bool is_empty;
    rwstq_read_lock(list);
    is_empty = stq_is_empty(&list->head);
    rwstq_read_unlock(list);
    return is_empty;
}
static inline void rwstq_add_head(rwstq_head_t *list, stq_node_t *node)
{
    rwstq_write_lock(list);
    stq_add_head(&list->head, node);
    rwstq_write_unlock(list);
    return;
}
static inline stq_node_t *rwstq_del_head(rwstq_head_t *list)
{
    stq_node_t *node = NULL;
    rwstq_write_lock(list);
    node = stq_del_head(&list->head);
    rwstq_write_unlock(list);
    return node;
}
static inline void rwstq_add_tail(rwstq_head_t *list, stq_node_t *node)
{
    rwstq_write_lock(list);
    stq_add_tail(&list->head, node);
    rwstq_write_unlock(list);
    return;
}
static inline void rwstq_del(rwstq_head_t *list, const stq_node_t *node)
{
    rwstq_write_lock(list);
    stq_del(&list->head, node);
    rwstq_write_unlock(list);
    return;
}
static inline void rwstq_free_all(rwstq_head_t *list, void (*free_node)(void *))
{
    rwstq_write_lock(list);
    stq_free_all(&list->head, free_node);
    rwstq_write_unlock(list);
    return;
}

/* rwdtq list */

typedef struct rwdtq_head {
    dtq_head_t head;
    pthread_rwlock_t rwlock;
} rwdtq_head_t;

static inline void rwdtq_read_lock(rwdtq_head_t *node);
static inline bool rwdtq_read_trylock(rwdtq_head_t *node);
static inline void rwdtq_read_unlock(rwdtq_head_t *node);
static inline void rwdtq_write_lock(rwdtq_head_t *node);
static inline bool rwdtq_write_trylock(rwdtq_head_t *node);
static inline void rwdtq_write_unlock(rwdtq_head_t *node);
static inline void rwdtq_deinit(rwdtq_head_t *node);
static inline void rwdtq_init(rwdtq_head_t *list);
static inline bool rwdtq_is_empty(rwdtq_head_t *list);
static inline bool rwdtq_is_end(rwdtq_head_t *list, const dtq_node_t *node);
static inline void rwdtq_add_head(rwdtq_head_t *list, dtq_node_t *node);
static inline dtq_node_t *rwdtq_del_head(rwdtq_head_t *list);
static inline void rwdtq_add_tail(rwdtq_head_t *list, dtq_node_t *node);
static inline void rwdtq_del(rwdtq_head_t *list, const dtq_node_t *node);
static inline void rwdtq_free_all(rwdtq_head_t *list, void (*free_node)(void *));

static inline void rwdtq_write_lock(rwdtq_head_t *node)
{
    (void)pthread_rwlock_wrlock(&node->rwlock);
    return;
}
static inline bool rwdtq_write_trylock(rwdtq_head_t *node)
{
    return (0 == pthread_rwlock_trywrlock(&node->rwlock));
}
static inline void rwdtq_write_unlock(rwdtq_head_t *node)
{
    (void)pthread_rwlock_unlock(&node->rwlock);
    return;
}

static inline void rwdtq_read_lock(rwdtq_head_t *node)
{
    (void)pthread_rwlock_rdlock(&node->rwlock);
    return;
}

static inline void rwdtq_read_unlock(rwdtq_head_t *node)
{
    (void)pthread_rwlock_unlock(&node->rwlock);
    return;
}

static inline void rwdtq_deinit(rwdtq_head_t *node)
{
    (void)pthread_rwlock_destroy(&node->rwlock);
    return;
}
static inline void rwdtq_init(rwdtq_head_t *list)
{
    dtq_init(&list->head);
    (void)pthread_rwlock_init(&list->rwlock, NULL);
    return;
}
static inline bool rwdtq_is_empty(rwdtq_head_t *list)
{
    bool is_empty;
    rwdtq_read_lock(list);
    is_empty = dtq_is_empty(&list->head);
    rwdtq_read_unlock(list);
    return is_empty;
}
static inline bool rwdtq_is_end(rwdtq_head_t *list, const dtq_node_t *node)
{
    bool is_empty;
    rwdtq_read_lock(list);
    is_empty = dtq_is_end(&list->head, node);
    rwdtq_read_unlock(list);
    return is_empty;
}
static inline void rwdtq_add_head(rwdtq_head_t *list, dtq_node_t *node)
{
    rwdtq_write_lock(list);
    dtq_add_head(&list->head, node);
    rwdtq_write_unlock(list);
    return;
}
static inline dtq_node_t *rwdta_del_head(rwdtq_head_t *list)
{
    dtq_node_t *node = NULL;
    rwdtq_write_lock(list);
    node = dta_del_head(&list->head);
    rwdtq_write_unlock(list);
    return node;
}
static inline void rwdtq_del(rwdtq_head_t *list, const dtq_node_t *node)
{
    rwdtq_write_lock(list);
    node->prev->next = node->next;
    node->next->prev = node->prev;
    rwdtq_write_unlock(list);
    return;
}
static inline void rwdtq_add_tail(rwdtq_head_t *list, dtq_node_t *node)
{
    rwdtq_write_lock(list);
    dtq_add_tail(&list->head, node);
    rwdtq_write_unlock(list);
    return;
}
static inline dtq_node_t *rwdtq_del_tail(rwdtq_head_t *list)
{
    dtq_node_t *node = NULL;
    rwdtq_write_lock(list);
    node = dtq_del_tail(&list->head);
    rwdtq_write_unlock(list);
    return node;
}
static inline void rwdtq_free_all(rwdtq_head_t *list, void (*free_node)(void *))
{
    rwdtq_write_lock(list);
    dtq_free_all(&list->head, free_node);
    rwdtq_write_unlock(list);
    return;
}


#ifdef __cplusplus
extern "C" {
#endif

#endif /* LIST_H */
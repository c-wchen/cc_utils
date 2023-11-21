#include <benchmark/benchmark.h>
#include <pthread.h>
#include <unistd.h>

#define atomic_fetch_sub __sync_fetch_and_sub
#define atomic_fetch_add __sync_fetch_and_add
#define atomic_compare_exchange __sync_bool_compare_and_swap
#define atomic_load(ptr) atomic_fetch_add(ptr, 0)

typedef struct sl_node
{
    struct sl_node *next;
} queue_node_t;

typedef struct cas_queue_head
{
    queue_node_t head;
    queue_node_t *last;
} cas_queue_head_t;


typedef struct spinlock_queue_head
{
    queue_node_t head;
    queue_node_t *last;
    pthread_spinlock_t spin;
} spinlock_queue_head_t;


// CAS无锁队列实现
static inline void cas_queue_init(cas_queue_head_t *queue)
{
    queue->head.next = NULL;
    queue->last = &queue->head;
}

static inline void cas_queue_push(cas_queue_head_t *queue, queue_node_t *node)
{
    queue_node_t *expect = NULL;
    queue_node_t *old_node = NULL;

    do
    {
        do
        {
            old_node = atomic_load(&queue->last);
            expect = NULL;
        } while (!atomic_compare_exchange(&old_node->next, expect, node));
    } while (!atomic_compare_exchange(&queue->last, old_node, node));
}

static inline queue_node_t *cas_queue_pop(cas_queue_head_t *queue, queue_node_t **last)
{
    queue_node_t *ret;
    queue_node_t *old_node = NULL;
    queue_node_t *record_old_node = NULL;

    if (queue->last == &queue->head)
    {
        return NULL;
    }
    ret = queue->head.next;
    old_node = atomic_load(&queue->last);
    record_old_node = old_node;

    if (!atomic_compare_exchange(&queue->last, old_node, &queue->head))
    {
        queue->head.next = record_old_node->next;
    }
    else
    {
        queue->head.next = NULL;
    }
    *last = record_old_node;
    return ret;
}

// spinlock 锁队列

static inline void spinlock_queue_init(spinlock_queue_head_t *queue)
{
    queue->head.next = NULL;
    queue->last = &queue->head;
    pthread_spin_init(&queue->spin, PTHREAD_PROCESS_PRIVATE);
}

static inline void spinlock_queue_push(spinlock_queue_head_t *queue, queue_node_t *node)
{
    pthread_spin_lock(&queue->spin);
    queue->last->next = node;
    queue->last = node;
    pthread_spin_unlock(&queue->spin);
}

static inline queue_node_t *spinlock_queue_pop(spinlock_queue_head_t *queue, queue_node_t **last)
{
    queue_node_t *ret;
    queue_node_t *old_node = NULL;
    queue_node_t *record_old_node = NULL;
    if (queue->last == &queue->head) {
        return NULL;
    }
    pthread_spin_lock(&queue->spin);
    ret = queue->head.next;
    *last = queue->last;
    queue->last = &queue->head;
    queue->head.next = NULL;

    pthread_spin_unlock(&queue->spin);

    return ret;
}


typedef struct
{
    uint64_t tid;
    uint64_t idx;
    queue_node_t node;
} data_node_t;

typedef struct
{
    
    int32_t index;
    int32_t count;
} record_t;

data_node_t *dnodes;
cas_queue_head_t cas_dqueue;
spinlock_queue_head_t spinlock_dqueue;

void *test_cas_queue_producer(void *arg)
{
    record_t *record = (record_t *)arg;
    for (int i = 0; i < record->count; i++)
    {
        cas_queue_push(&cas_dqueue, &dnodes[record->index + i].node);
    }
    return NULL;
}

void *test_cas_queue_consume(void *arg)
{
    uint64_t sum = (uint64_t)arg;
    uint64_t count = 1;
    queue_node_t *first = NULL, *last = NULL, *next = NULL, *node = NULL;
    do
    {
        last = NULL;
        first = cas_queue_pop(&cas_dqueue, &last);
        if (first == NULL)
        {
            continue;
        }
        next = first;
        do
        {
            node = next;
            ++count;
            next = node->next;
        } while (node != last);
        // printf("consume %p %p %d %d\n", first, last, count, sum);
    } while (count < sum);
    return NULL;
}


void *test_spinlock_queue_producer(void *arg)
{
    record_t *record = (record_t *)arg;
    for (int i = 0; i < record->count; i++)
    {
        spinlock_queue_push(&spinlock_dqueue, &dnodes[record->index + i].node);
    }
    return NULL;
}

void *test_spinlock_queue_consume(void *arg)
{
    uint64_t sum = (uint64_t)arg;
    uint64_t count = 1;
    queue_node_t *first = NULL, *last = NULL, *next = NULL, *node = NULL;
    do
    {
        last = NULL;
        first = spinlock_queue_pop(&spinlock_dqueue, &last);
        if (first == NULL)
        {
            continue;
        }
        next = first;
        do
        {
            node = next;
            ++count;
            next = node->next;
        } while (node != last);
        // printf("consume %p %p %d %d\n", first, last, count, sum);
    } while (count < sum);
    return NULL;
}

#define SINGLE_PRODUCER_CNT 1000
#define PRODUCER_THREAD 1
#define CONSUME_THREAD 1
#define THREAD (PRODUCER_THREAD + CONSUME_THREAD)


static void cas_queue_bench(benchmark::State &state)
{
    pthread_t threads[PRODUCER_THREAD + CONSUME_THREAD] = {0};
    dnodes = (data_node_t *)calloc(1, sizeof(data_node_t) * PRODUCER_THREAD * SINGLE_PRODUCER_CNT);
    
    record_t records[PRODUCER_THREAD] = {0};
    for (int i = 0; i < PRODUCER_THREAD; i++) {
        records[i].index = i * SINGLE_PRODUCER_CNT;
        records[i].count = SINGLE_PRODUCER_CNT;
    }
    for (auto _ : state)
    {
        cas_queue_init(&cas_dqueue);
        
        for (int i = 0; i < CONSUME_THREAD; i++) {
            pthread_create(&threads[i], NULL, test_cas_queue_consume, (void *)(uint64_t)(PRODUCER_THREAD * SINGLE_PRODUCER_CNT));
        }

        for (int i = 0; i < PRODUCER_THREAD; i++) {
             pthread_create(&threads[i + CONSUME_THREAD], NULL, test_cas_queue_producer, &records[i]);
        }

        for (int i = 0; i < THREAD; i++) {
            pthread_join(threads[i], NULL);
        }
    }
    free(dnodes);
}

static void spin_lock_bench(benchmark::State &state)
{
    pthread_t threads[PRODUCER_THREAD + CONSUME_THREAD] = {0};
    dnodes = (data_node_t *)calloc(1, sizeof(data_node_t) * PRODUCER_THREAD * SINGLE_PRODUCER_CNT);
    
    record_t records[PRODUCER_THREAD] = {0};
    for (int i = 0; i < PRODUCER_THREAD; i++) {
        records[i].index = i * SINGLE_PRODUCER_CNT;
        records[i].count = SINGLE_PRODUCER_CNT;
    }
    for (auto _ : state)
    {
        spinlock_queue_init(&spinlock_dqueue);
        
        for (int i = 0; i < CONSUME_THREAD; i++) {
            pthread_create(&threads[i], NULL, test_spinlock_queue_consume, (void *)(uint64_t)(PRODUCER_THREAD * SINGLE_PRODUCER_CNT));
        }

        for (int i = 0; i < PRODUCER_THREAD; i++) {
             pthread_create(&threads[i + CONSUME_THREAD], NULL, test_spinlock_queue_producer, &records[i]);
        }

        for (int i = 0; i < THREAD; i++) {
            pthread_join(threads[i], NULL);
        }
    }
    free(dnodes);
}

BENCHMARK(spin_lock_bench);
BENCHMARK(cas_queue_bench);

BENCHMARK_MAIN();
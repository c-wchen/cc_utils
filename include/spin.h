#ifndef ATOMIC_H
#define ATOMIC_H

#include <stddef.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <stdbool.h>

typedef struct spinlock {
    atomic_bool val;
} spinlock;

#define SPINLOCK_STATIC_INITIALIZER()  {0}

static inline void spinlock_acquire(spinlock *p_lock)
{
    while (atomic_flag_test_and_set_explicit(&p_lock->val, memory_order_acquire)) {
        while (atomic_load_explicit(&p_lock->val, memory_order_acquire) != false)
            ;
    }
}

/* Return FALSE if the lock is acquired. */
static inline bool spinlock_try_acquire(spinlock *p_lock)
{
    return atomic_flag_test_and_set_explicit(&p_lock->val, memory_order_acquire) ? true : false;
}

static inline void spinlock_release(spinlock *p_lock)
{
    atomic_flag_clear_explicit(&p_lock->val, memory_order_release);
}

#ifdef __cplusplus
}
#endif

#endif /* _MOCK_H_ */

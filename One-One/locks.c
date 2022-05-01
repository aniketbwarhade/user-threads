#define _GNU_SOURCE
#include <linux/futex.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "locks.h"

// @credit:- xv6 code and os lecture
/*
 *   Logic of synchronization code is referred from a xv6 code, link is pasted below:
 *   https://github.com/mit-pdos/xv6-public
 */

void thread_spin_init(spinlock *spin_lock)
{
    spin_lock->islocked = 0;
}

void thread_mutex_init(mutex *m)
{
    int i;
    m->islocked = 0;
    m->futex_word = 0;
    thread_spin_init(&m->spin_lock);
}

int xchg(int *addr, int newval)
{
    int result;

    // The + in "+m" denotes a read-modify-write operand.
    asm volatile("lock; xchgl %0, %1"
                 : "+m"(*addr), "=a"(result)
                 : "1"(newval)
                 : "cc");
    return result;
}

void thread_spin_lock(spinlock *spin_lock)
{
    // The xchg is atomic.
    while (xchg(&(spin_lock->islocked), 1) != 0)
        ;

    // Tell the C compiler and the processor to not move loads or stores
    // past this point, to ensure that all the stores in the critical
    // section are visible to other cores before the lock is released.
    // Both the C compiler and the hardware may re-order loads and
    // stores; __sync_synchronize() tells them both not to.
    __sync_synchronize();
}
// @credit:- xv6 code
void thread_spin_unlock(spinlock *spin_lock)
{
    // Tell the C compiler and the processor to not move loads or stores
    // past this point, to ensure that all the stores in the critical
    // section are visible to other cores before the lock is released.
    // Both the C compiler and the hardware may re-order loads and
    // stores; __sync_synchronize() tells them both not to.
    __sync_synchronize();

    // Release the lock, equivalent to lk->locked = 0.
    // This code can't use a C assignment, since it might
    // not be atomic. A real OS would use C atomics here.
    asm volatile("movl $0, %0"
                 : "+m"(spin_lock->islocked)
                 :);
}

void thread_mutex_block(mutex *m, spinlock *sl)
{
    thread_spin_unlock(sl);
    syscall(SYS_futex, &(m->islocked), FUTEX_WAIT, 1, NULL, NULL, 0);
    thread_spin_lock(sl);
}

void thread_mutex_lock(mutex *m)
{
    thread_spin_lock(&m->spin_lock);
    while (m->islocked)
        thread_mutex_block(m, &m->spin_lock);
    m->islocked = 1;
    thread_spin_unlock(&m->spin_lock);
}

void thread_mutex_unlock(mutex *m)
{
    int i, index;
    thread_spin_lock(&m->spin_lock);
    m->islocked = 0;
    syscall(SYS_futex, &(m->islocked), FUTEX_WAKE, 1, NULL, NULL, 0);
    thread_spin_unlock(&m->spin_lock);
}

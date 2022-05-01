#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdatomic.h>
#include <linux/futex.h>
#include <errno.h>

typedef unsigned int spinlock;
typedef unsigned int mutex;

// synchronization using spinlocks
int thread_spin_init(spinlock *lock);
int thread_spin_lock(spinlock *lock);
int thread_spin_unlock(spinlock *lock);
int thread_spin_trylock(spinlock *lock);

// synchronization using mutex
int thread_mutex_init(mutex *mutex);
int thread_mutex_lock(mutex *mutex);
int thread_mutex_unlock(mutex *mutex);
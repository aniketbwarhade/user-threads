
/*spinlock mutex structure*/
typedef struct spinlock
{
    int islocked;
} spinlock;

typedef struct mutex
{
    int islocked;
    spinlock spin_lock;
    int futex_word;
} mutex;

void thread_spin_init(spinlock *spin_lock);

void thread_spin_lock(spinlock *spin_lock);

void thread_spin_unlock(spinlock *spin_lock);

void thread_mutex_init(mutex *m);

void thread_mutex_block(mutex *m, spinlock *sl);

void thread_mutex_lock(mutex *m);

void thread_mutex_unlock(mutex *m);

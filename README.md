Lockers

There are 2 libraries implementing 2 types of MutEx es and Semaphores, both for linux.

---------------------
lockers.h
Mutual exlusion provided by the usage of a queue.

MutEx:
   Prevents starvation by using a queue of waiting threads.
   Uses sleep and signals to reduce overhead.

Semaphore:
    Has fixed number of resources.
    Prevents starvation by using a queue of waiting threads.
    Uses sleep and signals to reduce overhead.

---------------------
lockers2.h
Similar to pthread mutex and semaphore. Mutual exclusion provided by using atomic CAS.

MutEx:
   Uses atomic CAS.
   No starvation prevention (maybe the futex provides it).
   Uses futexes for reduced overhead.

Semaphore:
    Uses the defined mutex to work with the semaphore values.
    No starvation prevention (maybe the futex provides it).
    Uses futexes for reduced overhead.
    

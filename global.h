#ifndef GLOBAL_H
#define GLOBAL_H

#include "process.h"
#include <pthread.h>

extern int msid; 
extern key_t repo_key;
extern process list[100];
extern int cnt;
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;

#endif
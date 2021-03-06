#ifdef __APPLE__

#ifndef PTHREAD_BARRIER_H_
#define PTHREAD_BARRIER_H_

#include <pthread.h>
#include <errno.h>

typedef int pthread_barrierattr_t;
#define PTHREAD_BARRIER_SERIAL_THREAD 1

typedef struct
{
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  unsigned limit;
  unsigned count;
  unsigned phase;
} pthread_barrier_t;


int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
  if (count == 0) {
    errno = EINVAL;
    return -1;
  }
  if (pthread_mutex_init(&barrier->mutex, 0) < 0)
    return -1;
  if (pthread_cond_init(&barrier->cond, 0) < 0) {
    pthread_mutex_destroy(&barrier->mutex);
    return -1;
  }
  barrier->limit = count;
  barrier->count = 0;
  barrier->phase = 0;
  return 0;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
  pthread_mutex_destroy(&barrier->mutex);
  pthread_cond_destroy(&barrier->cond);
  return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier)
{
  pthread_mutex_lock(&barrier->mutex);
  if (++barrier->count >= barrier->limit) {
    barrier->phase++;
    barrier->count = 0;
    pthread_cond_broadcast(&barrier->cond);
    pthread_mutex_unlock(&barrier->mutex);
    return PTHREAD_BARRIER_SERIAL_THREAD;
  } else {
    unsigned phase = barrier->phase;
    do
      pthread_cond_wait(&barrier->cond, &barrier->mutex);
    while (phase == barrier->phase);
    pthread_mutex_unlock(&barrier->mutex);
    return 0;
  }
}

#endif // PTHREAD_BARRIER_H_
#endif // __APPLE__

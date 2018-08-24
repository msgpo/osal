
#define __USE_GNU

#include <sched.h>
#include "osal.h"

#include <string.h>
#include <errno.h>
#include <stdio.h>

osal_error_t OsalThreadCreate(thread_t* t, const unsigned int priority, const char name[32], void* (fn)(void*), void* args)
{
	pthread_attr_t a;
	struct sched_param p;
	
	pthread_attr_init(&a);

	p.sched_priority = priority;
	t->priority = priority;
	
	(void) OsalMutexCreate(&t->mutex);
	
	if (name != NULL)
	{
		strncpy(t->name, name, 31);
		t->name[31] = '\0';	
	}
	else
	{
		strcpy(t->name, "(no name)");
	}
	
	pthread_attr_setinheritsched(&a, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&a, SCHED_FIFO);
	pthread_attr_setschedparam(&a, &p);

	if (pthread_create(&t->Id, &a, fn, args))
	{
		fprintf(stderr, "pthread_create failed: %s\n", strerror(errno));
	}
	
	
	#if 0
	int policy = 0;
	pthread_getschedparam(*t, &policy, &p);
	fprintf(stderr, "ThreadCreate %llu, policy %s, priority %d\n", *t, (policy==SCHED_FIFO? "SCHED_FIFO":"Other"), p.sched_priority);
	#endif
	return OSAL_OK;
}

osal_error_t OsalThreadDestroy(thread_t* t)
{
	
	int r = pthread_cancel(t->Id);
	
	if (r == 0)
	{
		return OSAL_OK;
	}
}

osal_error_t OsalThreadSetPriority(thread_t* t, const uint32_t priority)
{
	OsalMutexLock(&t->mutex);
	pthread_t targetThread = t->Id;
	
	if (targetThread == 0)
	{
		targetThread = pthread_self();
	}
	
	if (pthread_setschedprio(targetThread, priority))
	{
		fprintf(stderr, "pthread_setschedprio(): %s\n", strerror(errno));
		OsalMutexUnlock(&t->mutex);
	
		return OSAL_FAIL;
	}
	
	t->priority = priority;
	
	#if 0
	struct sched_param p;
	
	int policy = 0;
	pthread_getschedparam(targetThread, &policy, &p);
	fprintf(stderr, "ThreadSetPriority %llu, policy %s, priority %d\n", targetThread, (policy==SCHED_FIFO? "SCHED_FIFO":"Other"), p.sched_priority);
	#endif
	OsalMutexUnlock(&t->mutex);
	
	return OSAL_OK;
}

osal_error_t OsalThreadGetPriority(thread_t* t, uint32_t* priority)
{
	OsalMutexLock(&t->mutex);
	*priority = t->priority;
	OsalMutexUnlock(&t->mutex);
	
	return OSAL_OK;
}

osal_error_t OsalThreadSuspend(thread_t t)
{

}

osal_error_t OsalThreadResume(thread_t t)
{

}

/*osal_error_t ThreadYield()
{
	pthread_yield();
}*/

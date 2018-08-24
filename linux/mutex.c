
#include "osal.h"
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

osal_error_t OsalMutexCreate(osal_mutex* m)
{
	osal_error_t status = OSAL_FAIL;
	
	pthread_mutexattr_t attr;
	if (pthread_mutexattr_init(&attr))
	{
		printf("pthread_mutexattr_init() %s\n", strerror(errno));
		return OSAL_FAIL;
	}
	else if (pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT))
	{
		printf("pthread_mutexattr_setprotocol() %s\n", strerror(errno));
	}
	else if (pthread_mutex_init(m, &attr))
	{
		printf("pthread_mutex_init() %s\n", strerror(errno));
	}
	else
	{
		status = OSAL_OK;
	}
	
	pthread_mutexattr_destroy(&attr);
	
	return status;
}

osal_error_t OsalTimedWait(osal_mutex* m, const uint32_t milliSeconds)
{
	//pthread_mutex_timedlock(m, );
}

void OsalMutexLock(osal_mutex* m)
{
	pthread_mutex_lock(m);
}

void OsalMutexUnlock(osal_mutex* m)
{
	pthread_mutex_unlock(m);
}


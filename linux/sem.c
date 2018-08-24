
#include "osal.h"

#include <stdio.h>	// eerror messages
#include <errno.h>
#include <string.h>

osal_error_t OsalSemCreate(osal_sem* sem, const unsigned int value)
{
	if(sem_init(sem, 0, value))
	{
		fprintf(stderr, "OsalSemCreate failed: %s\n", strerror(errno));
		return OSAL_FAIL;
	}
	return OSAL_OK;
}

osal_error_t OsalSemTimedWait(osal_sem* sem, const uint32_t milliSeconds)
{
	osal_error_t result = OSAL_OK;
	
	if (milliSeconds == 0)
	{
		if (sem_trywait(sem))
		{
			result = OSAL_TIMEOUT;
		}
	}
	else
	{
		struct timespec abs_timeout;

    	clock_gettime(CLOCK_REALTIME, &abs_timeout);
    
		const uint64_t new_nsec = abs_timeout.tv_nsec + (milliSeconds * 1000000);
		abs_timeout.tv_sec += new_nsec / 1000000000;
		abs_timeout.tv_nsec = new_nsec % 1000000000;
		
		if (sem_timedwait(sem, &abs_timeout))
		{
			result = OSAL_TIMEOUT;
		}
	}
	
	return result;
}

void OsalSemWait(osal_sem* sem)
{
	(void) sem_wait(sem);
}

void OsalSemPost(osal_sem* sem)
{
	(void) sem_post(sem);
}




#include "osal.h"
#include <pthread.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static event_t events; 

event_t OsalEventWaitAny(const event_t eventMask)
{
	event_t result;
	pthread_mutex_lock(&mutex);

	if (events & eventMask)
	{
		result = events & eventMask;
		events &= ~eventMask;
	}
	else
	{
		for (;;)
		{
			pthread_cond_wait(&cond, &mutex);
			result = events & eventMask;
			
			if (result)
			{
				events &= ~eventMask;
				break;
			}	
		}
	}
	pthread_mutex_unlock(&mutex);
	return result;
}

event_t OsalEventWaitAll(const event_t eventMask)
{
	event_t result;
	pthread_mutex_lock(&mutex);

	if ((events & eventMask) == eventMask)
	{
		result = events & eventMask;
		events &= ~eventMask;
	}
	else
	{
		for (;;)
		{
			pthread_cond_wait(&cond, &mutex);
			result = events & eventMask;
			
			if (result == eventMask)
			{
				events &= ~eventMask;
				break;
			}	
		}
	}
	pthread_mutex_unlock(&mutex);
	return result;
}

void OsalEventSend(const event_t e)
{
	pthread_mutex_lock(&mutex);
	
	events |= e;
	
	pthread_cond_broadcast( &cond );
	pthread_mutex_unlock(&mutex);

}

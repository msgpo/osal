
#ifndef OSAL_H
#define OSAL_H

#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>

typedef enum
{
	OSAL_OK,
	OSAL_FAIL,
	OSAL_TIMEOUT,
	OSAL_THREAD_INVALID,
	OSAL_MSGQUEUE_FULL,
	OSAL_MSGQUEUE_TOOBIG
} osal_error_t;

//---------------------------------------------------------
// Library
//---------------------------------------------------------

osal_error_t OsalInit(void);

//---------------------------------------------------------
// Mutex
//---------------------------------------------------------

typedef pthread_mutex_t osal_mutex;

osal_error_t OsalMutexCreate(osal_mutex* m);

osal_error_t OsalMutexTimedWait(osal_mutex* m, const uint32_t milliSeconds);

void OsalMutexLock(osal_mutex* m);

void OsalMutexUnlock(osal_mutex* m);

//---------------------------------------------------------
// Threading
//---------------------------------------------------------
typedef struct
{
	pthread_t Id;
	char name[32];
	uint32_t priority;
	osal_mutex mutex;
} thread_t;


osal_error_t OsalThreadCreate(thread_t* t, const unsigned int priority, const char name[32], void* (fn)(void*), void* args);

osal_error_t OsalThreadDestroy(thread_t* t);

osal_error_t OsalThreadSetPriority(thread_t* t, const uint32_t priority);

osal_error_t OsalThreadGetPriority(thread_t* t, uint32_t* priority);

// osal_error_t ThreadSuspend(thread_t t);

// osal_error_t ThreadResume(thread_t t);

//osal_error_t ThreadYield();

//---------------------------------------------------------
// Events
//---------------------------------------------------------

typedef uint64_t event_t;

event_t OsalEventWaitAny(const event_t eventMask);

event_t OsalEventWaitAll(const event_t eventMask);

void OsalEventSend(event_t e);


//---------------------------------------------------------
// Message Queues
//---------------------------------------------------------

typedef struct _queue_t * queue_t;

queue_t OsalMQCreate(const uint32_t maxMsgs, const uint32_t maxMsgSize);

osal_error_t OsalMQDestroy(queue_t q);

void OsalMQClear(queue_t q);

osal_error_t OsalMQSend(queue_t q, const char buffer[], const uint32_t bufferSize);

osal_error_t OsalMQSendStr(queue_t q, const char szBuffer[]);

osal_error_t OsalMQReceive(queue_t q, char buffer[], const uint32_t bufferSize, uint32_t* msgLen);

uint32_t OsalMQMessageCount(queue_t q);

//---------------------------------------------------------
// Semaphore
//---------------------------------------------------------

typedef sem_t osal_sem;

osal_error_t OsalSemCreate(osal_sem* sem, const unsigned int value);

osal_error_t OsalSemTimedWait(osal_sem* sem, const uint32_t milliSeconds);

void OsalSemWait(osal_sem* sem);

void OsalSemPost(osal_sem* sem);

//---------------------------------------------------------
#endif

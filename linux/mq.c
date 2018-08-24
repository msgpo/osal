
#include <stdlib.h>
#include <fcntl.h>           /* For O_* constants */
#include <string.h>
#include <pthread.h>

#include "osal.h"

typedef struct _queue_t
{
	uint32_t maxMsgCount;
	uint32_t maxMsgSize;
	uint32_t queueStart;
	uint32_t queueEnd;
	uint32_t MsgCount;
	uint8_t* buffer;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	
} * queue_t;

queue_t OsalMQCreate(const uint32_t maxMsgs, const uint32_t maxMsgSize)
{
	struct _queue_t* q = malloc(sizeof(struct _queue_t));
	
	if (q != NULL)
	{
		q->maxMsgCount = maxMsgs;
		q->maxMsgSize = maxMsgSize;
		q->queueStart = 0U;
		q->queueEnd = 0U;
		q->MsgCount = 0U;
		q->buffer = (uint8_t*)malloc(maxMsgs * (maxMsgSize + sizeof(maxMsgSize)));
		
		pthread_mutex_init(&q->mutex, NULL);
		pthread_cond_init(&q->cond, NULL);
		
		if (q->buffer == NULL)
		{
			free(q);
			q = NULL;
		}
	}

	return q;
}

osal_error_t OsalMQDestroy(queue_t q)
{
	if (q != NULL)
	{
		// Ensure no thread is currently doing anything with the queue
		
		free(q->buffer);
		pthread_mutex_destroy(&q->mutex);
		pthread_cond_destroy(&q->cond);
		free(q);
		return OSAL_OK;
	}
	return OSAL_FAIL;
}

void OsalMQClear(queue_t q)
{
	if (q != NULL)
	{
		pthread_mutex_lock(&q->mutex);
		q->queueStart = 0U;
		q->queueEnd = 0U;
		q->MsgCount = 0U;
		pthread_mutex_unlock(&q->mutex);
	}
}

osal_error_t OsalMQSend(queue_t q, const char buffer[], const uint32_t bufferSize)
{
	if (q != NULL && buffer != NULL && bufferSize > 0U)
	{
		pthread_mutex_lock(&q->mutex);
		
		if (q->MsgCount >= q->maxMsgCount)
		{
			pthread_mutex_unlock(&q->mutex);
			return OSAL_MSGQUEUE_FULL;
		}
		if (bufferSize > q->maxMsgSize)
		{
			pthread_mutex_unlock(&q->mutex);
			return OSAL_MSGQUEUE_TOOBIG;
		}
		
		uint32_t* msgLen = (uint32_t*)&q->buffer[sizeof(q->maxMsgSize) * q->queueEnd];
		*msgLen = bufferSize;
		memcpy(&q->buffer[sizeof(q->maxMsgSize) * q->maxMsgCount + q->maxMsgSize * q->queueEnd], buffer, bufferSize);
		
		++q->queueEnd;
		if (q->queueEnd == q->maxMsgCount)
		{
			q->queueEnd = 0U;
		}
		
		++q->MsgCount;
		
		pthread_cond_broadcast(&q->cond);
		pthread_mutex_unlock(&q->mutex);
		
		return OSAL_OK;
	}
	
	return OSAL_FAIL;
}

osal_error_t OsalMQSendStr(queue_t q, const char szBuffer[])
{
	if (q != NULL && szBuffer != NULL)
	{
		const uint32_t strLen = strnlen(szBuffer, q->maxMsgSize - 1);
		char buffer[strLen+1];
		memcpy(buffer,szBuffer,strLen+1);
		if (strLen == q->maxMsgSize)
		{
			buffer[strLen] = '\0';
		}
		
		OsalMQSend(q, buffer, strLen + 1);
	}
	return OSAL_FAIL;
}

osal_error_t OsalMQReceive(queue_t q, char buffer[], const uint32_t bufferSize, uint32_t* msgLength)
{
	if (q != NULL && buffer != NULL && bufferSize > 0U)
	{
		pthread_mutex_lock(&q->mutex);
			
		while (q->MsgCount == 0U)
		{
			pthread_cond_wait(&q->cond, &q->mutex ); 
		};
		
		const uint32_t* const pMsgLen = (uint32_t*)&q->buffer[sizeof(q->maxMsgSize) * q->queueStart];
		uint32_t copyLen = bufferSize;
		if (*pMsgLen < copyLen)
		{
			copyLen = *pMsgLen;
		}
		
		*msgLength = *pMsgLen;
		
		memcpy(buffer, &q->buffer[sizeof(q->maxMsgSize) * q->maxMsgCount + q->maxMsgSize * q->queueStart], copyLen);
		
		++q->queueStart;
		if (q->queueStart == q->maxMsgCount)
		{
			q->queueStart = 0U;
		}
		
		--q->MsgCount;
		
		pthread_mutex_unlock(&q->mutex);
		return OSAL_OK;
	}
	
	return OSAL_FAIL;
}

uint32_t OsalMQMessageCount(queue_t q)
{
	uint32_t result = -1;

	if (q != NULL)
	{
		pthread_mutex_lock(&q->mutex);
		result = q->MsgCount;
		pthread_mutex_unlock(&q->mutex);
	}
	
	return result;
}


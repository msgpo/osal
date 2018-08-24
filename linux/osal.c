

#include "osal.h"

#include <stdio.h>
#include <sys/mman.h> // mlockall

osal_error_t OsalInit(void)
{
	static int bInitialised = 0;
	osal_error_t result = OSAL_FAIL;
	
	if (!bInitialised)
	{
		bInitialised = 1;
		
		if (mlockall(MCL_CURRENT | MCL_FUTURE))
		{
			fprintf(stderr, "Failed to lock memory\n");
		}
		else
		{
			result = OSAL_OK;
		}
	}
	
	return result;
}

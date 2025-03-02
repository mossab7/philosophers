#include "philosophers.h"

void error_log(char *msg)
{
	if(msg)
	{
		while(*msg)
			write(2,msg++,1);
		if(*(msg - 1) != '\n')
			write(2,"\n",1);
	}
}

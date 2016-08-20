/* Every.c
 * 	Repeating task
 *
 * This file is part of Marcel project and is following the same
 * license rules (see LICENSE file)
 *
 * 07/09/2015	- LF - First version
 * 17/05/2016	- LF - Add 'Immediate'
 * 06/06/2016	- LF - If sample is null, stop 
 */
#ifdef LUA	/* Only useful with Lua support */

#include "Every.h"
#include "Marcel.h"

#include <stdlib.h>
#include <unistd.h>
#include <lauxlib.h>

void *process_Every(void *actx){
	struct _Every *ctx = actx;	/* Only to avoid zillions of cast */

	if(ctx->funcname && ctx->funcid == LUA_REFNIL){
		if( (ctx->funcid = findUserFunc( ctx->funcname )) == LUA_REFNIL ){
			fprintf(stderr, "*F* configuration error : user function \"%s\" is not defined\n", ctx->funcname);
			exit(EXIT_FAILURE);
		}
	}

	if(verbose)
		printf("Launching a processing flow for '%s' Every task\n", ctx->funcname);

	if(ctx->immediate && !ctx->disabled)
		execUserFuncEvery( ctx );

	for(;;){
		if(!ctx->sample){
			if(verbose)
				printf("*I* Every '%s' has 0 sample delay : dying ...\n", ctx->funcname);
			break;
		} else
			sleep( ctx->sample );
		if( ctx->disabled ){
			if(verbose)
				printf("*I* Every is disabled for function '%s'\n", ctx->funcname);
		} else 
			execUserFuncEvery( ctx );
	}

	pthread_exit(0);
}

#endif /* LUA */

/* DeadPublisherDetection.c
 * 	DPD processing
 *
 * This file is part of Marcel project and is following the same
 * license rules (see LICENSE file)
 *
 * 09/07/2015	- LF - First version
 */

#include "DeadPublisherDetection.h"
#include "MQTT_tools.h"

#include <sys/time.h>
#include <sys/select.h>

#include <sys/eventfd.h>
#include <unistd.h>
#include <string.h>

extern void *process_DPD(void *actx){
	struct _DeadPublisher *ctx = actx;	/* Only to avoid zillions of cast */
	struct timespec ts;

		/* Sanity checks */
	if(!ctx->topic){
		fputs("*E* configuration error : no topic specified, ignoring this section\n", stderr);
		pthread_exit(0);
	}
	if(!ctx->sample){
		fprintf(stderr, "*E* configuration error : no timeout specified for DPD '%s', ignoring this section\n", ctx->topic);
		pthread_exit(0);
	}
	if(!*ctx->errorid){
		fprintf(stderr, "*E* configuration error : no errorid specified for DPD '%s', ignoring this section\n", ctx->topic);
		pthread_exit(0);
	}

	if(debug)
		printf("Launching a processing flow for DeadPublisherDetect (DPD) '%s'\n", ctx->topic);

		/* Creating the fd for the notification */
	if(( ctx->rcv = eventfd( 0, 0 )) == -1 ){
		perror("eventfd()");
		pthread_exit(0);
	}

	if( MQTTClient_subscribe( cfg.client, ctx->topic, 0 ) != MQTTCLIENT_SUCCESS ){
		close( ctx->rcv );
		ctx->rcv = 1;
		fprintf(stderr, "Can't subscribe to '%s'\n", ctx->topic );
		pthread_exit(0);
	}

	ts.tv_sec = (time_t)ctx->sample;
	ts.tv_nsec = 0;

	for(;;){
		fd_set rfds;
		FD_ZERO( &rfds );
		FD_SET( ctx->rcv, &rfds );

		switch( pselect( ctx->rcv+1, &rfds, NULL, NULL, &ts, NULL ) ){
		case -1:	/* Error */
			close( ctx->rcv );
			ctx->rcv = 1;
			perror("pselect()");
			pthread_exit(0);
		case 0:	/* timeout */
			if(debug)
				printf("*I* timeout for DPD '%s'\n", ctx->errorid);
			if( !ctx->inerror ){
				char topic[strlen(ctx->errorid) + 7]; /* "Alert/" + 1 */
				const char *msg_info = "SNo data received after %d seconds";
				char msg[ strlen(msg_info) + 15 ];	/* Some room for number of seconds */
				int msg_len;

				strcpy( topic, "Alert/" );
				strcat( topic, ctx->errorid );

				msg_len = sprintf( msg, msg_info, ctx->sample );

				if( mqttpublish( cfg.client, topic, msg_len, msg, 0 ) == MQTTCLIENT_SUCCESS ){
					if(debug)
						printf("*I* Alert raises for DPD '%s'\n", ctx->errorid);
						ctx->inerror = 1;
					}
			}
			break;
		default:{	/* Got some data */
				uint64_t v;
				read(ctx->rcv, &v, sizeof( uint64_t ));	/* AF : normally, return value has to be check */
				if( ctx->inerror ){
					char topic[strlen(ctx->errorid) + 7]; /* "Alert/" + 1 */
					strcpy( topic, "Alert/" );
					strcat( topic, ctx->errorid );
					if( mqttpublish( cfg.client, topic, 1, "E", 0 ) == MQTTCLIENT_SUCCESS ){
						if(debug)
							printf("*I* Alert corrected for DPD '%s'\n", ctx->errorid);
						ctx->inerror = 0;
					}
				}
			}
			break;
		}
	}

	close( ctx->rcv );
	ctx->rcv = 1;
	
	pthread_exit(0);
}

/* Alerting.h
 * 	Definitions related to Alerting
 *
 * This file is part of Marcel project and is following the same
 * license rules (see LICENSE file)
 *
 * 16/07/2015	- LF - First version
 * 27/03/2016	- LF - replace AlertCmd() with SendAlert()
 */

#ifndef ALERTING_H
#define ALERTING_H

#include "DList.h"

	/* Active alert list */
struct alert {
	struct DLNode node;
	const char *alert;
};

extern struct DList alerts;

extern void init_alerting(void);
extern void RiseAlert(const char *id, const char *msg, int withSMS);
extern void AlertIsOver(const char *id);
extern void SendAlert(const char *id, const char *msg, int withSMS);
extern void pnNotify(const char *names, const char *title, const char *msg);

extern void rcv_alert(const char *id, const char *msg);
extern void rcv_notification(const char *id, const char *msg);
extern void rcv_nnotification(const char *names, const char *msg);

#endif

#ifndef IRODS_TICKET_ADMIN_H
#define IRODS_TICKET_ADMIN_H

#include "irods/objInfo.h"

/** Client connection type used by ticket administration APIs. */
struct RcComm;

/** Input for ticket administration operations. */
typedef struct TicketAdminInput {
    /** Primary ticket administration argument. */
    char *arg1;
    /** Secondary ticket administration argument. */
    char *arg2;
    /** Third ticket administration argument. */
    char *arg3;
    /** Fourth ticket administration argument. */
    char *arg4;
    /** Fifth ticket administration argument. */
    char *arg5;
    /** Sixth ticket administration argument. */
    char *arg6;
    /** Conditional input keywords for ticket administration. */
    struct KeyValPair condInput;
} ticketAdminInp_t;

/** Packing instruction string for ::ticketAdminInp_t. */
#define ticketAdminInp_PI "str *arg1; str *arg2; str *arg3; str *arg4; str *arg5; str *arg6; struct KeyValPair_PI;"

#ifdef __cplusplus
extern "C"
#endif
/**
 * Execute a ticket administration command.
 *
 * @param[in] conn Client connection handle.
 * @param[in] ticketAdminInp Ticket administration request input.
 *
 * @return Operation status.
 */
int rcTicketAdmin(struct RcComm* conn, struct TicketAdminInput* ticketAdminInp);

#endif // IRODS_TICKET_ADMIN_H

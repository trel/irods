/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/***************************************************************************

   This file contains all the structuresdefinitions used by ICAT.

****************************************************************************/

#ifndef ICAT_STRUCTS_HPP
#define ICAT_STRUCTS_HPP

/// \file

#include "irods/icatDefines.h"

/// \brief Describes a database statement used by the ICAT layer.
typedef struct {
    int status; ///< Statement execution status.
    void*   stmtPtr;                            /* internal db statemnt handle */ ///< Internal database statement handle.
    int     numOfCols;                          /* number of result columns */ ///< Number of result columns.
    char    *resultColName[MAX_NUM_OF_SELECT_ITEMS];  /* column names */ ///< Result column names.
    int     selectColIds[MAX_NUM_OF_SELECT_ITEMS];  /* rods-id to column in the
                                                     result (unused, so far) */ ///< Column identifiers for the select list.
    char    *resultValue[MAX_NUM_OF_SELECT_ITEMS];  /* pointer to data area */ ///< Result value pointers.
} icatStmtStrct;



/// \brief Describes an ICAT database session and its active statements.
typedef struct {
    int         status; ///< Session status.
    void*       environPtr;       /* internal db environment handle */ ///< Internal database environment handle.
    void*       connectPtr;       /* internal db connection handle */ ///< Internal database connection handle.
    icatStmtStrct* stmtPtr[MAX_NUM_OF_CONCURRENT_STMTS];  /* statement handles */ ///< Active statement handles.
    char databaseUsername[DB_USERNAME_LEN];  /* username for accessing the db */ ///< Database username.
    char databasePassword[DB_PASSWORD_LEN];  /* password for accessing the db */ ///< Database password.
    int         databaseType;     /* DB type, DB_TYPE_POSTGRES, etc */ ///< Database type identifier.
    char        database_plugin_type[ DB_TYPENAME_LEN ]; ///< Database plugin type name.
} icatSessionStruct;


#endif /* ICAT_STRUCTS_H */

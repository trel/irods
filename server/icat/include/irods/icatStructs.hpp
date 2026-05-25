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
    void*   stmtPtr; ///< Internal database statement handle.
    int     numOfCols; ///< Number of result columns.
    char    *resultColName[MAX_NUM_OF_SELECT_ITEMS]; ///< Result column names.
    int     selectColIds[MAX_NUM_OF_SELECT_ITEMS]; ///< Column identifiers for the select list.
    char    *resultValue[MAX_NUM_OF_SELECT_ITEMS]; ///< Result value pointers.
} icatStmtStrct;



/// \brief Describes an ICAT database session and its active statements.
typedef struct {
    int         status; ///< Session status.
    void*       environPtr; ///< Internal database environment handle.
    void*       connectPtr; ///< Internal database connection handle.
    icatStmtStrct* stmtPtr[MAX_NUM_OF_CONCURRENT_STMTS]; ///< Active statement handles.
    char databaseUsername[DB_USERNAME_LEN]; ///< Database username.
    char databasePassword[DB_PASSWORD_LEN]; ///< Database password.
    int         databaseType; ///< Database type identifier.
    char        database_plugin_type[ DB_TYPENAME_LEN ]; ///< Database plugin type name.
} icatSessionStruct;


#endif /* ICAT_STRUCTS_H */

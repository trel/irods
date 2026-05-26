#ifndef EXEC_CMD_H__
#define EXEC_CMD_H__

/// \file
/// \brief Declares the remote command execution API.

// Remove in iRODS 6.0
#if __has_include("irods/rodsConnect.h")
#  include "irods/rodsConnect.h"
#endif

#include "irods/procApiRequest.h"
#include "irods/dataObjInpOut.h"

/// Input describing a command to execute on an iRODS server.
typedef struct ExecCmd {
    char cmd[LONG_NAME_LEN];            ///< Command name to execute.
    char cmdArgv[HUGE_NAME_LEN];        ///< Raw argument string passed to the command.
    char execAddr[LONG_NAME_LEN];       ///< Explicit host address for command execution.
    char hintPath[MAX_NAME_LEN];        ///< Path hint used to resolve the execution host.
    int addPathToArgv;                  ///< Adds the resolved physical path to the argument string when set.
    int dummy;                          ///< Padding for 64-bit alignment.
    keyValPair_t condInput;             ///< Additional keyword options.
} execCmd_t;

/// Output produced by a remote command execution request.
typedef struct ExecCmdOut {
    bytesBuf_t stdoutBuf;               ///< Captured standard output.
    bytesBuf_t stderrBuf;               ///< Captured standard error.
    int status;                        ///< Exit status of the executed command.
} execCmdOut_t;

/// Packing instruction for `execCmd_t`.
#define ExecCmd_PI "str cmd[LONG_NAME_LEN]; str cmdArgv[HUGE_NAME_LEN]; str execAddr[LONG_NAME_LEN]; str hintPath[MAX_NAME_LEN]; int addPathToArgv; int dummy; struct KeyValPair_PI;"
/// Packing instruction for `execCmdOut_t`.
#define ExecCmdOut_PI "struct BinBytesBuf_PI; struct BinBytesBuf_PI; int status;"

/// Executes a command remotely and returns captured output buffers.
#ifdef __cplusplus
extern "C"
#endif
int rcExecCmd( rcComm_t *conn, execCmd_t *execCmdInp, execCmdOut_t **execCmdOut );

#endif

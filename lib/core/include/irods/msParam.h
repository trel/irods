#ifndef IRODS_MS_PARAM_H
#define IRODS_MS_PARAM_H

#include "irods/rods.h"
#include "irods/objInfo.h"
#include "irods/dataObjCopy.h"
#include "irods/structFileExtAndReg.h"
#include "irods/execCmd.h"
#include "irods/rodsPath.h"

/// Packing instruction name for string microservice parameters.
#define STR_MS_T                "STR_PI"
/// Packing instruction name for integer microservice parameters.
#define INT_MS_T                "INT_PI"
/// Packing instruction name for 16-bit integer microservice parameters.
#define INT16_MS_T              "INT16_PI"
/// Packing instruction name for character microservice parameters.
#define CHAR_MS_T               "CHAR_PI"
/// Packing instruction name for buffer-length microservice parameters.
#define BUF_LEN_MS_T            "BUF_LEN_PI"
/// Packing instruction name for stream handle microservice parameters.
#define STREAM_MS_T             "INT_PI"
/// Packing instruction name for double microservice parameters.
#define DOUBLE_MS_T             "DOUBLE_PI"
/// Packing instruction name for float microservice parameters.
#define FLOAT_MS_T              "FLOAT_PI"
/// Packing instruction name for boolean microservice parameters.
#define BOOL_MS_T               "BOOL_PI"
/// Packing instruction name for `dataObjInp_t` parameters.
#define DataObjInp_MS_T         "DataObjInp_PI"
/// Packing instruction name for data-object close parameters.
#define DataObjCloseInp_MS_T    "DataObjCloseInp_PI"
/// Packing instruction name for `dataObjCopyInp_t` parameters.
#define DataObjCopyInp_MS_T     "DataObjCopyInp_PI"
/// Packing instruction name for data-object read parameters.
#define DataObjReadInp_MS_T     "dataObjReadInp_PI"
/// Packing instruction name for data-object write parameters.
#define DataObjWriteInp_MS_T    "dataObjWriteInp_PI"
/// Packing instruction name for data-object seek input parameters.
#define DataObjLseekInp_MS_T    "fileLseekInp_PI"
/// Packing instruction name for data-object seek output parameters.
#define DataObjLseekOut_MS_T    "fileLseekOut_PI"
/// Packing instruction name for `keyValPair_t` parameters.
#define KeyValPair_MS_T         "KeyValPair_PI"
/// Packing instruction name for tag structure parameters.
#define TagStruct_MS_T          "TagStruct_PI"
/// Packing instruction name for `collInp_t` parameters.
#define CollInp_MS_T            "CollInpNew_PI"
/// Packing instruction name for `execCmd_t` parameters.
#define ExecCmd_MS_T            "ExecCmd_PI"
/// Packing instruction name for `execCmdOut_t` parameters.
#define ExecCmdOut_MS_T         "ExecCmdOut_PI"
/// Packing instruction name for `rodsObjStat_t` parameters.
#define RodsObjStat_MS_T        "RodsObjStat_PI"
/// Packing instruction name for vault path policy parameters.
#define VaultPathPolicy_MS_T    "VaultPathPolicy_PI"
/// Packing instruction name for string array parameters.
#define StrArray_MS_T           "StrArray_PI"
/// Packing instruction name for integer array parameters.
#define IntArray_MS_T           "IntArray_PI"
/// Packing instruction name for `genQueryInp_t` parameters.
#define GenQueryInp_MS_T        "GenQueryInp_PI"
/// Packing instruction name for `genQueryOut_t` parameters.
#define GenQueryOut_MS_T        "GenQueryOut_PI"
/// Packing instruction name for `structFileExtAndRegInp_t` parameters.
#define StructFileExtAndRegInp_MS_T         "StructFileExtAndRegInp_PI"
/// Packing instruction name for rule set parameters.
#define RuleSet_MS_T            "RuleSet_PI"
/// Packing instruction name for rule structure parameters.
#define RuleStruct_MS_T         "RuleStruct_PI"
/// Packing instruction name for data variable mapping parameters.
#define DVMapStruct_MS_T        "DVMapStruct_PI"
/// Packing instruction name for function name mapping parameters.
#define FNMapStruct_MS_T        "FNMapStruct_PI"
/// Packing instruction name for microservice definition parameters.
#define MsrvcStruct_MS_T        "MsrvcStruct_PI"
/// Packing instruction name for netCDF open parameters.
#define NcOpenInp_MS_T          "NcOpenInp_PI"
/// Packing instruction name for netCDF inquiry-by-id parameters.
#define NcInqIdInp_MS_T         "NcInqIdInp_PI"
/// Packing instruction name for netCDF inquiry-by-id results.
#define NcInqWithIdOut_MS_T     "NcInqWithIdOut_PI"
/// Packing instruction name for netCDF inquiry parameters.
#define NcInqInp_MS_T           "NcInqInp_PI"
/// Packing instruction name for netCDF inquiry results.
#define NcInqOut_MS_T           "NcInqOut_PI"
/// Packing instruction name for netCDF close parameters.
#define NcCloseInp_MS_T         "NcCloseInp_PI"
/// Packing instruction name for netCDF get-variable parameters.
#define NcGetVarInp_MS_T        "NcGetVarInp_PI"
/// Packing instruction name for netCDF get-variable results.
#define NcGetVarOut_MS_T        "NcGetVarOut_PI"
/// Packing instruction name for netCDF group inquiry results.
#define NcInqGrpsOut_MS_T       "NcInqGrpsOut_PI"
/// Packing instruction name for dictionary parameters.
#define Dictionary_MS_T         "Dictionary_PI"
/// Packing instruction name for dictionary array parameters.
#define DictArray_MS_T          "DictArray_PI"
/// Packing instruction name for generic array parameters.
#define GenArray_MS_T           "GenArray_PI"
/// Packing instruction name for `dataObjInfo_t` parameters.
#define DataObjInfo_MS_T        "DataObjInfo_PI"

/// Describes a single microservice parameter.
typedef struct MsParam {
    char *label;           /**< Parameter label. */
    char *type;            /**< Packing instruction name from `rodsPackTable.h`. */
    void *inOutStruct;     /**< Structured value for the parameter. */
    bytesBuf_t *inpOutBuf; /**< Buffer value for the parameter. */
} msParam_t;

/// Holds a collection of microservice parameters.
typedef struct MsParamArray {
    int len;             /**< Number of parameters in the array. */
    int oprType;         /**< Operation type associated with the array. */
    msParam_t **msParam; /**< Array of parameter pointers. */
} msParamArray_t;

/// Separator string used inside serialized microservice input.
#define MS_INP_SEP_STR    "++++"
/// String representing a null microservice input.
#define MS_NULL_STR       "null"
/// Parser state for key-value strings used by microservices.
typedef struct ParsedMsKeyValStr {
    char *inpStr; /**< Input string being parsed. */
    char *endPtr; /**< Pointer to the end of the input string. */
    char *curPtr; /**< Current parse position. */
    char *kwPtr;  /**< Current keyword pointer. */
    char *valPtr; /**< Current value pointer. */
} parsedMsKeyValStr_t;

/// Maps a keyword flag to its string form.
typedef struct ValidKeyWd {
    int flag;    /**< Bit flag associated with the keyword. */
    char *keyWd; /**< Keyword string. */
} validKeyWd_t;

/// Accepts the resource name keyword.
#define RESC_NAME_FLAG          0x1
/// Accepts the destination resource name keyword.
#define DEST_RESC_NAME_FLAG     0x2
// #define BACKUP_RESC_NAME_FLAG   0x4 // Removed in iRODS 5.0
/// Accepts the force operation keyword.
#define FORCE_FLAG_FLAG         0x8
/// Accepts all keywords.
#define ALL_FLAG                0x10
/// Accepts the local path keyword.
#define LOCAL_PATH_FLAG         0x20
/// Accepts the verify checksum keyword.
#define VERIFY_CHKSUM_FLAG      0x40
/// Accepts the admin keyword.
#define ADMIN_FLAG              0x80
/// Accepts the update replica keyword.
#define UPDATE_REPL_FLAG        0x100
/// Accepts the replica number keyword.
#define REPL_NUM_FLAG           0x200
/// Accepts the data type keyword.
#define DATA_TYPE_FLAG          0x400
/// Accepts the checksum all keyword.
#define CHKSUM_ALL_FLAG         0x800
/// Accepts the force checksum keyword.
#define FORCE_CHKSUM_FLAG       0x1000
/// Accepts the file path keyword.
#define FILE_PATH_FLAG          0x2000
/// Accepts the create mode keyword.
#define CREATE_MODE_FLAG        0x4000
/// Accepts the open flags keyword.
#define OPEN_FLAGS_FLAG         0x8000
/// Accepts collection flags.
#define COLL_FLAGS_FLAG         0x10000
/// Accepts the data size keyword.
#define DATA_SIZE_FLAGS         0x20000
/// Accepts the thread count keyword.
#define NUM_THREADS_FLAG        0x40000
/// Accepts the operation type keyword.
#define OPR_TYPE_FLAG           0x80000
/// Accepts the object path keyword.
#define OBJ_PATH_FLAG           0x100000
/// Accepts the collection name keyword.
#define COLL_NAME_FLAG          0x200000
/// Accepts the remove-to-trash keyword.
#define RMTRASH_FLAG            0x400000
/// Accepts the admin remove-to-trash keyword.
#define ADMIN_RMTRASH_FLAG      0x800000
/// Accepts the default resource name keyword.
#define DEF_RESC_NAME_FLAG      0x1000000
/// Accepts the bulk operation keyword.
#define BULK_OPR_FLAG           0x10000000
/// Accepts the unregister keyword.
#define UNREG_FLAG              0x20000000

#ifdef __cplusplus
extern "C" {
#endif

/// Resets a microservice parameter to its default state.
int
resetMsParam( msParam_t *msParam );

/// Deallocates memory referenced by members of a MsParam.
///
/// This function does not deallocate memory referenced by KeyValPairs.
///
/// \param[in] msParam    The pointer to a MsParam to deallocate.
/// \param[in] freeStruct Indicates whether the memory pointed to by MsParam::inOutStruct should be deallocated.
///
/// \returns An integer indicating the status of the operation.
/// \retval 0        On success.
/// \retval non-zero On failure.
int clearMsParam(msParam_t* msParam, int freeStruct);

/// Deallocates memory referenced by members of a MsParam.
///
/// Unlike clearMsParam(MsParam*, int), this function deallocates memory referenced by all data members,
/// including KeyValPairs.
///
/// \param[in] msParam The pointer to a MsParam to deallocate.
///
/// \returns An integer indicating the status of the operation.
/// \retval 0        On success.
/// \retval non-zero On failure.
///
/// \since 4.3.5
int clearMsParamFull(msParam_t* _msParam);

/// Appends a parameter to a microservice parameter array.
int addMsParam(msParamArray_t* msParamArray,
               const char* label,
               const char* packInstruct,
               void* inOutStruct,
               bytesBuf_t* inpOutBuf);
/// Adds an integer parameter to a microservice parameter array.
int
addIntParamToArray( msParamArray_t *msParamArray, char *label, int inpInt );
/// Adds or replaces a parameter in a microservice parameter array.
int
addMsParamToArray( msParamArray_t *msParamArray, const char *label,
                   const char *type, void *inOutStruct, bytesBuf_t *inpOutBuf, int replFlag );
/// Replaces the contents of one parameter array with another.
int
replMsParamArray( msParamArray_t *msParamArray,
                  msParamArray_t *outMsParamArray );
/// Replaces one microservice parameter with another.
int
replMsParam( msParam_t *msParam, msParam_t *outMsParam );
/// Replaces a structured parameter value.
int
replInOutStruct( void *inStruct, void **outStruct, const char *type );
/// Clones a byte buffer.
bytesBuf_t*
replBytesBuf( const bytesBuf_t* in);
/// Fills a microservice parameter with the provided values.
int
fillMsParam( msParam_t *msParam, const char *label,
              const char *type, void *inOutStruct, bytesBuf_t *inpOutBuf );
/// Returns the first parameter matching `label`.
msParam_t *
getMsParamByLabel( msParamArray_t *msParamArray, const char *label );
/// Returns the first parameter matching `type`.
msParam_t *
getMsParamByType( msParamArray_t *msParamArray, const char *type );
/// Removes a parameter by label.
int
rmMsParamByLabel( msParamArray_t *msParamArray, const char *label, int freeStruct );

/// Removes parameters not listed in `outParamDesc` or lacking required metadata.
int trimMsParamArray(msParamArray_t* msParamArray, char* outParamDesc);

/// Prints a parameter array.
int
printMsParam( msParamArray_t *msParamArray );
/// Serializes a parameter into `buf`.
int
writeMsParam( char *buf, int len, msParam_t *msParam );

/// Deallocates memory referenced by members of a MsParamArray.
///
/// This function does not deallocate memory referenced by KeyValPairs.
///
/// \param[in] msParamArray The pointer to a MsParamArray to deallocate.
/// \param[in] freeStruct   Indicates whether the memory pointed to by MsParam::inOutStruct should be deallocated.
///
/// \returns An integer indicating the status of the operation.
/// \retval 0        On success.
/// \retval non-zero On failure.
int clearMsParamArray(msParamArray_t* msParamArray, int freeStruct);

/// Deallocates memory referenced by members of a MsParamArray.
///
/// Unlike clearMsParamArray(MsParamArray*, int), this function deallocates memory referenced by all data members,
/// including KeyValPairs.
///
/// \param[in] msParamArray The pointer to a MsParamArray to deallocate.
///
/// \returns An integer indicating the status of the operation.
/// \retval 0        On success.
/// \retval non-zero On failure.
///
/// \since 4.3.5
int clearMsParamArrayFull(msParamArray_t* _msParamArray);

/// Stores an integer value in a microservice parameter.
void
fillIntInMsParam( msParam_t *msParam, const int myInt );
/// Stores a float value in a microservice parameter.
void
fillFloatInMsParam( msParam_t *msParam, const float myFloat );
/// Stores a character value in a microservice parameter.
void
fillCharInMsParam( msParam_t *msParam, const char myChar );
/// Stores a double-compatible numeric value in a microservice parameter.
void
fillDoubleInMsParam( msParam_t *msParam, const rodsLong_t myDouble );
/// Stores a string value in a microservice parameter.
void
fillStrInMsParam( msParam_t *msParam, const char *myStr );
/// Stores a buffer length and buffer value in a microservice parameter.
void
fillBufLenInMsParam( msParam_t *msParam, int myInt, bytesBuf_t *bytesBuf );
/// Parses a microservice parameter as `dataObjInp_t`.
int
parseMspForDataObjInp( msParam_t *inpParam, dataObjInp_t *dataObjInpCache,
                        dataObjInp_t **outDataObjInp, int writeToCache );
/// Parses a microservice parameter as `collInp_t`.
int
parseMspForCollInp( msParam_t *inpParam, collInp_t *collInpCache,
                     collInp_t **outCollInp, int writeToCache );
/// Parses a condition input keyword and value from a microservice parameter.
int
parseMspForCondInp( msParam_t *inpParam, keyValPair_t *condInput,
                     char *condKw );
/// Parses condition keywords from a microservice parameter.
int
parseMspForCondKw( msParam_t *inpParam, keyValPair_t *condInput );
/// Parses physical path registration keywords from a microservice parameter.
int
parseMspForPhyPathReg( msParam_t *inpParam, keyValPair_t *condInput );
/// Parses a positive integer from a microservice parameter.
int
parseMspForPosInt( msParam_t *inpParam );
/// Converts a microservice parameter into a string representation.
int msp_to_string(msParam_t* _inp_param, char** _out_param);
/// Parses a string from a microservice parameter.
char *
parseMspForStr( msParam_t *inpParam );
/// Parses a float from a microservice parameter.
int
parseMspForFloat( msParam_t *inpParam, float *floatout );
/// Parses a double from a microservice parameter.
int
parseMspForDouble( msParam_t *inpParam, double *doubleout );
/// Parses a microservice parameter as `dataObjCopyInp_t`.
int
parseMspForDataObjCopyInp( msParam_t *inpParam,
                            dataObjCopyInp_t *dataObjCopyInpCache, dataObjCopyInp_t **outDataObjCopyInp );
/// Parses a microservice parameter as `execCmd_t`.
int
parseMspForExecCmdInp( msParam_t *inpParam,
                        execCmd_t *execCmdInpCache, execCmd_t **ouExecCmdInp );
/// Returns the structured value for the first parameter matching `label`.
void
*getMspInOutStructByLabel( msParamArray_t *msParamArray, const char *label );
/// Extracts standard output text from an `ExecCmdOut` parameter.
int
getStdoutInExecCmdOut( msParam_t *inpExecCmdOut, char **outStr );
/// Extracts standard error text from an `ExecCmdOut` parameter.
int
getStderrInExecCmdOut( msParam_t *inpExecCmdOut, char **outStr );
/// Initializes a parsed key-value string state object.
int
initParsedMsKeyValStr( char *inpStr, parsedMsKeyValStr_t *parsedMsKeyValStr );
/// Clears a parsed key-value string state object.
int
clearParsedMsKeyValStr( parsedMsKeyValStr_t *parsedMsKeyValStr );
/// Advances to the next key-value pair in a parsed key-value string.
int
getNextKeyValFromMsKeyValStr( parsedMsKeyValStr_t *parsedMsKeyValStr );
/// Parses key-value input into `dataObjInp_t`.
int
parseMsKeyValStrForDataObjInp( msParam_t *inpParam, dataObjInp_t *dataObjInp,
                                char *hintForMissingKw, int validKwFlags, char **outBadKeyWd );
/// Checks whether a data object keyword is allowed by `validKwFlags`.
int
chkDataObjInpKw( char *keyWd, int validKwFlags );
/// Parses key-value input into `collInp_t`.
int
parseMsKeyValStrForCollInp( msParam_t *inpParam, collInp_t *collInp,
                             char *hintForMissingKw, int validKwFlags, char **outBadKeyWd );
/// Checks whether a collection keyword is allowed by `validKwFlags`.
int
chkCollInpKw( char *keyWd, int validKwFlags );
/// Adds a key-value pair to a serialized microservice key-value string.
int
addKeyValToMspStr( msParam_t *keyStr, msParam_t *valStr,
                    msParam_t *msKeyValStr );
/// Checks whether a struct file registration keyword is allowed.
int
chkStructFileExtAndRegInpKw( char *keyWd, int validKwFlags );
/// Parses key-value input into `structFileExtAndRegInp_t`.
int
parseMsKeyValStrForStructFileExtAndRegInp( msParam_t *inpParam,
         structFileExtAndRegInp_t *structFileExtAndRegInp,
         char *hintForMissingKw, int validKwFlags, char **outBadKeyWd );
/// Parses microservice parameters from an IR file buffer.
int
parseMsParamFromIRFile( msParamArray_t *inpParamArray, char *inBuf );

/// Deallocates memory which contains the type information for a microservice parameter.
///
/// \param[in] _msp The microservice parameter to check.
///
/// \since 4.2.12
void msp_free_type(struct MsParam* _msp);

/// Deallocates memory which contains the value information for a microservice parameter.
///
/// This function requires that the type information be valid, otherwise this function will result in a no-op.
///
/// \param[in] _msp The microservice parameter to check.
///
/// \since 4.2.12
void msp_free_inOutStruct(struct MsParam* _msp);

#ifdef __cplusplus
}
#endif

#endif // IRODS_MS_PARAM_H

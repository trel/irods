#!/bin/bash

SCRIPTDIR=$(dirname $0)
SCRIPTNAME=$(basename $0)
# check arguments
if [ $# -ne 0 ] ; then
  echo "Usage: $SCRIPTNAME"
  exit 1
fi

# prep
D=`date +%Y%m%dT%H%M%S`
COVERAGE_TMPDIR="/tmp/irods-coverage/${D}"
echo "COVERAGE_TMPDIR [${COVERAGE_TMPDIR}]"
su irods -c "mkdir -p ${COVERAGE_TMPDIR}"

# start server
echo "----- STARTING iRODS SERVER -----"
PROFILE_RAW_PATTERN="${COVERAGE_TMPDIR}/raw/profile-%p-%m.profraw"
su irods -c "LLVM_PROFILE_FILE=${PROFILE_RAW_PATTERN} /usr/sbin/irodsServer -d"
echo "PID is [$(cat /var/run/irods/irods-server.pid)]"
until su irods -c 'LLVM_PROFILE_FILE=${PROFILE_RAW_PATTERN} ils > /dev/null 2>&1' ; do
  echo -n "."
  sleep .2
done

# run test suite
echo "----- RUNNING TEST SUITE -----"
su irods -c "LLVM_PROFILE_FILE=${PROFILE_RAW_PATTERN} ils"

# stopping server
echo "----- STOPPING iRODS SERVER -----"
kill $(cat /var/run/irods/irods-server.pid)

# merge raw profile data
echo "----- GENERATING COVERAGE REPORT -----"
PROFILE_DATAFILE="${COVERAGE_TMPDIR}/irods.profdata"
~/externals/clang16.0.6-0_src/build/bin/llvm-profdata merge -sparse ${COVERAGE_TMPDIR}/raw/*.profraw -o ${PROFILE_DATAFILE}

# generate coverage report
COVERAGE_OUTPUT_DIR="${COVERAGE_TMPDIR}/report"
SERVER_BINARY_DIR=${SCRIPTDIR}/../server
~/externals/clang16.0.6-0_src/build/bin/llvm-cov show \
    -instr-profile=${PROFILE_DATAFILE} \
    --ignore-filename-regex='.*/irods-externals/.*' \
    -format=html -output-dir=${COVERAGE_OUTPUT_DIR} \
    -object=${SERVER_BINARY_DIR}/main_server/irodsAgent \
    -object=${SERVER_BINARY_DIR}/delay_server/irodsDelayServer \
    ${SERVER_BINARY_DIR}/main_server/irodsServer
chmod -R 755 ${COVERAGE_OUTPUT_DIR}

echo "${COVERAGE_OUTPUT_DIR}/"

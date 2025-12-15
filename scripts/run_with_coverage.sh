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

# run unit tests
echo "----- RUNNING UNIT TESTS -----"
for X in /var/lib/irods/unit_tests/irods_*; do
    echo "Running... ${X}"
    su irods -c "LLVM_PROFILE_FILE=${PROFILE_RAW_PATTERN} ${X}"
done

# stopping server
echo "----- STOPPING iRODS SERVER -----"
kill $(cat /var/run/irods/irods-server.pid)
sleep 1

# running via run_tests.py
echo "----- RUNNING TEST SUITE -----"
su - irods -c "LLVM_PROFILE_FILE=${PROFILE_RAW_PATTERN} \
    python3 ~/scripts/run_tests.py --run_python_suite"
#    python3 ~/scripts/run_tests.py --run_s test_ils"

# stopping server
echo "----- STOPPING iRODS SERVER -----"
kill $(cat /var/run/irods/irods-server.pid)
sleep 1

# merge raw profile data
echo "----- GENERATING COVERAGE REPORT -----"
PROFILE_DATAFILE="${COVERAGE_TMPDIR}/irods.profdata"
CLANG_BIN_DIR=/opt/irods-externals/clang16.0.6-0/bin
${CLANG_BIN_DIR}/llvm-profdata merge \
    -sparse ${COVERAGE_TMPDIR}/raw/*.profraw \
    -o ${PROFILE_DATAFILE}

# generate coverage report
COVERAGE_OUTPUT_DIR="${COVERAGE_TMPDIR}/report"
${CLANG_BIN_DIR}/llvm-cov show \
    -instr-profile=${PROFILE_DATAFILE} \
    --ignore-filename-regex='.*/irods-externals/.*' \
    -format=html -output-dir=${COVERAGE_OUTPUT_DIR} \
    -object=/usr/sbin/irodsAgent \
    -object=/usr/sbin/irodsDelayServer \
    /usr/sbin/irodsServer
chmod -R 755 ${COVERAGE_OUTPUT_DIR}

# build stable link
COVERAGE_LINK=/tmp/irods-latest-report
rm -f ${COVERAGE_LINK}
ln -s ${COVERAGE_OUTPUT_DIR} ${COVERAGE_LINK}

echo "${COVERAGE_OUTPUT_DIR}/"
echo "rm -rf /var/www/coverage && docker cp ubcov:${COVERAGE_LINK}/ /var/www/coverage/"

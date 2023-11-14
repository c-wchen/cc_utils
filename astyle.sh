#!/bin/bash

CURRENT_PATH=$(pwd)
SCAN_PATH=${CURRENT_PATH}/src,${CURRENT_PATH}/testcase
SCAN_SUFFIX=*.c,*.h,*.cpp
ASTYLE_BIN=${CURRENT_PATH}/third_party/astyle/_install/bin/astyle


# 内部功能
SCAN_SUFFIX_ARR=(${SCAN_SUFFIX//,/ })
SCAN_PATH_ARR=(${SCAN_PATH//,/ })
SCAN_SUFFIX_ARR_SIZE=${#SCAN_SUFFIX_ARR[@]}


for((i=0;i<${SCAN_SUFFIX_ARR_SIZE};i++))
do
    FIND_COND="${FIND_COND} -name ${SCAN_SUFFIX_ARR[$i]}"
    if [ $i -eq `expr $SCAN_SUFFIX_ARR_SIZE - 1` ]
    then
        continue
    else
        FIND_COND="$FIND_COND -or "
    fi
done

echo "$FIND_COND"


for path in ${SCAN_PATH_ARR[@]}
do
    found=$(find ${path} -maxdepth 1 ${FIND_COND})
    ${ASTYLE_BIN} --style=kr -n $found
done
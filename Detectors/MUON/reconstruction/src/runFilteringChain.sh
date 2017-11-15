#!/bin/bash

if [ -z $1 ]; then
    echo "Gimme the damn ALISOFT!"
    exit 1;
fi

if [ -z $2 ]; then
    echo "Gimme the damn [DY]LD_LIBRARY_PATH!"
    exit 2;
fi

export _LD_LIBRARY_PATH=$2
export _DYLD_LIBRARY_PATH=${_LD_LIBRARY_PATH}

ALISOFT=$1
ALICE_WORK_DIR=${ALISOFT}"/sw"
RUNDIR="$ALISOFT/test_device_data"
JSONDIR="$O2_ROOT/bin/config"

EXECUTABLES=("runMIDFilter" "runMIDMaskGenerator" "runMIDRatesComputer" "runBroadcaster")

originalindices=${!EXECUTABLES[*]}

for index in ${originalindices};
do
    IDS[$index]=`echo ${EXECUTABLES[$index]} | sed 's/run//g'`
    JSONS[$index]=`echo "$O2_ROOT/bin/config/"${EXECUTABLES[$index]}".json"`
    echo ${JSONS[$index]}
done

HOST=localhost

OPTIONS=(
    "--binmapfile 'binmapfile_MID.dat'"
    "--binmapfile 'binmapfile_MID.dat'"
    "--binmapfile 'binmapfile_MID.dat'"
    ""
    )

#GENERATOR_EXECUTABLE="aliceHLTWrapperApp 'DigitReader' 1 -x --output type=push,size=10,method=bind,address=tcp://*:22777 --library libdhlt.dylib --component MUONDigitReader --parameter '-datafile digits.root'"

GENERATOR_EXECUTABLE="AliceHLTWrapperDevice 'DigitReader' --id 1 --channel-config 'name=data-out,type=push,size=10,method=bind,address=tcp://*:22777' --library libdhlt --component MUONDigitReader --parameter '-datafile digits.root' --run 169099 -m 2"

echo "Following commands will be executed:"
echo ${GENERATOR_EXECUTABLE}

for index in ${originalindices};
do

    EXPORT_COMMAND='export PATH='${PATH}'; export LD_LIBRARY_PATH='${_LD_LIBRARY_PATH}'; export DYLD_LIBRARY_PATH='${_DYLD_LIBRARY_PATH}
    CD_COMMAND='cd ~/alice_sw/test_device_data'
    RUN_COMMAND=${EXECUTABLES[$index]}" --id '"${IDS[$index]}"' --mq-config '"${JSONS[$index]}"' "${OPTIONS[$index]}

    echo "$RUN_COMMAND"

    screen -dmS ${EXECUTABLES[$index]} bash -c "$EXPORT_COMMAND; $CD_COMMAND; $RUN_COMMAND"
done

read -p "Press any key to kill all screens..."
for index in ${originalindices};
do
    killall ${EXECUTABLES[$index]}
done
screen -ls | grep Detached | cut -d. -f1 | awk '{print $1}' | xargs kill
wait 1
for index in ${originalindices};
do
    killall ${EXECUTABLES[$index]}
done

exit 0

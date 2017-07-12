#!/usr/bin/env bash

ALISOFT="/Users/Gabriele/alice_sw"
O2_ROOT="/Users/Gabriele/alice_sw/sw/osx_x86-64/O2/O2-dev-alo-1"

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

GENERATOR_EXECUTABLE="aliceHLTWrapperApp 'DigitReader' 1 -x --output type=push,size=10,method=bind,address=tcp://*:22777 --library libdhlt.dylib --component MUONDigitReader --parameter '-datafile digits.root'"

echo "Following commands will be executed:"
echo ${GENERATOR_EXECUTABLE}
for index in ${originalindices};
do
    screen -D -RR ${EXECUTABLES[$index]} -X quit || true
#    screen -dmS ${EXECUTABLES[$index]} bash -c "echo bravo;$SHELL";
    screen -dmS ${EXECUTABLES[$index]} bash -c '$SHELL'
    screen -S ${EXECUTABLES[$index]} -p 0 -X stuff $'eval \"`~/Library/Python/2.7/bin/alienv -w $ALICE_WORK_DIR load O2/latest,alo/latest,AliRoot-OCDB/latest`\"\n'
    sleep 1

    COMMAND=${EXECUTABLES[$index]}" --id '"${IDS[$index]}"' --mq-config '"${JSONS[$index]}"' "${OPTIONS[$index]}
    echo ${COMMAND}

    screen -S ${EXECUTABLES[$index]} -p 0 -X stuff "cd $RUNDIR"
    screen -S ${EXECUTABLES[$index]} -p 0 -X stuff $'\n'
    screen -S ${EXECUTABLES[$index]} -p 0 -X stuff "${COMMAND}"
    screen -S ${EXECUTABLES[$index]} -p 0 -X stuff $'\n'

#    eval "`screen -S ${EXECUTABLES[$index]} -p 0 -X stuff ${COMMAND}`"

done

#read -p "Press any key to kill all screens..."
#for index in ${originalindices};
#do
#    screen -S ${EXECUTABLES[$index]} -p 0 -X stuff "^C"
#done
#screen -ls | grep Detached | cut -d. -f1 | awk '{print $1}' | xargs kill

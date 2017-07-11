#!/usr/bin/env bash
#Just a basic RC to initialize the terminals in the right way to run O2 devices

export TEST_DIR="$HOME/alice_sw/test_device_data/"

function loadO2-dev ()
{
    echo "";
    cat $HOME/.O2logo.txt;
    eval "`AliEn load O2/latest,alo/latest,AliRoot-OCDB/latest`";
    export ALIHLT_HCDBDIR="$ALIROOT_OCDB_ROOT/OCDB";
    LOADEDALIENPACKAGE="O2/latest,alo/latest,AliRoot-OCDB/latest";
    AliEn list
}

function cdO2 {
    cd $TEST_DIR
}

#unset TEST_DIR

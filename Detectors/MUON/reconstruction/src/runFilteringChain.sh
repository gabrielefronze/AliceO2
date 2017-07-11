#!/usr/bin/env bash

#TODO: here goes the array of executables to run
#TODO: and here the array of settings and options


#TODO: this is simply a wrapper to run another terminal window. It is a prototype to be used in a loop over exes!
osascript -e 'tell application "Terminal" to do script "cd \"'`pwd`'\";source ./FilteringRC.sh;cd \"$TEST_DIR\";loadO2-dev"'
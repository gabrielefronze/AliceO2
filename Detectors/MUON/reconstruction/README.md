# Running MID clustering
## Preparatory steps (once)
First generate the mapping for the MID following the instructions at [dhlt](https://github.com/mrrtf/alo/tree/master/dhlt) readme (Preparatory steps).

To perform a lightweight mapping use:

```
dhlt-generate-binmapfile -cdbpath local://$ALIROOT_OCDB_ROOT/OCDB -run 0 -binmapfile binmapfile.dat -first 10
```

Get a digit file. An example file was produced by [Laurent](https://github.com/aphecetche/AliceO2). You can download it following the doc [here](https://github.com/aphecetche/AliceO2/tree/hltwrapper/Detectors/MUON/hltwrapper). Following the instructions you will produce the file: merged.digits.MB.196099.root

## Execution
### Start digits reader
In one terminal launch:
```bash
aliceHLTWrapperApp 'DigitReader' 1 -x --output type=push,size=10,method=bind,address=tcp://*:45000 --library libdhlt.dylib --component MUONDigitReader --parameter '-datafile merged.digits.MB.196099.root'
```

### Start clusterizer
In another terminal:
```bash
runMIDclustering --id 'MIDclustering' --binmapfile 'binmapfile.dat' --source 'tcp://localhost:45000' --mq-config "$O2_ROOT/bin/config/runMIDclustering.json"
```

# Running filtering chain
## Devices description and data flow
The Digits filtering chain performs the detection of dead and noisy strips online.
After the detection of the problematic strips their digits are purged from the data that will be forwarded to the reconstruction devices (e.g. the above cited MIDclustering).
The following diagram represents the data flows and the devices used to perform the filtering.

<center><img src="https://github.com/gabrielefronze/AliceO2/blob/my-dev-occupancy/Detectors/MUON/MIDFiltering_diagram.svg"></center>

Here some details on the devices:

* The digits generator devices still has to be defined. For testing purposes `aliHLTWrapper` works well as described in the above section.
* The `Broadcaster` device sends copies of the same received message to all the connected devices. Is a generic device that will likely be implemented in FairROOT generic software.
* The `MIDRatesComputer` device contains a data member capable of storing the number of counts per strip as well as some information about the strip itself. It records the number of times each strip has been fired. Each time enough statistics has been acquired the rates are sent to the following device.
* The `MIDMaskGenerator` device receives the "scaler counts" per strip and performs the algorithm to detects noisy and dead strips. The algorithm output is stored in a mask object streamed towards the following device.
* The `MIDFilter` device connects to the digits source, receives and stores a mask, and purges the noisy strips from the incoming message before forwarding the purged message to the following devices.

## Preparatory steps (once)
Follow the same paragraph for the `MIDclustering` device.
All the devices requiring a `binmapfile.dat` or a `digits.root` file have to be executed from a folder containing the needed files.

## Execution (interactive)
Please note that the digits reader has to be started after all the other devices. In this way the beginning of data processing can be controlled. Conceptually the filtering chain will always be kept online, while the data production from the detector would be variable in time.

### Start Broadcaster
In one terminal launch:
```bash
runBroadcaster --id 'Broadcaster' --mq-config '$O2_ROOT/bin/config/runBroadcaster.json'
```

### Start MIDRatesComputer
In another terminal launch:
```bash
runMIDRatesComputer --id 'MIDRatesComputer' --mq-config '$O2_ROOT/bin/config/runMIDRatesComputer.json' --binmapfile 'binmapfile.dat'
```

### Start MIDMaskGenerator
In another terminal launch:
```bash
runMIDMaskGenerator --id 'MIDMaskGenerator' --mq-config '$O2_ROOT/bin/config/runMIDMaskGenerator.json' --binmapfile 'binmapfile.dat'
```

### Start MIDFilter
In one terminal launch:
```bash
runMIDFilter --id 'MIDFilter' --mq-config '$O2_ROOT/bin/config/runMIDFilter.json' --binmapfile 'binmapfile.dat'
```

### Start digits reader
In another terminal launch:
```bash
aliceHLTWrapperApp 'DigitReader' 1 -x --output type=push,size=10,method=bind,address=tcp://*:22777 --library libdhlt.dylib --component MUONDigitReader --parameter '-datafile merged.digits.MB.196099.root'
```

## Execution (automatic)
An automatic `bash` script has been developed to start all the devices in the correct order without human control. Once started the devices can be accessed through screen sessions. The digit reader device still has to be started manually to keep control on the data processing.

### Preparatory steps
Follow the same paragraph for the `MIDclustering` device.
All the devices requiring a `binmapfile.dat` or a `digits.root` file have to be executed from a folder containing the needed files.

The automatic script is called `runFilteringChain.sh <alice_sw_path> <library_path>`.
It takes two arguments to be correctly executed:
* `<alice_sw_path>`: is the path where the ALICE software is installed.
* `<library_path>`: is the library path.

Environment variables corresponding to the two parameters are set by alienv entering the correct O2 environment:
* `<alice_sw_path>` = `$ALISOFT`
* `<library_path>` = `$[DY]LD_LIBRARY_PATH`

The correct `alienv enter(load)` command has to be run prior the script execution.

### Screen sessions naming
A screen session running the device `A` will be called `runA` and always contains a single instance of `runA` application being executed.
The output of `screen -ls` after the script execution will be similar to:
```bash
There are screens on:
	30877.runMIDFilter	(Detached)
	30880.runMIDMaskGenerator	(Detached)
	30883.runMIDRatesComputer	(Detached)
	30886.runBroadcaster	(Detached)
4 Sockets in /var/folders/[...]/.screen.
```

### Run the script
In one terminal execute the automatic script within the correct environment loaded with `alienv`.
```bash
runFilteringChain.sh $ALISOFT $LD_LIBRARY_PATH
```

### Start digits reader
In another terminal launch:
```bash
aliceHLTWrapperApp 'DigitReader' 1 -x --output type=push,size=10,method=bind,address=tcp://*:22777 --library libdhlt.dylib --component MUONDigitReader --parameter '-datafile merged.digits.MB.196099.root'
```

### Connect to screen session
In several other terminal sessions:
```bash
screen -r [sessionName]
```
Choose sessionName based on `screen -ls` output.

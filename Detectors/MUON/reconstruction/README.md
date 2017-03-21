# Running MID clustering

## Preparatory steps (once)
First generate the mapping for the MID. This requires the AliRoot development branch 'feature-muonhlt':
```bash
dhlt-generate-binmapfile -cdbpath "local://$HOME/alicesw/AliRoot/OCDB" -run 0 -binmapfile "binmapfile_MID.dat" -first 10
```

Get a digit file. An example file was produced by [Laurent](https://github.com/aphecetche/AliceO2). You can download it following the doc [here](https://github.com/aphecetche/AliceO2/tree/hltwrapper/Detectors/MUON/hltwrapper). Following the instructions you will produce the file: merged.digits.MB.196099.root

## Execution
### Start digits reader
In one terminal launch:
```bash
aliceHLTWrapperApp 'DigitReader' 1 -x --output type=push,size=10,method=bind,address=tcp://*:45000 --library libAliHLTMUON.so --component MUONDigitReader --parameter '-datafile merged.digits.MB.196099.root'
```

### Start clusterizer
In another terminal:
```bash
runMIDclustering --id 'MIDclustering' --binmapfile 'binmapfile.dat_MID.dat' --source 'tcp://localhost:45000' --mq-config "$O2_ROOT/bin/config/runMIDclustering.json"
```

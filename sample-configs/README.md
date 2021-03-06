Running the Project
=========
After building the project, navigate to `build/src/` and find `parallel-neuro-sim`. You will run `parallel-neuro-sim` following the instructions below for using run-config files or command line options.

## Using Config Files
The easiest way to run the project is to use a config file like [run-configs/example-config.ini](run-configs/example-config.ini). To use a run-config, simply execute:

```
./parallel-neuro-sim --config-file <path to run-config>
```

To use a sample run-config, execute:


```
./parallel-neuro-sim --config-file ../../sample-configs/run-configs/example-config.ini
```

For more details on run-configs, see the comments in [run-configs/example-config.ini](run-configs/example-config.ini).

## Using Command Line Options

Command line options and a run-config file can be used simultaneously. If the same option is specified twice, the value provided as a command line option will be used.

To run the project with minimal command line options, run:

```
./parallel-neuro-sim --input-file <input file> --output-file <output file>
```

Run the following to see a list of all available options:

```
./parallel-neuro-sim --help
```

# A Geant4 template for detector simulation

## Introduction
This repository is a template and a framework for writing Geant4 code for detector simulation. The framework provides some basic functions such as parsing of commandline arguments, messenger classes to set parameters for geometries, GeneratorAction and RunAction. The program also outputs a ROOT file detailing stepwise information of all the particles simulated.

## Installation
The code can be installed following the basic Geant4 application installation procedures. Within the project directory, execute
```
mkdir build
cd build
cmake ..
make
```
Note that one needs ROOT to run this application.

## Running the program
The basic usage of this program is: executable [-option [argument(s)] ]
- -m/--macro,       used to spefify the macro file to execute.
- -u/--interactive, enter interactive session.
- --vis,            enable visualization. (disabled by default)
- --seed,           the random seed to be used. (default current time)
- -o/--output       specify the output file name to which trajectories will be recorded.

Unlike many Geant4 examples, the program will do nothing by default. The user is responsible for specifying a macro to execute, or to enter interactive session. In the interactive mode, *init_vis.mac* will be executed by default.

To accomodate situations when the program is run in a cluster, visualization has to be turned on manually with *--vis* option.

By default, the program uses current time. This behavior can be altered with *--seed* option so that a user-specified random seed is used.

ROOT output is specified with *-o/--output* option. If the file already exists, it WILL NOT be overwritten.

## Output Format

The output ROOT file has three entries:
- macros used
- the random seeds
- a TTree entry called *events* recording all the steps of all particles in the simulation (except neutrinos)

The *events* TTree records the event ID, track ID, step ID, parent ID, particle name, kinetic information (xyz, momentum, energy, time) and the process that defined the step. There are three special *flag* processes:
* initStep: marks the beginning of a new track
* newEvent: marks the beginning of a new event
* timeReset: when a radioactive decay ocurrs, the timescale can exceed float precision. To preserve all information, all radioactive decays are treated as a new sub-event. User can always merge them later in the offline analysis.

The default units are mm for length, ns for time and keV for energy.

## Custom Commands

### Geometry
The current mock-up geometry is a cube target named *target* in the center of the world. The cube has an empty center, and is surrounded by a few farside detectors called *fsN* where N is a number starting from 0.

Available commands include 
```
/target/material materialName
/target/pos x y z unit
/target/rotateX(Y,Z) N unit
```
```
/farside/pos x y z unit
/farside/rotateX(Y,Z) N unit
/farside/set Name （apply the above parameters to farside detector called Name）
```

### Generator Action
```
/generator/spectrum foo.root
```
can be used to sample particle distributions from another ROOT file.

### Filtering
```
/filter/recordWhenHit bar
```
record the entire tracks when there is a hit in detector *bar*.

```
/filter/excludeParticle bar
/filter/excludeProcess bar
/filter/excludeVolume bar
```
excludes certain particles, processes and volumes. When the condition matches, the step is not recorded. In addition to these explicit conditions, neutrinos are by default ignored.

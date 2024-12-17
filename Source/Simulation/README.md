# PSU-CIDD-Malaria-Simulation

[Temple University](https://www.temple.edu/) - [Boni Lab](http://mol.ax/)

---

## Overview

This repository contains the working codebase for the Spatial Malaria Simulation (MaSim) under development by the Boni Lab at Temple University. The codebase was originally forked from [maciekboni/PSU-CIDD-Malaria-Simulation](https://github.com/maciekboni/PSU-CIDD-Malaria-Simulation) and was detached in perpetration of future development.

A comprehensive [technical manual](manual/manual.pdf) (PDF) for the simulation can be found in the [manual](manual/) directory which includes comprehensive directions for working with the simulation; however, a basic reference is provided in the Wiki. Stable code specific to publications are maintained in repositories under the [Boni Lab on GitHub](https://github.com/bonilab). 

The simulation has been tested to run on Windows 10, Windows Subsystem for Linux (Ubuntu), and Red Hat 7.9. The majority of development is performed on under Linux so building and running under Windows may be impacted.  While basic simulations are possible on desktop computing environments, regional and national scale simulations require advanced computing environments with access to 64 GB of RAM or more. Sample configuration files can be found under [documentation/input/](documentation/input), and examination of `simple.yml` or `spatial.yml` is recommended after working with the demonstration configuration in [documentation/demo/](documentation/demo/).

After cloning the repository to your local computer, the `config.sh` script can be run via `sudo` to install dependencies for building and creation of the build script at `build\build.sh`.

## Command Line Arguments

The following commands are available from the simulation:
<pre>
-c / --config     Configuration file, variant flag 
-h / --help       Display this help menu
-i / --input      Configuration file, preferred flag
-j                The job number for this replicate
-l / --load       Load genotypes to the database and exit
-o                The path for output files, default is the current directory
-r                The reporter type to use, multiple supported when comma delimited
-s                The study number to associate with the configuration

--dump            Dump the movement matrix as calculated
--im              Record individual movement detail
--lg              List the possible genotypes and their internal id values
--lr              List the possible data reporters
--mc              Record the movement between cells, cannot run with --md
--md              Record the movement between districts, cannot run with --mc

--v=[int]         Sets the verbosity of the logging, default zero
</pre>

Use of either the `-c` or `-i` switch with an appropriate YAML file is required. When the `-r` switch is not supplied the simulation defaults to the `DbReporter`; however, with the `-r` switch the reporters listed using the `--lr` switch can be used instead.


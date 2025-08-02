# Modeling Malaria Dynamics and Interventions in Uganda

This repository contains configuration files, Geographic Information System (GIS) data, and analysis scripts used for simulating the prevalence and transmission dynamics of _Plasmodium falciparum_ malaria in **Uganda**. Uganda faces a significant malaria burden, making it a critical area for modeling and intervention planning.

This work utilizes the `Temple-Malaria-Simulation` framework, developed by the Boni Lab (formerly associated with Penn State's Center for Infectious Disease Dynamics (CIDD), now at Temple University). The primary goals of these simulations are to:

1. Investigate the emergence and spread of antimalarial drug resistance within the parasite population in Uganda.
2. Evaluate the potential impact of various intervention strategies considered by the National Malaria Control Division (NMCD) of the Ugandan Ministry of Health.

The `Temple-Malaria-Simulation` model is actively used to support policy decisions in Uganda by simulating complex scenarios related to drug resistance and control strategies.

---

## System Requirements

- **Operating System**: Tested on **Ubuntu 20.04**
- **Compiler**: Requires **GNU G++** (compatible with C++17)
- **CPU**: Single-core execution (no multithreading)
- **RAM**: At least **40 GB** for large-scale runs simulating 25% of Uganda's ~50M population
- **Disk**: ~50 MB per output SQLite file
- **Dependencies** (defined in `CMakeLists.txt`):
  - `GSL`
  - `yaml-cpp`
  - `fmt`
  - `CLI11`
  - `date`
  - `args`
  - `unofficial-sqlite3`
- **Non-standard hardware**: None required

---

## Repository Structure

This repository is organized as follows:

- `GIS/`: Contains GIS files prepared for the **Uganda** simulation context. This likely includes spatial boundaries, environmental covariates (like rainfall or temperature), population density maps, and potentially health facility locations relevant for modeling malaria transmission in Uganda.

- `Validation/`: Includes configuration files used for initial model calibration, validation against known Ugandan epidemiological data (e.g., malaria prevalence rates from surveys like the Uganda Malaria Indicator Survey), and specific simulations exploring _de novo_ mutation events related to drug resistance.

- `Analysis/`: Contains Python scripts and Jupyter Notebooks used for post-simulation analysis, including processed data, visualization, and statistical analysis of simulation outcomes.

- `Results/`: Contains configuration files for the main analysis scenarios simulating different intervention strategies in Uganda, as presented in the associated publication.

  - **Note:** Due to their large size, the raw output data files (e.g., SQLite databases) generated from these simulations are **not** stored directly in this directory. They can be downloaded from the **[Releases](https://github.com/bonilab/Uganda-phase-1/releases/tag/v1.0_250505)** section of this GitHub repository.

- `Source/`: A mirror of the specific version of the `Temple-Malaria-Simulation` source code used for these analyses. This is included for reproducibility, ensuring the exact modeling framework version is accessible.
  - **Origin:** [bonilab/Temple-Malaria-Simulation GitHub](https://github.com/bonilab/Temple-Malaria-Simulation/)
  - **Branch Used:** `4.x.main`

---

## Installation Instructions

### Option 1: Standard CMake

```bash
mkdir build
cd build
cmake ..
make
```

### Option 2: Using vcpkg

```bash
# Clone and bootstrap vcpkg
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh

# Install required packages
./vcpkg install gsl yaml-cpp fmt cli11 date args3 unofficial-sqlite3

# Build with toolchain
cd ..
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
make
```

Replace `/path/to/vcpkg/` with your local vcpkg path.

---

## Getting Started

1. **Clone the repository:**

   ```bash
   git clone https://github.com/bonilab/Uganda-phase-1.git
   cd Uganda-phase-1
   ```

2. **Build the Simulation Executable:**
   Follow the instructions in the "Installation Instructions" section above.

3. **Run a Simulation:**
   Navigate to a configuration folder, then run:

   ```bash
   ./MaSim -i input.yaml -r SQLiteDistrictReporter -j 10
   ```

   - `-i` specifies the input YAML config file
   - `-r` specifies the reporter (default: `SQLiteDistrictReporter`)
   - `-j` is the job ID, which is used to label the output file

   The simulation will produce:

   ```
   monthly_data_10.db
   ```

---

## Sample Scenarios and Reproducibility

The repository includes a `Sample_Datasets/` folder with a minimal example `Status Quo` scenarios that can be used to verify your installation and understand the model structure:

- `000_status_quo`

The folder contains:

- `bin/` – a precompiled `MaSim` executable
- `input/` – a sample `input.yml` file
- `raw/` – a subfolder to execute the run and store outputs

### Running the Demo

To reproduce the results, on a Linux x86_64 system (tested on Ubuntu 20.04 and 24.04), run:

```bash
cd Sample_Datasets/000_status_quo/raw
../bin/MaSim -i ../input/input.yml -r SQLiteDistrictReporter -j 5
```

This will generate `monthly_data_5` in the same directory, which can be opened with any SQLite viewer or can be processed using Python.

---

## Simulation Execution Context

- **Initial Calibration & Validation:** Performed on Penn State's Institute for Computational and Data Sciences (ICDS) Roar supercomputer using configurations in the `Validation/` directory.

- **Main Analysis Scenarios:** Executed on the Temple University High Performance Computing Cluster ([TU HPC](https://www.hpc.temple.edu/)) using files in the `Results/` directory.

- **Runtime & Memory:** For a simulation of 25% of Uganda’s population (\~12.5 million individuals), the model requires approximately:

  - **40 GB of RAM**
  - **48 hours** wall-clock time
  - **50 MB** of output in a SQLite `.db` file

---

## Citation

If you use the configurations, data, or findings from this repository in your work, please cite the associated publication:

- `TBD`

---

## Contact

For questions regarding the configurations, analyses, or data specific to this Uganda modeling project, please contact:

- Maciej F. Boni / Boni Lab
- Email: `mboni-at-temple.edu`
- Or open an issue on this GitHub repository.

---

## License

This work is licensed under the [Creative Commons Attribution-NonCommercial 4.0 International License (CC BY-NC 4.0)](https://creativecommons.org/licenses/by-nc/4.0/).

![CC BY-NC 4.0](https://licensebuttons.net/l/by-nc/4.0/88x31.png)

You are free to use, adapt, and share this material for **non-commercial purposes**, provided you give appropriate credit.

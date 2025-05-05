# Modeling Malaria Dynamics and Interventions in Uganda

This repository contains configuration files, Geographic Information System (GIS) data, and analysis scripts used for simulating the prevalence and transmission dynamics of _Plasmodium falciparum_ malaria in **Uganda**. Uganda faces a significant malaria burden, making it a critical area for modeling and intervention planning.

This work utilizes the `Temple-Malaria-Simulation` framework, developed by the Boni Lab (formerly associated with Penn State's Center for Infectious Disease Dynamics (CIDD), now at Temple University). The primary goals of these simulations are to:

1.  Investigate the emergence and spread of antimalarial drug resistance within the parasite population in Uganda.
2.  Evaluate the potential impact of various intervention strategies considered by the National Malaria Control Division (NMCD) of the Ugandan Ministry of Health.

The `Temple-Malaria-Simulation` model is actively used to support policy decisions in Uganda by simulating complex scenarios related to drug resistance and control strategies.

## Repository Structure

This repository is organized as follows:

- `GIS/`: Contains GIS files prepared for the **Uganda** simulation context. This likely includes spatial boundaries, environmental covariates (like rainfall or temperature), population density maps, and potentially health facility locations relevant for modeling malaria transmission in Uganda.
- `Validation/`: Includes configuration files used for initial model calibration, validation against known Ugandan epidemiological data (e.g., malaria prevalence rates from surveys like the Uganda Malaria Indicator Survey), and specific simulations exploring _de novo_ mutation events related to drug resistance.
- `Results/`: Contains configuration files for the main analysis scenarios simulating different intervention strategies in Uganda, as presented in the associated publication.
  - **Note:** Due to their large size, the raw output data files (e.g., SQLite databases) generated from these simulations are **not** stored directly in this directory. They can be downloaded from the **[Releases](https://github.com/bonilab/Uganda-phase-1/releases/tag/v1.0_250505)** section of this GitHub repository.
- `Source/`: A mirror of the specific version of the `Temple-Malaria-Simulation` source code used for these analyses. This is included for reproducibility, ensuring the exact modeling framework version is accessible.
  - **Origin:** [bonilab/Temple-Malaria-Simulation GitHub](https://github.com/bonilab/Temple-Malaria-Simulation/)
  - **Branch Used:** `4.x.main`

## Prerequisites

To run these simulations or analyze the results, you may need:

- Git (for cloning the repository)
- Access to the `Temple-Malaria-Simulation` software (compilation may be required, see its documentation linked above).
- Python (for analysis scripts, potentially) and relevant libraries (e.g., pandas, sqlite3, geospatial libraries if interacting with GIS data).
- Access to a High-Performance Computing (HPC) environment is likely necessary to run the large-scale Uganda simulations in a reasonable timeframe, as originally performed.

## Getting Started

1.  **Clone the repository:**
    ```bash
    # Note: Uses the actual repository name
    git clone https://github.com/bonilab/Uganda-phase-1.git
    cd Uganda-phase-1
    ```
2.  **Set up the Simulation Environment:** Ensure you have a working build of the `Temple-Malaria-Simulation` executable corresponding to the version in the `Source/` directory or the specified commit. Refer to the original simulation software's documentation for installation instructions.
3.  **Run a Simulation:**
    - Navigate to a configuration directory (e.g., `Validation/some_scenario/` or `Results/some_scenario/`).
    - Execute the simulation using the configuration file provided (e.g., `config.yml` or similar). The exact command will depend on the `Temple-Malaria-Simulation` executable. It might look something like:
      ```bash
      ./MaSim -i path/to/config.yml
      ```
    - _(Note: Execution often requires significant computational resources and time)._

## Simulation Execution Context

- **Initial Calibration & Validation:** Performed on the Pennsylvania State University’s Institute for Computational and Data Sciences (ICDS) Roar supercomputer using configurations primarily found in the `Validation/` directory, likely reflecting the project's origins or collaborators' resources at the time.
- **Main Analysis Scenarios:** Performed on the Temple University High Performance Computing Cluster ([TU HPC](https://www.hpc.temple.edu/)) using configurations primarily found in the `Results/` directory to explore intervention impacts in Uganda, reflecting the simulation group's current affiliation.

## Citation

If you use the configurations, data, or findings from this repository in your work, please cite the associated publication:

- `TBD`

## Contact

For questions regarding the configurations, analyses, or data specific to this Uganda modeling project, please contact:

- `Maciej F. Boni / Boni Lab`
- `mboni-at-temple.edu`
- Or open an issue on this GitHub repository.

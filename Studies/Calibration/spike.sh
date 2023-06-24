#!/bin/bash

# Script to run various C469Y spiking combinations.
#
# Note that the calibrationLib.sh from PSU-CIDD-MaSim-Supoort is needed
source ./calibrationLib.sh

# Settings for the cluster
user='rbz5100'

# Iterate over all of the key variables and create the replicates
for raster in agago gulu; do
  for year in `seq 2010 1 2013`; do
    for fraction in `seq 0.005 0.005 0.1`; do
      
      # Prepare the files
      configuration="uga-spike-$raster-$year-$fraction.yml"
      sed 's/#YEAR#/'"$year"'/g' uga-spike-template.yml > $configuration
      sed -i 's/#FRACTION#/'"$fraction"'/g' $configuration
      sed -i 's/#LOCATION#/'"$raster"'/g' $configuration
      job="uga-spike-$raster-$year-$fraction.job"
      sed 's/#FILENAME#/'"$configuration"'/g' uga-spike-template.job > $job

      # Queue the job
      check_delay $user
      sbatch $job
    done
  done
done

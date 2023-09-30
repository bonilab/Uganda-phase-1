# uganda.py
#
# This file contains common properties for Uganda and associated reporting and plots.
import numpy as np
import os
import pandas as pd
import sys

# From the PSU-CIDD-MaSim-Support repository
sys.path.insert(1, '../../PSU-CIDD-MaSim-Support/Python/include')
from utility import progressBar

# Connection string for the database
CONNECTION = 'host=masimdb.vmhost.psu.edu dbname=uganda user=sim password=sim connect_timeout=60'

# The mapping file for the districts
DISTRICTS_MAPPING = '../GIS/administrative/uga_districts.csv'

# First year of model execution
MODEL_YEAR = 2004

# Template and paths for the mutations
MUTATIONS_TEMPLATE = '../GIS/mutations/uga_{}_mutations.csv'
MUTATIONS_469Y = '../GIS/mutations/uga_469y_mutations.csv'
MUTATIONS_675V = '../GIS/mutations/uga_675v_mutations.csv'

# Settings for plots
LINE_CONFIGURATION = 'include/matplotlibrc-line'
VIOLIN_CONFIGURATION = 'include/matplotlibrc-violin'

def load_all_datasets():
    DATASETS_PATH = '../Analysis/data/datasets'

    datasets = {}
    for file in os.listdir(DATASETS_PATH):
        key = file.split('/')[-1].replace('uga-policy-', '').replace('.csv', '')
        datasets[key] = load_dataset(os.path.join(DATASETS_PATH, file))
    return datasets, datasets[key].days.unique()


def load_dataset(dataset):
    REPLICATE, DATES, DISTRICT, INFECTIONS, TREATMENTS, FAILURES = 1, 2, 3, 4, 12, 13
    MUTATION_MAPPING = { '469Y' : 8, '675V' : 11, 'either' : 14 }
    CACHE_DIRECTORY = 'cache/'

    # Check to see if the cache exists, load and return if it does
    filename = dataset.split('/')[-1].replace('uga-policy-', '').replace('.csv', '')
    cache_file = os.path.join(CACHE_DIRECTORY, filename + '-cache.csv')
    if os.path.exists(cache_file):
        return pd.read_csv(cache_file)

    # Inform the user
    print('Create cache for {}...'.format(filename))

    # The cache does not exist, start by loading the full dataset
    data = pd.read_csv(dataset, header=None)
    dates = data[DATES].unique().tolist()
    replicates = data[REPLICATE].unique()

    # Parse the data into a list of dictionaries
    rows = []
    for replicate in replicates:
        for date in dates:

            # Filter on this combination
            subset = data[(data[REPLICATE] == replicate) & (data[DATES] == date)]

            # Parse the simple items, create the row
            treatments = np.sum(subset[TREATMENTS])
            failures = np.sum(subset[FAILURES])
            infections = np.sum(subset[INFECTIONS])
            row = {'replicate': replicate, 'days': date, 'treatments': treatments, 'failures': failures, 'infections': infections}

            # Append the mutation data to the row
            for mutation, index in MUTATION_MAPPING.items():
                row[mutation] = np.sum(subset[index])

            # Append the row to the rows and note the status
            rows.append(row)
            progressBar(len(rows), len(dates) * len(replicates))

    # Copy the data to the data frame, save, and return the data
    df = pd.DataFrame(rows)
    df.to_csv(cache_file, index=False)
    return df
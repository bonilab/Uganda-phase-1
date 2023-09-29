# violin.py
#
# This class wraps the functions related to plotting the frequency and treatment failure violin plots.
import datetime
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os
import pandas as pd
import sys

import include.uganda as uganda

# From the PSU-CIDD-MaSim-Support repository
sys.path.insert(1, '../../PSU-CIDD-MaSim-Support/Python/include')
from plotting import increment, scale_luminosity
from utility import progressBar

class violin:
  # Internal mapping of the dataset columns
  REPLICATE, DATES, DISTRICT, INFECTIONS, TREATMENTS, FAILURES = 1, 2, 3, 4, 12, 13
  MUTATION_MAPPING = { '469Y' : 8, '675V' : 11, 'either' : 14 }

  # Internal constant for the cache directory
  CACHE_DIRECTORY = 'cache/'

  # Various private member variables for data processing
  dataset, mutations = None, None
  
  # Various private member variables for formatting
  labels, title = None, None
  
  def __load(self, filename):
    # Check to see if the cache exists, load and return if it does
    filename = self.dataset.split('/')[-1].replace('uga-policy-', '').replace('.csv', '')
    cache_file = os.path.join(self.CACHE_DIRECTORY, filename + '-cache.csv')
    if os.path.exists(cache_file):
      return pd.read_csv(cache_file)

    # Inform the user
    print('Create cache for {}...'.format(filename))

    # The cache does not exist, start by loading the full dataset
    data = pd.read_csv(self.dataset, header=None)
    dates = data[self.DATES].unique().tolist()
    replicates = data[self.REPLICATE].unique()

    # Parse the data into a list of dictionaries
    rows = []
    for replicate in replicates:
      for date in dates:
        # Filter on this combination
        subset = data[(data[self.REPLICATE] == replicate) & (data[self.DATES] == date)]

        # Parse the simple items, create the row
        treatments = np.sum(subset[self.TREATMENTS])
        failures = np.sum(subset[self.FAILURES])
        infections = np.sum(subset[self.INFECTIONS])
        row = {'replicate': replicate, 'days': date, 'treatments': treatments, 'failures': failures, 'infections': infections}

        # Append the mutation data to the row
        for mutation, index in self.MUTATION_MAPPING.items():
          row[mutation] = np.sum(subset[index])

        # Append the row to the rows and note the status
        rows.append(row)
        progressBar(len(rows), len(dates) * len(replicates))

    # Copy the data to the data frame, save, and return the data
    df = pd.DataFrame(rows)
    df.to_csv(cache_file, index=False)
    return df


  def __plot(self, data, title, filename):

    # Setup to generate the plot
    matplotlib.rc_file(uganda.VIOLIN_CONFIGURATION)

              
    # Save the plot
    plt.savefig('out/{}'.format(filename))
    plt.close()
  

  def __process_frequency(self):
    MAPPING = { '469Y' : 8, '675V' : 11, 'either' : 14 }
    
    # Load relevant data, dates, and labels
    data = pd.read_csv(self.dataset, header=None)
    dates = data[self.DATES].unique().tolist()
    dates = [datetime.datetime(uganda.MODEL_YEAR, 1, 1) + datetime.timedelta(days=x) for x in dates]
    self.labels = pd.read_csv(uganda.DISTRICTS_MAPPING)

    for mutation in MAPPING.keys():
      print('Plotting {} mutation for {}'.format(mutation, self.dataset))

      # Calculate the frequency, prepare the mutation data
      data['frequency'] = data[MAPPING[mutation]] / data[self.INFECTIONS]
      if mutation == 'either':
        # We use the district with more points if either mutation is plotting
        self.mutations = pd.read_csv(uganda.MUTATIONS_TEMPLATE.format('675V'))
      else:  
        self.mutations = pd.read_csv(uganda.MUTATIONS_TEMPLATE.format(mutation))
    
      # Set the title, labels, and filename for the results
      title = '{}, {}'.format(self.title, mutation)
      ylabel = '{} Frequency'.format(mutation)
      if mutation == 'either':
        title = '{} / Total ART Resistance'.format(self.title)
        ylabel = 'Total ART Resistance Frequency'
      filename = self.dataset.split('/')[-1].replace('uga-policy-', '').replace('.csv', '')
      filename += '-{}.png'.format(mutation)

      # Prepare the plot
      self.__plot(data, dates, mutation, ylabel, title, filename)

    # Free the memory and return
    del data


  def __process_treatment_failure(self, data):
        
    # Set the title, labels, and filename for the results
    title = '{}, Treatment Failures'.format(self.title)
    filename = self.dataset.split('/')[-1].replace('uga-policy-', '').replace('.csv', '')
    filename += '-tf.png'

    # Prepare the plot
    print('Plotting treatment failures for {}'.format(self.dataset))
    self.__plot(data, title, filename)


  def process(self, dataset, title):
    """Process the dataset in the file and generate three spaghetti plots.
    
    dataset - The full or relative path to the file"""
    os.makedirs(self.CACHE_DIRECTORY, exist_ok=True)

    self.dataset = dataset
    self.title = title
    self.labels = pd.read_csv(uganda.DISTRICTS_MAPPING)

    data = self.__load(dataset)
    self.__process_treatment_failure(data)
    # self.__process_frequency()

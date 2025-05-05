# median.py
#
# This class wraps the functions related to plotting the median and IQR.
import datetime
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os
import pandas as pd
import sys

import include.uganda as uganda
from include.uganda import DATASET_LAYOUT

# From the PSU-CIDD-MaSim-Support repository
sys.path.insert(1, '../../PSU-CIDD-MaSim-Support/Python/include')
from plotting import increment, scale_luminosity

class median:
  DIRECTORY = os.path.join('out', 'median')
    
  # Various private member variables for formatting
  labels, title = None, None
  
  def __districts(self, filename):
    # Load relevant data, dates, and labels
    data = pd.read_csv(filename, header=None)
    dates = data[DATASET_LAYOUT['dates']].unique().tolist()
    dates = [datetime.datetime(uganda.MODEL_YEAR, 1, 1) + datetime.timedelta(days=x) for x in dates]
    self.labels = pd.read_csv(uganda.DISTRICTS_MAPPING)

    for mutation, index in DATASET_LAYOUT['mutations'].items():
      print('Creating district plot for {}...'.format(mutation))    

      # Calculate the frequency based on the current mutation
      data['frequency'] = data[index] / data[DATASET_LAYOUT['infections']]
    
      # Set the title, labels, and filename for the results
      title = '{}, {}'.format(self.title, mutation)
      ylabel = '{} Frequency'.format(mutation)
      if mutation == 'either':
        title = '{} / Total ART Resistance'.format(self.title)
        ylabel = 'Total ART Resistance Frequency'
      image_filename = filename.split('/')[-1].replace('uga-policy-', '').replace('.csv', '')
      image_filename += '-{}.png'.format(mutation)

      # Prepare the plot
      self.__plot_districts(data, dates, mutation, ylabel, title, image_filename)

    # Free the memory before returning
    del data


  def __plot_districts(self, data, dates, mutation, ylabel, title, filename):
    ROWS, COLUMNS = 3, 5

    def add_points():
      row, col = 0, 0
      for district in districts:
        plt.sca(axes[row, col])
        for index, data_row in mutation_points[mutation_points.District == district].iterrows():
          x = datetime.datetime(data_row.Year, 9, 30)
          y = data_row.Frequency
          plt.scatter(x, y, color = 'black', s = 100, zorder = 99)
        row, col = increment(row, col, COLUMNS)
  
    # Load the mutation point data 
    if mutation == 'either':
      # We use the district with more points if either mutation is plotting
      mutation_points = pd.read_csv(uganda.MUTATIONS_TEMPLATE.format('675V'))
    else:  
      mutation_points = pd.read_csv(uganda.MUTATIONS_TEMPLATE.format(mutation))    
    districts = mutation_points.District.unique()
  
    # Get the frequency data and transpose it
    frequencies = {}
    for replicate in data[DATASET_LAYOUT['replicate']].unique():
      for district in districts:
        id = self.labels[self.labels.Label == district].ID.values[0]
        row = data[(data[DATASET_LAYOUT['district']] == id) & (data[DATASET_LAYOUT['replicate']] == replicate)].frequency.tolist()
        if district in frequencies.keys():
          frequencies[district] = np.vstack((frequencies[district], row))
        else:
          frequencies[district] = row

    # Setup to generate the plot
    matplotlib.rc_file(uganda.LINE_CONFIGURATION)
    figure, axes = plt.subplots(ROWS, COLUMNS)
    figure.suptitle(title, y = 0.94)

    # Generate a 15 panel plot while looping over the districts that we have spiking data for
    row, col = 0, 0
    for district in districts:
      upper = np.percentile(frequencies[district], 75, axis=0)
      median = np.percentile(frequencies[district], 50, axis=0)
      lower = np.percentile(frequencies[district], 25, axis=0)

      # Add the data to the plot
      axes[row, col].plot(dates, median)
      color = scale_luminosity(axes[row, col].lines[-1].get_color(), 1)
      axes[row, col].fill_between(dates, lower, upper, alpha=0.5, facecolor=color)
      axes[row, col].title.set_text(district)
      row, col = increment(row, col, COLUMNS)
        
    # Next, add the known data points to the plots unless we are plotting the total resistance
    if mutation != 'either': add_points()
          
    # Format the x, y axis and ticks
    for row in range(3):
      for col in range(5):
        axes[row, col].set_ylim([0, 1])
        axes[row, col].set_xlim([min(dates), max(dates)])
        axes[row, col].xaxis.set_major_formatter(matplotlib.dates.DateFormatter("'%y"))
        if row != 2 and not (row == 1 and col == 4):
          plt.setp(axes[row, col].get_xticklabels(), visible = False)
        if col != 0:
          plt.setp(axes[row, col].get_yticklabels(), visible = False)
          
    # Apply the final figure formatting
    if len(districts) < 15:
      axes[2, 4].set_visible(False)
    plt.setp(axes[2, 0].get_xticklabels()[0], visible = False)
    plt.sca(axes[1, 0])
    plt.ylabel(ylabel)
    plt.sca(axes[2, 2])
    plt.xlabel('Model Year')
    
    # Save the plot
    os.makedirs(self.DIRECTORY, exist_ok=True)
    plt.savefig(os.path.join(self.DIRECTORY, filename))
    plt.close()
  

  def __national(self, filename):
    # Since we need national summary data, we can use the cache if it is available
    data = uganda.load_dataset(filename)
    dates = data.days.unique().tolist()
    dates = [datetime.datetime(uganda.MODEL_YEAR, 1, 1) + datetime.timedelta(days=x) for x in dates]

    for mutation in DATASET_LAYOUT['mutations'].keys():
      print('Creating national plot for {}...'.format(mutation))    

      # Calculate the frequency based on the current mutation
      data['frequency'] = data[mutation] / data.infections

      # Set the title, labels, and filename for the results
      title = '{}, {}'.format(self.title, mutation)
      ylabel = '{} Frequency'.format(mutation)
      if mutation == 'either':
        title = '{} / Total ART Resistance'.format(self.title)
        ylabel = 'Total ART Resistance Frequency'
      image_filename = filename.split('/')[-1].replace('uga-policy-', '').replace('.csv', '')
      image_filename += '-national-{}.png'.format(mutation)

      # Prepare the plot
      self.__plot_national(data, dates, ylabel, title, image_filename)

    # Free the memory before returning
    del data

  
  def __plot_national(self, data, dates, ylabel, title, filename):

    # Get the frequency data and transpose it
    frequencies = []
    for replicate in data.replicate.unique():
        row = data[data.replicate == replicate].frequency.tolist()
        if len(frequencies) != 0:
          frequencies = np.vstack((frequencies, row))
        else:
          frequencies = row

    # Calculate the median and IQR
    upper = np.percentile(frequencies, 75, axis=0)
    median = np.percentile(frequencies, 50, axis=0)
    lower = np.percentile(frequencies, 25, axis=0)

    # Setup and format the plot
    matplotlib.rc_file(uganda.LINE_CONFIGURATION)
    axes = plt.axes()
    axes.set_xlim([min(dates), max(dates)])
    axes.set_ylim([0, 1.0])
    axes.set_title(title)
    axes.set_ylabel(ylabel)

    # Add the data
    plt.plot(dates, median)
    color = scale_luminosity(plt.gca().lines[-1].get_color(), 1)
    plt.fill_between(dates, lower, upper, alpha=0.5, facecolor=color)

    # Save the plot
    os.makedirs(self.DIRECTORY, exist_ok=True)
    plt.savefig(os.path.join(self.DIRECTORY, filename))
    plt.close()



  def process(self, filename, title):
    """Process the dataset in the file and generate three spaghetti plots.
    
    dataset - The full or relative path to the file"""

    self.title = title

    print('Creating median and IQR plots for: {}'.format(filename))
    self.__districts(filename)
    self.__national(filename)

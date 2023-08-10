#!/usr/bin/python3

# spiking.py
#
# Load the relevant spike studies from the database.
import argparse
import datetime
import matplotlib
import matplotlib.dates
import matplotlib.pyplot as plt
import numpy as np
import os
import pandas as pd

from include.spike.calibration import calibration
from include.spike.district import district
from include.spike.loader import loader
import include.spike.common as shared


# This class wraps the functions related to plotting dual spike studies and 
# the spike calibration / validation studies.
class dual_spike:
  # Note the assumed model year
  MODEL_YEAR = 2004

  mutations = None
  labels = None
  
  def __plot(self, replicates, mutation, ylabel, title, filename):
    DATES, DISTRICT, INFECTIONS = 2, 3, 4

    # Note the correct index of the weighted values
    if mutation == '469Y': weighted = 8
    elif mutation == '675V': weighted = 11
    else: raise Exception('Unknown mutation: {}'.format(mutation))
  
    # Setup to generate the plot
    matplotlib.rc_file('../Scripts/matplotlibrc-line')
    figure, axes = plt.subplots(3, 5)
    figure.suptitle(title, y = 0.94)
    
    # Set a single order for the districts
    districts = self.mutations.District.unique()
  
    # Start by preparing the replicate data that we need to plot
    ymax = max(self.mutations.Frequency)
    for replicate in replicates:
      # Load the data and prepare the dates
      data = pd.read_csv('data/spiking/{}.csv'.format(replicate), header = None)
      data['frequency'] = data[weighted] / data[INFECTIONS]
      ymax = max(ymax, max(data.frequency))
      dates = data[DATES].unique().tolist()
      dates = [datetime.datetime(self.MODEL_YEAR, 1, 1) + datetime.timedelta(days=x) for x in dates]
  
      # Generate a 15 panel plot while looping over the districts that we have spiking data for
      row, col = 0, 0
      for district in districts:
        district_id = self.labels[self.labels.Label == district].ID.values[0]
        axes[row, col].plot(dates, data[data[DISTRICT] == district_id].frequency)
        axes[row, col].title.set_text(district)
        row, col = shared.increment(row, col)
          
    # Next, add the know data points to the plots
    row, col = 0, 0
    for district in districts:
      plt.sca(axes[row, col])
      for index, data_row in self.mutations[self.mutations.District == district].iterrows():
        x = datetime.datetime(data_row.Year, 9, 30)
        y = data_row.Frequency
        plt.scatter(x, y, color = 'black', s = 100, zorder = 99)
      row, col = shared.increment(row, col)
          
    # Format the x, y axis and ticks
    for row in range(3):
      for col in range(5):
        axes[row, col].set_ylim([0, ymax])
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
    plt.savefig('plots/{}'.format(filename))
    plt.close()
  
  def __process(self, mutation):
    CONFIGURATION, REPLICATE, FILENAME = 0, 3, 2
  
    # Load relevant data
    data = pd.read_csv(shared.REPLICATES_LIST, header = None)
    self.labels = pd.read_csv(shared.DISTRICTS_MAPPING)
    self.mutations = pd.read_csv(shared.MUTATIONS_TEMPLATE.format(mutation))
  
    configurations = []
    shared.progressBar(0, len(data))
    for index, row in data.iterrows():
      try:
        # Check to see if we can skip this entry
        if len(data[data[FILENAME] == row[FILENAME]]) == 1: continue
        if row[CONFIGURATION] in configurations: continue

        # Get the list of replicates associated with this configuration
        replicates = data[data[CONFIGURATION] == row[CONFIGURATION]][REPLICATE]

        # Set the title and filename for the results
        title = 'Spike Calibration, {}'.format(mutation)
        filename = 'uga-spike-{}-{}.png'.format(row[CONFIGURATION], mutation)

        # Prepare the plot, note the configuration
        self.__plot(replicates, mutation, '{} Frequency'.format(mutation), title, filename)
        configurations.append(row[CONFIGURATION])
        shared.progressBar(index, len(data))
      except Exception as ex:
          print('\nError plotting replicate {}, configuration {}'.format(row[REPLICATE], row[FILENAME]))
          print(ex)
    shared.progressBar(len(data), len(data))    

  def process(self):
    self.__process('469Y')
    self.__process('675V')


def plot_genotypes():
  
  def plot(filename, color):
    row, col = 0, 0
    data = pd.read_csv(filename)
    for district in districts:
      # Plot the frequency points
      plt.sca(axes[row, col])
      for index, data_row in data[data.District == district].iterrows():
        x = datetime.datetime(data_row.Year, 1, 1)
        y = data_row.Frequency
        plt.scatter(x, y, color = color, s = 100, zorder = 99)
      # Move to the next plot
      row, col = shared.increment(row, col)
    return max(data.Frequency)
      
  # Generate the list of unique districts
  districts = pd.read_csv(shared.MUTATIONS_469Y).District.unique()  
  districts = np.append(districts, pd.read_csv(shared.MUTATIONS_675V).District.unique())
  districts = np.unique(districts)
  
  # Prepare the figure
  matplotlib.rc_file('../Scripts/matplotlibrc-line')
  figure, axes = plt.subplots(3, 5)
  
  # Add the data points
  ymax = max(plot(shared.MUTATIONS_469Y, 'black'),
             plot(shared.MUTATIONS_675V, 'red'))
  
  # Format the plots
  row, col = 0, 0
  for district in districts:
    axes[row, col].set_ylim([0, ymax])
    axes[row, col].set_xlim([datetime.datetime(2015, 1, 1), datetime.datetime(2022, 1, 1)])
    axes[row, col].title.set_text(district)
    if row != 2:
      plt.setp(axes[row, col].get_xticklabels(), visible = False)
    else:
      axes[row, col].xaxis.set_major_formatter(matplotlib.dates.DateFormatter("'%y"))  
    if col != 0:
      plt.setp(axes[row, col].get_yticklabels(), visible = False)
    row, col = shared.increment(row, col)
    
  # Wrap up the formatting
  plt.setp(axes[2, 0].get_xticklabels()[0], visible = False)
  plt.sca(axes[1, 0])
  plt.ylabel('469Y (black) / 675V (red) Frequency')

  # Save the plot
  plt.savefig('plots/datapoints.png')
  plt.close()


def main(args):
  # Perform any common setup
  if not os.path.exists(os.path.join(shared.PLOTS_DIRECTORY, '469Y')): 
    os.makedirs(os.path.join(shared.PLOTS_DIRECTORY, '469Y'))
  if not os.path.exists(os.path.join(shared.PLOTS_DIRECTORY, '675V')):
    os.makedirs(os.path.join(shared.PLOTS_DIRECTORY, '675V'))

  # Everything goes through the same loader
  loader().load()

  # Hand things off to the correct processing
  if args.type == 'c':
    calibration().process()
  elif args.type == 'd':
    dual_spike().process()
  elif args.type == 's':
    district().process('469Y')
    district().process('675V')
  elif args.type == 'g':
    plot_genotypes()    
  else:
    print('Unknown type parameter, {}'.format(args.type))
     

if __name__ == '__main__':
  # Parse the parameters and defer to the main function
  parser = argparse.ArgumentParser()
  parser.add_argument('-t', action='store', dest='type', required=True,
    help='The type of plots to generate, c for calibration, d for dual spiking, or s for single district')
  main(parser.parse_args())

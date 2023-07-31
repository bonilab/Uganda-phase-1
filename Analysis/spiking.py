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
    help='The type of plots to generate, c for calibration or d for district')
  main(parser.parse_args())

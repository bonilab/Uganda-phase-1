#!/usr/bin/python3

# plot_irs_impact.py
#
# Generate impact plots for the IRS experiments.
import datetime
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys

sys.path.insert(1, '../../PSU-CIDD-MaSim-Support/Python/include')
from plotting import scale_luminosity

DATA = 'data/uga-lamwo.csv'
MODEL_YEAR = 2004    
    
def plot_clinical():
  # Load the data, note the unique replicates, dates, and filenames
  data = pd.read_csv(DATA)
  dates = data.dayselapsed.unique()
  dates = [datetime.datetime(MODEL_YEAR, 1, 1) + datetime.timedelta(days=int(x)) for x in dates]
  filenames = data.filename.unique()
  
  for filename in filenames:
    
    # Prepare the plot
    matplotlib.rc_file('../Scripts/matplotlibrc-line')
    replicates = []
    for replicate in data[data.filename == filename].id.unique():
      if len(replicates) == 0:
        replicates = data[data.id == replicate].clinicalepisodes
      else:
        replicates = np.vstack((replicates, data[data.id == replicate].clinicalepisodes))
        
    # Calculate the median and IRQ
    upper = np.percentile(replicates, 25, axis=0)
    median = np.percentile(replicates, 50, axis=0)
    lower = np.percentile(replicates, 75, axis=0)
      
    # Generate the plot
    lines = plt.plot(dates, median)
    color = scale_luminosity(lines[-1].get_color(), 1)
    plt.fill_between(dates, lower, upper, alpha=0.5, facecolor=color)

  # Format the plot    
  plt.legend(filenames)
  plt.ylim([0, 35000])
  plt.xlim([min(dates), max(dates)])
  plt.title('469Y Spike Comparison')
  plt.ylabel('Clinical Cases (Shaded: IQR)')
  plt.xlabel('Model Date')
  
  # Draw the intervention lines
  plt.axvline(x=datetime.datetime(2005, 4, 1), color='gray', ls='--')
  plt.text(datetime.datetime(2005, 5, 1), 30000, 'Spike 469Y', rotation=90, fontsize='small', color='gray')
  plt.axvline(x=datetime.datetime(2014, 1, 1), color='gray', ls='--')
  plt.text(datetime.datetime(2013, 10, 1), 24500, 'End of Routine Spraying', rotation=90, fontsize='small', color='gray')
  plt.axvline(x=datetime.datetime(2017, 1, 1), color='gray', ls='--')
  plt.text(datetime.datetime(2016, 10, 1), 27000, 'One Time Spraying', rotation=90, fontsize='small', color='gray')
  plt.axvline(x=datetime.datetime(2018, 1, 1), color='gray', ls='--')
  plt.text(datetime.datetime(2017, 10, 1), 28000, 'End of Spraying', rotation=90, fontsize='small', color='gray')
  
  # Save the figure
  plt.savefig('plots/irs_clinical.png')
  plt.close()


def plot_irs():
  # Load the data and calculate the frequency
  data = pd.read_csv(DATA)
  data['frequency'] = data.weightedoccurrences / data.infectedindividuals

  # Note the unique replicates and dates
  dates = data.dayselapsed.unique()
  dates = [datetime.datetime(MODEL_YEAR, 1, 1) + datetime.timedelta(days=int(x)) for x in dates]
  
  # Note the unique filenaames
  filenames = data.filename.unique()
  
  for filename in filenames:
    
    # Prepare the plot
    matplotlib.rc_file('../Scripts/matplotlibrc-line')
    replicates = []
    for replicate in data[data.filename == filename].id.unique():
      if len(replicates) == 0:
        replicates = data[data.id == replicate].frequency
      else:
        replicates = np.vstack((replicates, data[data.id == replicate].frequency))
        
    # Calculate the median and IRQ
    upper = np.percentile(replicates, 45, axis=0)
    median = np.percentile(replicates, 50, axis=0)
    lower = np.percentile(replicates, 55, axis=0)
      
    # Generate the plot
    lines = plt.plot(dates, median)
    color = scale_luminosity(lines[-1].get_color(), 1)
    plt.fill_between(dates, lower, upper, alpha=0.5, facecolor=color)

  # Format the plot    
  plt.legend(filenames)
  plt.ylim([0, 1])
  plt.xlim([min(dates), max(dates)])
  plt.title('469Y Spike Comparison')
  plt.ylabel('469Y frequency (Median, Shaded: ±5%)')
  plt.xlabel('Model Date')
  
  # Draw the intervention lines
  plt.axvline(x=datetime.datetime(2005, 4, 1), color='gray', ls='--')
  plt.text(datetime.datetime(2005, 5, 1), 0.87, 'Spike 469Y', rotation=90, fontsize='small', color='gray')
  plt.axvline(x=datetime.datetime(2014, 1, 1), color='gray', ls='--')
  plt.text(datetime.datetime(2014, 2, 1), 0.7, 'End of Routine Spraying', rotation=90, fontsize='small', color='gray')
  plt.axvline(x=datetime.datetime(2017, 1, 1), color='gray', ls='--')
  plt.text(datetime.datetime(2017, 2, 1), 0.78, 'One Time Spraying', rotation=90, fontsize='small', color='gray')
  plt.axvline(x=datetime.datetime(2018, 1, 1), color='gray', ls='--')
  plt.text(datetime.datetime(2018, 2, 1), 0.81, 'End of Spraying', rotation=90, fontsize='small', color='gray')
  
  # Save the figure
  plt.savefig('plots/irs_frequency.png')
  plt.close()
  
    
if __name__ == '__main__':
  plot_clinical()
  plot_irs()
  
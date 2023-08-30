# plot_irs_impact.py
#
# Generate impact plots for the IRS experiments.
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys

sys.path.insert(1, '../../PSU-CIDD-MaSim-Support/Python/include')
from plotting import scale_luminosity


def plot(filterOn):
  FILENAME = 'data/irs-data.csv'
  TEMPLATE = 'uga-irs-{}{}.yml'
  
  def __plot(filename, ndx):
    replicates = []
    for replicate in data[data.filename == filename].id.unique():
      if len(replicates) == 0:
        replicates = data[data.id == replicate].frequency
      else:
        replicates = np.vstack((replicates, data[data.id == replicate].frequency))
        
    # Calcluate the median and IRQ
    upper = np.percentile(replicates, 75, axis=0)
    median = np.percentile(replicates, 50, axis=0)
    lower = np.percentile(replicates, 25, axis=0)
    
    # Generate the plot
    axes[ndx].plot(dates, median)
    color = scale_luminosity(axes[ndx].lines[-1].get_color(), 1)
    axes[ndx].fill_between(dates, lower, upper, alpha=0.5, facecolor=color)
  
  # Load the data and calculate the frequency
  data = pd.read_csv(FILENAME)
  data['frequency'] = data.weightedoccurrences / data.infectedindividuals
  
  # Note the unique replicates and dates
  dates = data.dayselapsed.unique()
  
  # Prepare the plot
  matplotlib.rc_file('../Scripts/matplotlibrc-line')
  figure, axes = plt.subplots(2, 1)
  __plot(TEMPLATE.format(filterOn, ''), 0) 
  __plot(TEMPLATE.format(filterOn, '-steady'), 1)
    
  # Format the plot
  axes[0].title.set_text('{} Prevalence, IRS'.format(filterOn.capitalize()))
  axes[1].title.set_text('{} Prevalence, No Intervention'.format(filterOn.capitalize()))
  axes[1].set_xlabel('Model Days Elapsed')
  for ndx in [0, 1]:
    axes[ndx].set_ylim([0, 1])
    axes[ndx].set_xlim([min(dates), max(dates)])
    axes[ndx].set_ylabel('469Y frequency')
        
    
if __name__ == '__main__':
  # plot('high')
  plot('low')
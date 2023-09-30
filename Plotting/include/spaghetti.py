# spaghetti.py
#
# This class wraps the functions related to plotting a spaghetti plot.
import datetime
import matplotlib
import matplotlib.pyplot as plt
import pandas as pd
import sys

import include.uganda as uganda

# From the PSU-CIDD-MaSim-Support repository
sys.path.insert(1, '../../PSU-CIDD-MaSim-Support/Python/include')
from plotting import increment

class spaghetti:
  # Internal mapping of the dataset columns
  DATES, DISTRICT, INFECTIONS = 2, 3, 4
  MAPPING = { '469Y' : 8, '675V' : 11, 'either' : 14 }
  
  # Various private member variables for formatting
  labels, title = None, None
  
  def __plot(self, data, dates, mutation, ylabel, title, filename):
    ROWS, COLUMNS = 3, 5

    def add_points():
      row, col = 0, 0
      for district in districts:
        plt.sca(axes[row, col])
        for index, data_row in self.mutations[self.mutations.District == district].iterrows():
          x = datetime.datetime(data_row.Year, 9, 30)
          y = data_row.Frequency
          plt.scatter(x, y, color = 'black', s = 100, zorder = 99)
        row, col = increment(row, col, COLUMNS)
  
    # Setup to generate the plot
    matplotlib.rc_file(uganda.LINE_CONFIGURATION)
    figure, axes = plt.subplots(ROWS, COLUMNS)
    figure.suptitle(title, y = 0.94)
    
    # Set a single order for the districts
    districts = self.mutations.District.unique()
  
    # Start by preparing the replicate data that we need to plot
    for replicate in data[1].unique():
      # Load the data and prepare the dates
      replicate_data = data[data[1] == replicate]
  
      # Generate a 15 panel plot while looping over the districts that we have spiking data for
      row, col = 0, 0
      for district in districts:
        district_id = self.labels[self.labels.Label == district].ID.values[0]
        axes[row, col].plot(dates, replicate_data[replicate_data[self.DISTRICT] == district_id].frequency)
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
    plt.savefig('out/{}'.format(filename))
    plt.close()
  
  def __districts(self, filename):    
    # Load relevant data, calculate the frequency and the dates
    data = pd.read_csv(filename, header=None)
    dates = data[self.DATES].unique().tolist()
    dates = [datetime.datetime(uganda.MODEL_YEAR, 1, 1) + datetime.timedelta(days=x) for x in dates]

    for mutation, index in self.MAPPING.items():
      # Status update for the user
      print('Processing {} for {}'.format(filename, mutation))    

      # Calculate the frequency based on the current mutation 
      data['frequency'] = data[index] / data[self.INFECTIONS]

      # Load the remainder of the data
      self.labels = pd.read_csv(uganda.DISTRICTS_MAPPING)
      if mutation == 'either':
        self.mutations = pd.read_csv(uganda.MUTATIONS_TEMPLATE.format('675V'))
      else:  
        self.mutations = pd.read_csv(uganda.MUTATIONS_TEMPLATE.format(mutation))
    
      # Set the title, labels, and filename for the results
      title = '{}, {}'.format(self.title, mutation)
      ylabel = '{} Frequency'.format(mutation)
      if mutation == 'either':
        title = '{} / Total ART Resistance'.format(self.title)
        ylabel = 'Total ART Resistance Frequency'
      image_filename = filename.split('/')[-1].replace('uga-policy-', '').replace('.csv', '')
      image_filename += '-{}.png'.format(mutation)

      # Prepare the plot, note the configuration
      self.__plot(data, dates, mutation, ylabel, title, image_filename)


  def process(self, filename, title):
    """Process the dataset in the file and generate three spaghetti plots.
    
    dataset - The full or relative path to the file"""
    self.title = title
    self.__districts(filename)

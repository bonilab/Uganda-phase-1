# dual.py
#
# Include file for the spiking.py script that dual spike plotting class.
import datetime
import matplotlib
import matplotlib.pyplot as plt
import pandas as pd

import include.common as shared

# This class wraps the functions related to plotting dual spike studies and 
# the spike calibration / validation studies.
class dual_spike:
  # Note the assumed model year
  MODEL_YEAR = 2004

  mutations = None
  labels = None
  
  def __plot(self, replicates, mutation, ylabel, title, footer, filename):
    DATES, DISTRICT, INFECTIONS = 2, 3, 4
    MAPPING = { '469Y' : 8, '675V' : 11, 'either' : 14 }

    def add_points():
      row, col = 0, 0
      for district in districts:
        plt.sca(axes[row, col])
        for index, data_row in self.mutations[self.mutations.District == district].iterrows():
          x = datetime.datetime(data_row.Year, 9, 30)
          y = data_row.Frequency
          plt.scatter(x, y, color = 'black', s = 100, zorder = 99)
        row, col = shared.increment(row, col)

    # Note the correct index of the weighted values
    weighted = MAPPING[mutation]
  
    # Setup to generate the plot
    matplotlib.rc_file('../Scripts/matplotlibrc-line')
    figure, axes = plt.subplots(3, 5)
    figure.suptitle(title, y = 0.94)
    figure.text(0.5 - (len(footer) / 400), 0.04, footer, size='small')
    
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
          
    # Next, add the known data points to the plots unless we are plotting the total resistance
    if mutation != 'either': add_points()
          
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
    if mutation == 'either':
      self.mutations = pd.read_csv(shared.MUTATIONS_TEMPLATE.format('675V'))
    else:  
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
        ylabel = '{} Frequency'.format(mutation)
        if mutation == 'either':
          title = 'Total ART Resistance'
          ylabel = 'Total ART Resistance Frequency'
        footer = '{}, n = {}'.format(row[FILENAME], len(replicates))
        filename = 'uga-spike-{}-{}.png'.format(row[CONFIGURATION], mutation)

        # Prepare the plot, note the configuration
        self.__plot(replicates, mutation, ylabel, title, footer, filename)
        configurations.append(row[CONFIGURATION])
        shared.progressBar(index, len(data))
      except Exception as ex:
          print('\nError plotting replicate {}, configuration {}'.format(row[REPLICATE], row[FILENAME]))
          print(ex)
    shared.progressBar(len(data), len(data))    

  def process(self):
    self.__process('469Y')
    self.__process('675V')
    self.__process('either')
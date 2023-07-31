# calibration.py
#
# Include file for the spiking.py script that defines the calibration class.
import datetime
import matplotlib
import matplotlib.pyplot as plt
import pandas as pd

import include.spike.common as shared

# This class warps the functions related to plotting calibration studies.
class calibration:
  def __plot(self, replicate, title, labels, mutations):
    DATES, DISTRICT, INFECTED, WEIGHTED = 2, 3, 4, 8

    def label(region):
      # Filter mutations to the current region, exit if there are none
      filtered = mutations[(mutations.MisRegion == region) & (mutations.Frequency != 0)]
      if len(filtered) == 0: return
      
      # Add a labeled point for each year and frequency combination
      plt.sca(axes[row, col])
      for index, data_row in filtered.iterrows():
        x = datetime.datetime(data_row.Year, 9, 30)
        y = data_row.Frequency
        plt.scatter(x, y, color = 'black', s = 50)
        plt.annotate('{} ({:.3f})'.format(data_row.District, y), (x, y), textcoords = 'offset points', xytext=(0,10), ha='center', fontsize=18)
    
    # Load the spiking data, skip plotting if there is nothing to plot
    data = pd.read_csv('data/spiking/{}.csv'.format(replicate), header = None)
    data['frequency'] = data[WEIGHTED] / data[INFECTED]
    if max(data.frequency) == 0: return
    
    # Finish setting up our data for plotting
    # WARNING The start date is hard coded, this might need to change if re-calibration takes place
    districts = data[DISTRICT].unique().tolist()
    dates = data[DATES].unique().tolist()
    dates = [datetime.datetime(2009, 1, 1) + datetime.timedelta(days=x) for x in dates]  

    # Determine our limits
    ylim = [0, max(max(data.frequency), max(mutations.Frequency))]
    xlim = [min(dates), max(dates)]
      
    # Setup to generate the plot
    matplotlib.rc_file('../Scripts/matplotlibrc-line')
    figure, axes = plt.subplots(3, 5)
    figure.suptitle(title, y = 0.94)
    
    # Generate a 15 panel plot 
    row, col = 0, 0
    for district in districts:
      axes[row, col].plot(dates, data[data[DISTRICT] == district].frequency)
      label(labels[labels.ID == district].Label.values[0])
      axes[row, col].title.set_text(labels[labels.ID == district].Label.values[0])
      
      if row != 2:    
        plt.setp(axes[row, col].get_xticklabels(), visible = False)
      if col != 0:
        plt.setp(axes[row, col].get_yticklabels(), visible = False)

      axes[row, col].xaxis.set_major_formatter(matplotlib.dates.DateFormatter('%y'))
      axes[row, col].set_xlim(xlim)
      axes[row, col].set_ylim(ylim)      
        
      row, col = shared.increment(row, col)
    
    # Apply any final formatting
    plt.setp(axes[2, 0].get_xticklabels()[0], visible = False) 
    plt.sca(axes[1, 0])      
    plt.ylabel('469Y Frequency')
    plt.sca(axes[2, 2])
    plt.xlabel('Model Year')

    # Save the plot
    plt.savefig('plots/{}.png'.format(title))
    plt.close()

  def process(self):
    REPLICATE, STUDYID, FILENAME = 3, 1, 2

    # Load relevant data    
    data = pd.read_csv(shared.REPLICATES_LIST, header = None)
    labels = pd.read_csv(shared.MIS_MAPPING)
    mutations = pd.read_csv(shared.MUTATIONS_469Y)
    
    shared.progressBar(0, len(data))
    for index, row in data.iterrows():
      try:
        # Check to see if this is a calibration configuration
        if row[STUDYID] != 4: continue
        if len(data[data[FILENAME] == row[FILENAME]]) > 1: continue
        
        # Parse out the title components of a calibration filename
        parts = row[FILENAME].split('-')
        title = '{} - {} - {}'.format(parts[2].capitalize(), parts[3], parts[4].replace('.yml', ''))

        # Generate the plot and update the progress bar
        self.__plot(row[REPLICATE], title, labels, mutations)
        shared.progressBar(index, len(data))
      except Exception as ex:
        print('\nError plotting replicate {}, configuration {}'.format(row[REPLICATE], row[FILENAME]))
        print(ex)
    shared.progressBar(len(data), len(data))    
#!/usr/bin/python3

# spiking.py
#
# Load the relevant spike studies from the database.
import argparse
import csv
import datetime
import matplotlib
import matplotlib.dates
import matplotlib.pyplot as plt
import pandas as pd
import os
import sys

# From the PSU-CIDD-MaSim-Support repository
sys.path.insert(1, '../../PSU-CIDD-MaSim-Support/Python/include')
from database import select
from utility import progressBar

# Connection string for the database
CONNECTION = 'host=masimdb.vmhost.psu.edu dbname=uganda user=sim password=sim connect_timeout=60'

# Common start date for all configurations
START_DATE = datetime.datetime(2009, 1, 1)

# Paths for reference data
DISTRICTS_MAPPING = '../GIS/administrative/uga_districts.csv'
MIS_MAPPING = '../GIS/administrative/uga_mis_mapping.csv'
MUTATIONS_469Y = '../GIS/mutations/uga_469y_mutations.csv'

# Paths for the resulting data
PLOTS_DIRECTORY = 'plots'
SPIKING_DIRECTORY = 'data/spiking'

# Path for the replicates data
REPLICATES_LIST = 'data/uga-replicates.csv'

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
    districts = data[DISTRICT].unique().tolist()
    dates = data[DATES].unique().tolist()
    dates = [START_DATE + datetime.timedelta(days=x) for x in dates]  

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
        
      col += 1
      if col % 5 == 0:
        row += 1
        col = 0
    
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
    data = pd.read_csv(REPLICATES_LIST, header = None)
    labels = pd.read_csv(MIS_MAPPING)
    mutations = pd.read_csv(MUTATIONS_469Y)
    
    progressBar(0, len(data))
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
        progressBar(index, len(data))
      except Exception as ex:
        print('\nError plotting replicate {}, configuration {}'.format(row[REPLICATE], row[FILENAME]))
        print(ex)
    progressBar(len(data), len(data))    


# This class wraps the functions related to plotting district spike studies
class district:
  mutations = None
  labels = None
  
  def __plot(self, replicates, title, filename):
    DATES, DISTRICT, INFECTED, WEIGHTED = 2, 3, 4, 8
  
    # Setup to generate the plot
    matplotlib.rc_file('../Scripts/matplotlibrc-line')
    figure, axes = plt.subplots(3, 5)
    figure.suptitle(title, y = 0.94)
    
    # Set a single order for the districts
    districts = self.mutations.District.unique()
  
    # Start by preparing the replicate data that we need to plot
    ymax = 0
    for replicate in replicates:
      # Load the data and prepare the dates
      data = pd.read_csv('data/spiking/{}.csv'.format(replicate), header = None)
      data['frequency'] = data[WEIGHTED] / data[INFECTED]
      ymax = max(ymax, max(data.frequency))
      dates = data[DATES].unique().tolist()
      dates = [START_DATE + datetime.timedelta(days=x) for x in dates]
  
      # Generate a 15 panel plot while looping over the districts that we have 
      # spiking data for
      row, col = 0, 0
      for district in districts:
        district_id = self.labels[self.labels.Label == district].ID.values[0]
        axes[row, col].plot(dates, data[data[DISTRICT] == district_id].frequency)
        axes[row, col].title.set_text(district)
        
        col += 1
        if col % 5 == 0:
          row += 1 
          col = 0
          
    # Next, add the know data points to the plots
    row, col = 0, 0
    for district in districts:
      plt.sca(axes[row, col])
      for index, data_row in self.mutations[self.mutations.District == district].iterrows():
        x = datetime.datetime(data_row.Year, 9, 30)
        y = data_row.Frequency
        plt.scatter(x, y, color = 'black', s = 100, zorder = 99)
        
      col += 1
      if col % 5 == 0:
        row += 1
        col = 0
          
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
    axes[2, 4].set_visible(False)
    plt.setp(axes[2, 0].get_xticklabels()[0], visible = False)
    plt.sca(axes[1, 0])
    plt.ylabel('469Y Frequency')
    plt.sca(axes[2, 2])
    plt.xlabel('Model Year')
    
    # Save the plot
    plt.savefig('plots/{}'.format(filename))
    plt.close()
  
  def process(self):
    CONFIGURATION, REPLICATE, FILENAME = 0, 3, 2
  
    # Load relevant data
    data = pd.read_csv(REPLICATES_LIST, header = None)
    self.labels = pd.read_csv(DISTRICTS_MAPPING)
    self.mutations = pd.read_csv(MUTATIONS_469Y)
  
    configurations = []
    progressBar(0, len(data))
    for index, row in data.iterrows():
      try:
        # Skip if this is not a district calibration
        if len(data[data[FILENAME] == row[FILENAME]]) == 1: continue
        
        # Skip if we have already seen this configuration
        if row[CONFIGURATION] in configurations: continue
  
        # Get the list of replicates associated with this configuration, note that
        # we are assuming that using the configuration id is more reliable to distinguish
        # between configurations than their filename
        replicates = data[data[CONFIGURATION] == row[CONFIGURATION]][REPLICATE]
        self.__plot(replicates, '{} ({}), n = {}'.format(
          row[FILENAME].replace('.yml', ''), row[CONFIGURATION], len(replicates)),
          '{}-{}.png'.format(row[FILENAME].replace('.yml', ''), row[CONFIGURATION]))
        configurations.append(row[CONFIGURATION])
        progressBar(index, len(data))
      except Exception as ex:
          print('\nError plotting replicate {}, configuration {}'.format(row[REPLICATE], row[FILENAME]))
          print(ex)
    progressBar(len(data), len(data))    
  

# This class wraps the functions related to loading replicate data.
class loader:
  # Get the spiking replicates from the database, note the default study is set.
  def __get_replicates(self, studyId = 4):
    sql = """  
        SELECT c.id AS configurationid, 
          c.studyid, 
          c.filename, 
          r.id AS replicateid, 
          r.starttime, 
          r.endtime
        FROM sim.replicate r
          INNER JOIN sim.configuration c ON c.id = r.configurationid
        WHERE c.studyid = 4
        AND r.endtime IS NOT NULL
        ORDER BY c.id desc, c.studyid, c.filename, r.id
    """
    return select(CONNECTION, sql, None)

  # Get the spiking replicate data from the database
  def __get_replicate(self, replicateId):
    sql = """
        SELECT c.id as configurationid, sd.replicateid, sd.dayselapsed,
          sd.district, infectedindividuals,  clinicalepisodes, 
          CASE WHEN gd.occurrences IS NULL THEN 0 else gd.occurrences END AS occurrences,
          CASE WHEN gd.clinicaloccurrences IS NULL THEN 0 else gd.clinicaloccurrences END AS clinicaloccurrences,
          CASE WHEN gd.weightedoccurrences IS NULL THEN 0 else gd.weightedoccurrences END AS weightedoccurrences,
          treatments,
          treatmentfailures,
          genotypecarriers
        FROM (
          SELECT md.replicateid, md.dayselapsed, msd.location AS district,
            sum(msd.infectedindividuals) AS infectedindividuals, 
            sum(msd.clinicalepisodes) AS clinicalepisodes,
            sum(msd.treatments) AS treatments,
            sum(msd.treatmentfailures) as treatmentfailures,
            sum(genotypecarriers) as genotypecarriers
          FROM sim.monthlydata md
            INNER JOIN sim.monthlysitedata msd on msd.monthlydataid = md.id
          WHERE md.replicateid = %(replicateId)s
            AND md.dayselapsed > (7 * 365)
          GROUP BY md.replicateid, md.dayselapsed, msd.location) sd
        LEFT JOIN (
          SELECT md.replicateid, md.dayselapsed, mgd.location AS district,
          sum(mgd.occurrences) AS occurrences,
            sum(mgd.clinicaloccurrences) AS clinicaloccurrences,
            sum(mgd.weightedoccurrences) AS weightedoccurrences
          FROM sim.monthlydata md
            INNER JOIN sim.monthlygenomedata mgd on mgd.monthlydataid = md.id
            INNER JOIN sim.genotype g on g.id = mgd.genomeid
          WHERE md.replicateid = %(replicateId)s
            AND md.dayselapsed > (7 * 365)
            AND g.name ~ '^.....Y.'
          GROUP BY md.replicateid, md.dayselapsed, mgd.location) gd ON (gd.replicateid = sd.replicateid 
            AND gd.dayselapsed = sd.dayselapsed
            AND gd.district = sd.district)
          INNER JOIN sim.replicate r on r.id = sd.replicateid
          INNER JOIN sim.configuration c on c.id = r.configurationid
        WHERE r.endtime is not null
          AND r.id = %(replicateId)s
        ORDER BY replicateid, dayselapsed"""
    return select(CONNECTION, sql, {'replicateId':replicateId})  

  # Process the replicates to make sure we have all of the data we need locally
  def load(self):
    def save_csv(filename, data):
      with open(filename, 'w') as csvfile:
        writer = csv.writer(csvfile)
        for row in data:
          writer.writerow(row)

    print("Querying for replicates list...")
    if not os.path.exists(SPIKING_DIRECTORY): os.makedirs(SPIKING_DIRECTORY)
    replicates = self.__get_replicates()
    save_csv(REPLICATES_LIST, replicates)
    
    print("Processing replicates...")  
    count = 0
    progressBar(count, len(replicates))
    for row in replicates:
      # Check to see if we already have the data
      filename = os.path.join(SPIKING_DIRECTORY, "{}.csv".format(row[3]))
      if os.path.exists(filename): continue

      # Query and store the data
      replicate = self.__get_replicate(row[3])
      save_csv(filename, replicate)

      # Update the progress bar
      count = count + 1
      progressBar(count, len(replicates))

    # Complete progress bar for replicates
    if count != len(replicates): progressBar(len(replicates), len(replicates))

def main(args):
  # Perform any common setup
  if not os.path.exists(PLOTS_DIRECTORY): os.makedirs(PLOTS_DIRECTORY)

  # Everything goes through the same loader
  loader().load()

  # Hand things off to the correct processing
  if args.type == 'c':
    calibration().process()
  elif args.type == 'd':
    district().process()
  else:
    print('Unknown type parameter, {}'.format(args.type))
     

if __name__ == '__main__':
  # Parse the parameters and defer to the main function
  parser = argparse.ArgumentParser()
  parser.add_argument('-t', action='store', dest='type', required=True,
    help='The type of plots to generate, c for calibration or d for district')
  main(parser.parse_args())

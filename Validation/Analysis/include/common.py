# spike_common.py
#
# Include file for the shared data between the spiking scripts.
import csv
import sys

# Connection string for the database
CONNECTION = 'host=masimdb.vmhost.psu.edu dbname=uganda user=sim password=sim connect_timeout=60'

# Paths for reference data
DISTRICTS_MAPPING = '../GIS/administrative/uga_districts.csv'
MIS_MAPPING = '../GIS/administrative/uga_mis_mapping.csv'

# Template and paths for the mutations
MUTATIONS_TEMPLATE = '../GIS/mutations/uga_{}_mutations.csv'
MUTATIONS_469Y = '../GIS/mutations/uga_469y_mutations.csv'
MUTATIONS_675V = '../GIS/mutations/uga_675v_mutations.csv'

# Path for the replicates data
DEFAULT_REPLICATE_STUDY = 4
REPLICATES_LIST = 'data/uga-replicates.csv'

# Paths for the resulting data
PLOTS_DIRECTORY = 'plots'
SPIKING_DIRECTORY = 'data/spiking'

# From the PSU-CIDD-MaSim-Support repository, relative to the base script
sys.path.insert(1, '../../PSU-CIDD-MaSim-Support/Python/include')
from database import select
from utility import progressBar

def increment(row, col):
  col += 1
  if col % 5 == 0:
    row += 1
    col = 0
  return row, col

def save_csv(filename, data):
  with open(filename, 'w') as csvfile:
    writer = csv.writer(csvfile)
    for row in data:
      writer.writerow(row)
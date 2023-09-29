# uganda.py
#
# This file contains common properties for Uganda and associated reporting and plots.

# Connection string for the database
CONNECTION = 'host=masimdb.vmhost.psu.edu dbname=uganda user=sim password=sim connect_timeout=60'

# The mapping file for the districts
DISTRICTS_MAPPING = '../GIS/administrative/uga_districts.csv'

# First year of model execution
MODEL_YEAR = 2004

# Template and paths for the mutations
MUTATIONS_TEMPLATE = '../GIS/mutations/uga_{}_mutations.csv'
MUTATIONS_469Y = '../GIS/mutations/uga_469y_mutations.csv'
MUTATIONS_675V = '../GIS/mutations/uga_675v_mutations.csv'

# Settings for plots
LINE_CONFIGURATION = 'include/matplotlibrc-line'
VIOLIN_CONFIGURATION = 'include/matplotlibrc-violin'
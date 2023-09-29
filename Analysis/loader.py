#!/usr/bin/python3

# loader.py
# 
# Load the relevant data from the database.
import os
import pandas as pd
import sys

import include.common as shared

# From the PSU-CIDD-MaSim-Support repository
sys.path.insert(1, '../../PSU-CIDD-MaSim-Support/Python/include')
from database import select
from utility import progressBar

# Various script constants
DATASET_DIRECTORY = 'data/datasets'
REPLICATE_DIRECTORY = 'data/replicates'
REPLICATES_LIST = 'data/uga-loader-replicates.csv'


def get_replicates(studyId):
    sql = """
      SELECT c.id AS configurationid, 
        c.studyid, 
        c.filename, 
        r.id AS replicateid, 
        r.starttime, 
        r.endtime
      FROM sim.replicate r
        INNER JOIN sim.configuration c ON c.id = r.configurationid
      WHERE r.endtime IS NOT NULL
        AND c.studyid = 5
      ORDER BY c.id desc, c.studyid, c.filename, r.id"""
    return select(shared.CONNECTION, sql, {'studyId':studyId})

def get_replicate(replicateId):
    sql = """
      SELECT *, 
        (weightedoccurrences_469y + weightedoccurrences_675v) as weightedsum,
        (occurrences_469Y + occurrences_675v) as occurrences_sum
      FROM (
        SELECT c.id as configurationid, sd.replicateid, sd.dayselapsed,
          sd.district, infectedindividuals,  clinicalepisodes, 
          CASE WHEN y_mutant.occurrences IS NULL THEN 0 else y_mutant.occurrences END AS occurrences_469y,
          CASE WHEN y_mutant.clinicaloccurrences IS NULL THEN 0 else y_mutant.clinicaloccurrences END AS clinicaloccurrences_469y,
          CASE WHEN y_mutant.weightedoccurrences IS NULL THEN 0 else y_mutant.weightedoccurrences END AS weightedoccurrences_469y,
          CASE WHEN v_mutant.occurrences IS NULL THEN 0 else v_mutant.occurrences END AS occurrences_675v,
          CASE WHEN v_mutant.clinicaloccurrences IS NULL THEN 0 else v_mutant.clinicaloccurrences END AS clinicaloccurrences_675v,
          CASE WHEN v_mutant.weightedoccurrences IS NULL THEN 0 else v_mutant.weightedoccurrences END AS weightedoccurrences_675v,		  
          treatments,
          treatmentfailures
        FROM (
          SELECT md.replicateid, md.dayselapsed, msd.location AS district,
            sum(msd.infectedindividuals) AS infectedindividuals, 
            sum(msd.clinicalepisodes) AS clinicalepisodes,
            sum(msd.treatments) AS treatments,
            sum(msd.treatmentfailures) as treatmentfailures
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
            AND g.name ~ '^.....Y..'
          GROUP BY md.replicateid, md.dayselapsed, mgd.location) y_mutant ON (y_mutant.replicateid = sd.replicateid 
            AND y_mutant.dayselapsed = sd.dayselapsed
            AND y_mutant.district = sd.district)
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
            AND g.name ~ '^......V.'
          GROUP BY md.replicateid, md.dayselapsed, mgd.location) v_mutant ON (v_mutant.replicateid = sd.replicateid 
            AND v_mutant.dayselapsed = sd.dayselapsed
            AND v_mutant.district = sd.district)			
          INNER JOIN sim.replicate r on r.id = sd.replicateid
          INNER JOIN sim.configuration c on c.id = r.configurationid
        WHERE r.endtime is not null
          AND r.id = %(replicateId)s) iq
      ORDER BY replicateid, dayselapsed"""
    return select(shared.CONNECTION, sql, {'replicateId':replicateId})


def main(studyId):
    # Make the relevant directories
    os.makedirs(DATASET_DIRECTORY, exist_ok=True)
    os.makedirs(REPLICATE_DIRECTORY, exist_ok=True)

    # Query for the completed replicates
    replicates = get_replicates(5)
    shared.save_csv(REPLICATES_LIST, replicates)

    # Query for the replicates
    count = 0
    progressBar(count, len(replicates))
    for row in replicates:
        # Check for the data
        id = row[3]
        filename = os.path.join(REPLICATE_DIRECTORY, '{}.csv'.format(id))
        if os.path.exists(filename): continue

        # Query and store the replicate
        replicate = get_replicate(id)
        shared.save_csv(filename, replicate)

        # Update the status
        count += 1
        progressBar(count, len(replicates))

    # Complete progress bar for replicates
    if count != len(replicates): progressBar(len(replicates), len(replicates))
    
    # Merge the replicates into their data sets
    replicates = pd.read_csv(REPLICATES_LIST, header=None)
    for configuration in replicates[2].unique():
        print('Merging {}...'.format(configuration.replace('.yml', '')))
        configuration_replicates = replicates[replicates[2] == configuration]
        filename = os.path.join(DATASET_DIRECTORY, configuration.replace('yml', 'csv'))
        merge_data(configuration_replicates[3].to_list(), REPLICATE_DIRECTORY, filename)


def merge_data(replicates, path, outfile):
  # Let the user know we haven't hung
  count = 0
  progressBar(count, len(replicates))

  # Read the first file so we have something to append to
  infile = os.path.join(path, "{}.csv".format(replicates[0]))
  data = pd.read_csv(infile, header=None)

  for replicate in replicates[1:]:
    # Load the file from the disk
    infile = os.path.join(path, "{}.csv".format(replicate))
    working = pd.read_csv(infile, header=None)
    data = data.append(working)

    # Update the status
    count += 1
    progressBar(count, len(replicates))
    
  # Save and produce the final status bar
  data.to_csv(outfile, header=None, index=False)
  if count != len(replicates): progressBar(len(replicates), len(replicates))



if __name__ == '__main__':
    main(5)
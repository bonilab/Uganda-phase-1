# spike_loader,py
#
# Include file for spiking.py script that defines the loader class.
#
# NOTE that this was upgraded from only returning a single mutant in the uga_calibration
# NOTE database. So we assume that we are pointing at the correct database when running.
import csv
import os

import include.spike.common as shared

# This class wraps the functions related to loading replicate data.
class loader:
  # Get the spiking replicates from the database, note the default study is set.
  def __get_replicates(self, studyId = shared.DEFAULT_REPLICATE_STUDY):
    sql = """  
        SELECT c.id AS configurationid, 
          c.studyid, 
          c.filename, 
          r.id AS replicateid, 
          r.starttime, 
          r.endtime
        FROM sim.replicate r
          INNER JOIN sim.configuration c ON c.id = r.configurationid
        WHERE c.studyid = 3
        AND r.endtime IS NOT NULL
        ORDER BY c.id desc, c.studyid, c.filename, r.id
    """
    return shared.select(shared.CONNECTION, sql, None)

  # Get the spiking replicate data from the database
  def __get_replicate_single(self, replicateId):
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
    return shared.select(shared.CONNECTION, sql, {'replicateId':replicateId})  

  # Process the replicates to make sure we have all of the data we need locally
  def load(self):
    def save_csv(filename, data):
      with open(filename, 'w') as csvfile:
        writer = csv.writer(csvfile)
        for row in data:
          writer.writerow(row)

    print("Querying for replicates list...")
    if not os.path.exists(shared.SPIKING_DIRECTORY): os.makedirs(shared.SPIKING_DIRECTORY)
    replicates = self.__get_replicates()
    save_csv(shared.REPLICATES_LIST, replicates)
    
    print("Processing replicates...")  
    count = 0
    shared.progressBar(count, len(replicates))
    for row in replicates:
      # Check to see if we already have the data
      filename = os.path.join(shared.SPIKING_DIRECTORY, "{}.csv".format(row[3]))
      if os.path.exists(filename): continue

      # Query and store the data
      replicate = self.__get_replicate_single(row[3])
      save_csv(filename, replicate)

      # Update the progress bar
      count = count + 1
      shared.progressBar(count, len(replicates))

    # Complete progress bar for replicates
    if count != len(replicates): shared.progressBar(len(replicates), len(replicates))
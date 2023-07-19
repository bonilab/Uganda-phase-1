# spike_loader,py
#
# Include file for spiking.py script that defines the loader class
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
        WHERE c.studyid = 4
        AND r.endtime IS NOT NULL
        ORDER BY c.id desc, c.studyid, c.filename, r.id
    """
    return shared.select(shared.CONNECTION, sql, None)

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
      replicate = self.__get_replicate(row[3])
      save_csv(filename, replicate)

      # Update the progress bar
      count = count + 1
      shared.progressBar(count, len(replicates))

    # Complete progress bar for replicates
    if count != len(replicates): shared.progressBar(len(replicates), len(replicates))
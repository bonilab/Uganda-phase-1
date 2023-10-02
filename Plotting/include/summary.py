# summary.py
#
# This class wraps the functions related to generating the summary data.
import datetime
import os
import numpy as np

import include.uganda as uganda

class summary:
  def generate(self):
    # Load the data, time spans, and note the date
    data, dates = uganda.load_all_datasets()
    ranges, points = self.__time_span(dates)
    
    # Calculate the metrics
    self.__treatment_failures(data, ranges)
    self.__frequency(data, points)

  
  # Calculate the frequency for each genotype and save the results
  def __frequency(self, data, points):
    # Pre-compute the date string for the annual endpoints
    date = datetime.datetime(uganda.MODEL_YEAR, 1, 1)
    date_string, check_string = ',', 'record source,'
    for point in points:
      date_string += '{:%Y},'.format(date + datetime.timedelta(days=int(point)))
      check_string += '\'{:%Y/%m},'.format(date + datetime.timedelta(days=int(point)))

    # Iterate on each of the mutations
    for mutation in uganda.DATASET_LAYOUT['mutations']:
      results = self.__prepare()
      for key in uganda.LABELS.keys():       
        for point in points:
          # Calculate the frequency
          data[key]['frequency'] = data[key][mutation] / data[key].infections
          row = data[key][data[key].days == point].frequency

          # Append the results
          upper = np.percentile(row, 75)
          median = np.percentile(row, 50)
          lower = np.percentile(row, 25)
          results[key] += '{:.2f} ({:.2f} - {:.2f}),'.format(median, lower, upper)

      # Save the results
      os.makedirs('out', exist_ok=True)
      with open(os.path.join('out', '{}.csv'.format(mutation)), 'w') as out:
        out.write(date_string + '\n')
        for key, format in uganda.LABELS.items():
          out.write('{},{}\n'.format(format[0], results[key]))
        out.write('\n' + check_string + '\n')


  # Internal helper function to prepare dictionaries
  def __prepare(self):
    structure = {}
    for key in uganda.LABELS.keys(): structure[key] = ''
    return structure


  # Internal function to calculate the time spans, returns the 12-month ranges
  # and the point date to use for calculations.
  def __time_span(self, dates):
    ranges, points = [], []
    for ndx in range(1, 16):
      # Define the date frame for the results
      start = -12 * ndx
      end = -12 * (ndx - 1)
      if ndx == 1: end = None

      # Append them to the list
      ranges.append(dates[start:end])
      points.append(dates[start:end][-1])

    # Reverse the final lists and return
    ranges.reverse()
    points.reverse()
    return ranges, points     


  # Calculate the treatment failures and save the results
  def __treatment_failures(self, data, ranges):
    # Pre-compute the check date
    date = datetime.datetime(uganda.MODEL_YEAR, 1, 1)

    # Calculate the median, IQR for all of the treatment failures
    date_string, check_string = ',', 'record range,'
    treatment_failures = self.__prepare()
    for time_span in ranges:
      results = self.__get_treatment_failures(data, time_span)
      for key in uganda.LABELS.keys():
        upper = np.percentile(results[key], 75)
        median = np.percentile(results[key], 50)
        lower = np.percentile(results[key], 25)
        treatment_failures[key] += '{:.2f} ({:.2f} - {:.2f}),'.format(median, lower, upper)

      # Append the date for the results, and the range used to calculate them
      date_string += '{:%Y},'.format(date + datetime.timedelta(days=int(time_span[0])))
      check_string += '{:%Y/%m}-{:%Y/%m},'.format(
        date + datetime.timedelta(days=int(time_span[0])), 
        date + datetime.timedelta(days=int(time_span[-1])))
      
    # Save the results
    os.makedirs('out', exist_ok=True)
    with open('out/treatment_failures.csv', 'w') as out:
      out.write(date_string + '\n')
      for key, format in uganda.LABELS.items():
        out.write('{},{}\n'.format(format[0], treatment_failures[key]))
      out.write('\n' + check_string + '\n')


  # Get the treatment failures for the defined time frame
  def __get_treatment_failures(self, data, dates):
    # Start by generating the data to plot
    results = {}
    for key in uganda.LABELS.keys():
      row = []
      for replicate in data[key].replicate.unique():
        # Filter on the dataset to get this replicate
        subset = data[key][data[key].replicate == replicate]

        # Filter on the replicate to get sum of treatments, failures
        treatments = np.sum(subset[(subset.days >= dates[0]) & (subset.days <= dates[-1])].treatments)
        failures = np.sum(subset[(subset.days >= dates[0]) & (subset.days <= dates[-1])].failures)

        # Treatment failures is returned as percentage
        row.append((failures / treatments) * 100.0)
      results[key] = row
    return results    
      
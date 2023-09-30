# violin.py
#
# This class wraps the functions related to plotting the frequency and treatment failure violin plots.
import datetime
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
import seaborn as sb

import include.uganda as uganda

class violin:
  ENDPOINTS = {
    # Endpoint : First Date Offset, Last Date Offset, Numeric Value
    'Three' : [-96, -84, 3],
    'Five'  : [-72, -60, 5],
    'Ten'   : [-12, None, 10]
  }

  LABELS = {
    'status-quo'            : ['Status Quo', '#bdd7e7'],
    'sft-al'                : ['AL', '#6baed6'],
    'sft-asaq'              : ['ASAQ', '#6baed6'],
    'sft-dhappq'            : ['DHA-PPQ', '#6baed6'],
    'mft-al-25-asaq-75'     : ['AL (25%) + ASAQ (75%)', '#bae4b3'],
    'mft-al-50-asaq-50'     : ['AL (50%) + ASAQ (50%)', '#bae4b3'],
    'mft-al-75-asaq-25'     : ['AL (75%) + ASAQ (25%)', '#bae4b3'],
    'mft-al-75-dhappq-25'   : ['AL (75%) + DHA-PPQ (25%)', '#74c476'],
    'mft-asaq-75-dhappq-25' : ['ASAQ (75%) + DHA-PPQ (25%)', '#31a354'],
    'tact-alaq'             : ['ALAQ', '#df65b0'],
    'tact-asmqppq'          : ['ASMQ-PPQ', '#df65b0']
  }

  def treatment_failures(self):
    """Generate the 3, 5, and 10 year endpoint treatment failure violin plots"""
    data, dates = uganda.load_all_datasets()

    print('Preparing treatment failure plots...')
    for endpoint, bounds in self.ENDPOINTS.items():
      range = dates[bounds[0]:bounds[1]]
      
      # Offer some validation that this the correct bounds  
      date = datetime.datetime(uganda.MODEL_YEAR, 1, 1)
      print('{} Year: {:%Y-%m} to {:%Y-%m}'.format(endpoint,
        date + datetime.timedelta(days=int(range[0])), 
        date + datetime.timedelta(days=int(range[-1]))))

      # Prepare the actual plot
      filename = 'treatment-failures-{}-year.png'.format(bounds[2])
      self.__plot_failures(data, range, filename)


  def __plot_failures(self, data, dates, filename):

    # Start by generating the data to plot
    records, labels, colors = [], [], []
    for key, format in self.LABELS.items():
      row = []
      for replicate in data[key].replicate.unique():
        # Filter on the dataset to get this replicate
        subset = data[key][data[key].replicate == replicate]

        # Filter on the replicate to get sum of treatments, failures
        treatments = np.sum(subset[(subset.days >= dates[0]) & (subset.days <= dates[-1])].treatments)
        failures = np.sum(subset[(subset.days >= dates[0]) & (subset.days <= dates[-1])].failures)

        # Treatment failures is returned as percentage
        row.append((failures / treatments) * 100.0)

      # Append the processed data
      labels.append(format[0])
      colors.append(format[1])
      records.append(row)

    # Generate the plot
    matplotlib.rc_file(uganda.VIOLIN_CONFIGURATION)
    figure, axis = plt.subplots()
    violin = sb.violinplot(data=records, palette=colors, cut=0, scale='width', inner=None, linewidth=0.5, orient='h')   
    sb.boxplot(data=records, palette=colors, width=0.2, boxprops={'zorder' : 2}, orient='h')
    sb.despine(top=True, right=True, left=True, bottom=True)

    # Scale the alpha channel for the violin plot
    for item in violin.collections: item.set_alpha(0.5)

    # Format the plot for the data
    axis.set_yticklabels(labels)
    axis.xaxis.set_major_formatter(ticker.PercentFormatter())    
    axis.set_xlabel('Percent Treatment Failures')
              
    # Save the plot
    plt.savefig('out/{}'.format(filename))
    plt.close()
  

  def frequencies(self):
    """Generate the 3, 5, and 10 year endpoint frequency plots"""
    data, dates = uganda.load_all_datasets()
    
    for allele in ['469Y', '675V', 'either']:
      print('Generating {} allele plots...'.format(allele))
      for endpoint, bounds in self.ENDPOINTS.items():
        range = dates[bounds[0]:bounds[1]]
              
        # Offer some validation that this the correct bounds  
        date = datetime.datetime(uganda.MODEL_YEAR, 1, 1)
        print('{} Year: {:%Y-%m}'.format(endpoint, date + datetime.timedelta(days=int(range[-1]))))

        # Prepare the actual plot
        filename = 'frequency-{}-{}-year.png'.format(allele, bounds[2])
        self.__plot_frequencies(data, allele, range[-1], filename)


  def __plot_frequencies(self, data, allele, date, filename):

    # Start by generating the data to plot
    records, labels, colors = [], [], []
    for key, format in self.LABELS.items():
      data[key]['frequency'] =  data[key][allele] / data[key].infections
      row = data[key][data[key].days == date].frequency
      
      # Append the processed data
      labels.append(format[0])
      colors.append(format[1])
      records.append(row)

    # Generate the plot
    matplotlib.rc_file(uganda.VIOLIN_CONFIGURATION)
    figure, axis = plt.subplots()
    violin = sb.violinplot(data=records, palette=colors, cut=0, scale='width', inner=None, linewidth=0.5, orient='h')   
    sb.boxplot(data=records, palette=colors, width=0.2, boxprops={'zorder' : 2}, orient='h')
    sb.despine(top=True, right=True, left=True, bottom=True)

    # Scale the alpha channel for the violin plot
    for item in violin.collections: item.set_alpha(0.5)

    # Format the plot for the data
    axis.set_yticklabels(labels)
    axis.set_xlabel('{} allele frequency'.format(allele))
    if allele == 'either':
          axis.set_xlabel('ART-R alleles frequency'.format(allele))

    # Save the plot
    plt.savefig('out/{}'.format(filename))
    plt.close()
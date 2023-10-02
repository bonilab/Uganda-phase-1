#!/usr/bin/python3

# plot_astmh.py
#
# Plot the calibration and violin plots for the ASTMH poster.
import os

from include.spaghetti import spaghetti
from include.summary import summary
from include.median import median
from include.violin import violin


def make_plots(plot):
  plot.process('../Analysis/data/datasets/uga-policy-status-quo.csv', 'Status Quo')
  
  plot.process('../Analysis/data/datasets/uga-policy-mft-al-25-asaq-75.csv', 'AL (25%) + ASAQ (75%)')
  plot.process('../Analysis/data/datasets/uga-policy-mft-al-50-asaq-50.csv', 'AL (50%) + ASAQ (50%)')
  plot.process('../Analysis/data/datasets/uga-policy-mft-al-75-asaq-25.csv', 'AL (75%) + ASAQ (25%)')
  plot.process('../Analysis/data/datasets/uga-policy-mft-al-75-dhappq-25.csv', 'AL (75%) + DHA-PPQ (25%)')
  plot.process('../Analysis/data/datasets/uga-policy-mft-asaq-75-dhappq-25.csv', 'ASAQ (75%) + DHA-PPQ (25%)')
  
  plot.process('../Analysis/data/datasets/uga-policy-sft-al.csv', 'AL')
  plot.process('../Analysis/data/datasets/uga-policy-sft-asaq.csv', 'ASAQ')
  plot.process('../Analysis/data/datasets/uga-policy-sft-dhappq.csv', 'DHA-PPQ')
  
  plot.process('../Analysis/data/datasets/uga-policy-tact-alaq.csv', 'ALAQ')
  plot.process('../Analysis/data/datasets/uga-policy-tact-asmqppq.csv', 'ASMQ-PPQ')
  
if __name__ == '__main__':
  make_plots(spaghetti())
  make_plots(median())
  violin().treatment_failures()
  violin().frequencies()
  summary().generate()

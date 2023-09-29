#!/usr/bin/python3

# plot_astmh.py
#
# Plot the calibration and violin plots for the ASTMH poster.
import os

from include.spaghetti import spaghetti 

def make_plots(plot):
  plot.process('../Analysis/data/datasets/uga-policy-status-quo.csv', 'Status Quo')
  
  plot.process('../Analysis/data/datasets/uga-policy-mft-al-25-asaq-75.csv', 'MFT: AL 25% / ASAQ 75%')
  plot.process('../Analysis/data/datasets/uga-policy-mft-al-50-asaq-50.csv', 'MFT: AL 50% / ASAQ 50%')
  plot.process('../Analysis/data/datasets/uga-policy-mft-al-75-asaq-25.csv', 'MFT : AL 75% / ASAQ 25%')
  plot.process('../Analysis/data/datasets/uga-policy-mft-al-75-dhappq-25.csv', 'MFT: AL 75% / DHA-PPQ 25%')
  plot.process('../Analysis/data/datasets/uga-policy-mft-asaq-75-dhappq-25.csv', 'MFT: ASAQ 75% / DHA-PPQ 25%')
  
  plot.process('../Analysis/data/datasets/uga-policy-sft-al.csv', 'AL Only')
  plot.process('../Analysis/data/datasets/uga-policy-sft-asaq.csv', 'ASAQ Only')
  plot.process('../Analysis/data/datasets/uga-policy-sft-dhappq.csv', 'DHA-PPQ Only')
  
  plot.process('../Analysis/data/datasets/uga-policy-tact-alaq.csv', 'ALAQ Only')
  plot.process('../Analysis/data/datasets/uga-policy-tact-asmqppq.csv', 'ASMQ-PPQ Only')
  
if __name__ == '__main__':
  os.makedirs('out', exist_ok=True)
  make_plots(spaghetti())
  
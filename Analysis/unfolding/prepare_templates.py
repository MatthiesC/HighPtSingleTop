from __future__ import print_function

import uproot

import os
import sys
import argparse

sys.path.append(os.path.join(os.environ.get('CMSSW_BASE'), 'src/UHH2/LegacyTopTagging/Analysis'))
from parallel_threading import run_with_pool

inputDirBase = os.environ.get('CMSSW_BASE')+'src/UHH2/HighPtSingleTop/output/Analysis/mainsel/{year}/{channel}/{syst_dir}/hadded/'
outputDirBase = os.environ.get('CMSSW_BASE')+'src/UHH2/HighPtSingleTop/output/Analysis/mainsel/{year}/{channel}/combine'

class TemplateCreator():
    def __init__(self, years):
        pass

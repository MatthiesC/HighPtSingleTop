from unfolding_constants import unfoldingConstants as UC

import os
import sys
import argparse

import uproot
# import awkward as ak
import numpy as np
from hist import Hist
import hist

# sys.path.append(os.path.join(os.environ.get('CMSSW_BASE'), 'src/UHH2/LegacyTopTagging/Analysis'))
# from constants import _YEARS
# from parallel_threading import run_with_pool

all_years = ['UL16preVFP', 'UL16postVFP', 'UL17', 'UL18']
all_channels = ['ele', 'muo']
relevant_regions = ['unfold', '1t', '0t1W', '1b1t', '2b1t', '1b0t1W', '2b0t1W']

if not sys.argv[1:]: sys.exit('No arguments provided. Exit.')
parser = argparse.ArgumentParser()
parser.add_argument('-y', '--years', nargs='+', default=all_years)
parser.add_argument('-c', '--channels', nargs='+', default=all_channels)
parser.add_argument('-r', '--regions', nargs='+', choices=UC.Regions.keys(), default=relevant_regions)
parser.add_argument('-l', '--genlevel', choices=['Matrix', 'Parton', 'Particle'])
parser.add_argument('-d', '--drds', choices=['DR', 'DS'], default='DR')
parser.add_argument('--runii', action='store_true')
args = parser.parse_args(sys.argv[1:])

if args.runii:
    args.years = ['run2']

vois = UC.VOIs
gen_level = args.genlevel
rec_level = 'Reco'
branch_weight = 'weight'

for year in args.years:

    for channel in args.channels:

        for region in args.regions:

            inputDir = os.path.join(os.environ.get('CMSSW_BASE'), 'src/UHH2/HighPtSingleTop/output/Analysis/mainsel', year, channel, 'nominal/hadded/')
            inputFilePath = os.path.join(inputDir, 'uhh2.AnalysisModuleRunner.MC.ST_tW_'+args.drds+'.root')
            file = uproot.open(inputFilePath)
            tree = file['AnalysisTree']

            for voi_k, voi_v in vois.items():

                cut = UC.Regions[region].cut_rule
                voi_gen = 'gen_'+voi_k
                voi_rec = 'rec_'+voi_k
                aliases = {
                    voi_gen: voi_v.get_alias(gen_level),
                    voi_rec: voi_v.get_alias(rec_level),
                }
                arrays = tree.arrays([branch_weight, voi_gen, voi_rec], cut=cut, aliases=aliases)

                label_gen = voi_v.get_axis_label(gen_level)
                label_rec = voi_v.get_axis_label(rec_level)

                hist2d = Hist(
                    hist.axis.Variable(
                        voi_v.bins, label=label_gen, name=voi_gen, underflow=True, overflow=True
                    ),
                    hist.axis.Variable(
                        voi_v.bins, label=label_rec, name=voi_rec, underflow=True, overflow=True
                    ),
                    storage=hist.storage.Weight(), # like ROOT's Sumw2()
                )

                gen_array = arrays[voi_gen]
                rec_array = arrays[voi_rec]

                hist2d.fill(gen_array, rec_array, weight=arrays[branch_weight])

                outputDir = os.path.join(os.environ.get('CMSSW_BASE'), 'src/UHH2/HighPtSingleTop/output/Analysis/mainsel', year, channel, 'unfolding/migration_matrices/')
                os.system('mkdir -p '+outputDir)

                outfilePath = os.path.join(outputDir, 'migration_matrix__'+args.drds+'__'+voi_k+'__'+gen_level+'__region_'+region+'__'+year+'_'+channel+'.root')
                with uproot.recreate(outfilePath) as outfile:
                    outfile['migration_matrix'] = hist2d
                    outfile['axis_label_gen'] = label_gen
                    outfile['axis_label_rec'] = label_rec
                print('Wrote', outfilePath)

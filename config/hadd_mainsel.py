import os
import sys
import argparse
import glob
import numpy as np
import ROOT as root
import subprocess
import multiprocessing

sys.path.append(os.path.join(os.environ.get('CMSSW_BASE'), 'src/UHH2/LegacyTopTagging/Analysis'))
from constants import _JECSMEAR_SOURCES

years = [
'UL16preVFP',
'UL16postVFP',
'UL17',
'UL18',
]

channels = [
'muo',
'ele',
]

possible_systs = [
'nominal',

# 'jes_up',
# 'jes_down',
'jesTotal_up',
'jesTotal_down',
'jer_up',
'jer_down',
'hdamp_up',
'hdamp_down',
'tune_up',
'tune_down',
'mtop_mtop171p5',
'mtop_mtop173p5',
'cr_cr1',
'cr_cr2',
'cr_erdon',

# 'btagging_down_bc',
# 'btagging_down_udsg',
# 'btagging_up_bc',
# 'btagging_up_udsg',
# 'jec_down',
# 'jec_up',
# 'jer_down',
# 'jer_up',
# 'muf_down',
# 'muf_up',
# 'muonid_down',
# 'muonid_up',
# 'muontrigger_down',
# 'muontrigger_up',
# 'mur_down',
# 'mur_up',
# 'murmuf_down',
# 'murmuf_up',
# 'pileup_down',
# 'pileup_up',
# 'ps_FSRdown_2',
# 'ps_FSRup_2',
# 'ps_ISRdown_2',
# 'ps_ISRup_2',
# 'wp_down',
# 'wp_up',
# 'tau21_down',
# 'tau21_up',
# 'toppt_a_up',
# 'toppt_a_down',
# 'toppt_b_up',
# 'toppt_b_down',
]

# for k in _JECSMEAR_SOURCES.keys():
#     for x in ['up', 'down']:
#         possible_systs.append('jes'+k+'_'+x)


dict_sourceFiles = {
    # ### TTbar
    'TTbar': [
        'TTbar*',
    ],
    # ### Single Top
    'ST_otherChannels': [
        'ST_tChannel*',
        'ST_sChannel*',
    ],
    'ST_tChannel': [
        'ST_tChannel*',
    ],
    'ST_sChannel': [
        'ST_sChannel*',
    ],
    'ST_tW_DR': [
        'ST_tW_DR_*_dnn*',
    ],
    'ST_tW_DR_dnnSig': [
        'ST_tW_DR_*_dnnSig_*',
    ],
    'ST_tW_DR_dnnBkg': [
        'ST_tW_DR_*_dnnBkg_*',
    ],
    'ST_tW_DS': [
        'ST_tW_DS_*_dnn*',
    ],
    'ST_tW_DS_dnnSig': [
        'ST_tW_DS_*_dnnSig_*',
    ],
    'ST_tW_DS_dnnBkg': [
        'ST_tW_DS_*_dnnBkg_*',
    ],
    # ### Other
    'WJetsToLNu': [
        'WJetsToLNu_HT*',
    ],
    'DYJetsToLL': [
        'DYJetsToLL_HT*',
    ],
    'Diboson': [
        'Diboson*',
    ],
    # 'DYJetsToLLAndDiboson': [
    #     'DYJetsToLL_HT*',
    #     'Diboson*',
    # ],
    'VJetsAndVV': [
        'WJetsToLNu_HT*',
        'DYJetsToLL_HT*',
        'Diboson*',
    ],
    'QCD': [
        'QCD_Mu*',
        'QCD_bcToE*',
        'QCD_EM*',
    ],
    # # 'nonQCD': [
    # #     'TTbar*__AllMergeScenarios',
    # #     'ST*__AllMergeScenarios',
    # #     'WJetsToLNu_HT*',
    # #     'DYJetsToLL_HT*',
    # #     'Diboson*',
    # # ],
    'DATA': [
        'DATA*',
    ],
}

fileNamePrefix = 'uhh2.AnalysisModuleRunner.'

parser = argparse.ArgumentParser()
parser.add_argument('-y', '--years', choices=years, nargs='+', default=years)
parser.add_argument('-c', '--channels', choices=channels, nargs='+', default=channels)
parser.add_argument('-s', '--systs', choices=possible_systs, nargs='+', default=['nominal'])
parser.add_argument('--all', action='store_true', help='Instead of defining the syst directories via --syst, you can hadd all of them in one go.')
parser.add_argument('-t', '--targets', choices=dict_sourceFiles.keys(), nargs='+', default=dict_sourceFiles.keys(), help='E. g., if you choose "TTbar__FullyMerged", then only this target root file will be created.')
parser.add_argument('-f', '--force', action='store_true', help='''Use hadd's -f option. Force overwriting of output files.''')
parser.add_argument('--run2', action='store_true')
args = parser.parse_args(sys.argv[1:])

if args.run2:
    args.years = ['run2']
    args.channels = ['both']

if not args.all:
    args_systs = args.systs
else:
    args_systs = possible_systs
print args_systs

# sys.exit()

mainselOutputDir = os.path.join(os.environ.get('CMSSW_BASE'), 'src/UHH2/HighPtSingleTop/output/Analysis/mainsel')

hadd_tasks = list() # pairs of command_string, logFilePath (see below)

for year in args.years:
    for channel in args.channels:
        for syst in args_systs:
            if syst=='nominal':
                nominal=True
            else:
                nominal=False
            # outputDir = os.path.join(mainselOutputDir, '{UL16preVFP,UL16postVFP,UL17,UL18}' if year=='run2' else year, '{ele,muo}' if channel=='both' else channel, ('' if nominal else 'syst_')+syst)
            haddDir = os.path.join(mainselOutputDir, year, channel, ('' if nominal else 'syst_')+syst, 'hadded')
            # haddDir = os.path.join(outputDir, 'hadded')
            logDir = os.path.join(haddDir, 'log')
            os.system('mkdir -p '+logDir)
            # print haddDir
            # print outputDir

            for key in dict_sourceFiles.keys():
                if not key in args.targets:
                    continue
                if key=='DATA':
                    data=True
                else:
                    data=False
                if key=='nonQCD' and not nominal:
                    continue
                if not nominal and data: continue
                fileNamePrefix_ = fileNamePrefix+('DATA.' if data else 'MC.')
                targetFilePath = os.path.join(haddDir, fileNamePrefix_+key+'.root')
                # print targetFilePath
                sourceFilePaths = np.array([]) # list of all input root files to be hadded.
                for pattern in dict_sourceFiles[key]:
                    if year=='run2':
                        for year_ in years:
                            if channel=='both':
                                for channel_ in channels:
                                    outputDir = os.path.join(mainselOutputDir, year_, channel_, ('' if nominal else 'syst_')+syst)
                                    sourceFilePaths = np.append(sourceFilePaths, glob.glob(os.path.join(outputDir, fileNamePrefix_+pattern+'_'+year_+'.root')))
                            else:
                                outputDir = os.path.join(mainselOutputDir, year_, channel, ('' if nominal else 'syst_')+syst)
                                sourceFilePaths = np.append(sourceFilePaths, glob.glob(os.path.join(outputDir, fileNamePrefix_+pattern+'_'+year_+'.root')))
                    else:
                        if channel=='both':
                            for channel_ in channels:
                                outputDir = os.path.join(mainselOutputDir, year, channel_, ('' if nominal else 'syst_')+syst)
                                sourceFilePaths = np.append(sourceFilePaths, glob.glob(os.path.join(outputDir, fileNamePrefix_+pattern+'_'+year+'.root')))
                        else:
                            outputDir = os.path.join(mainselOutputDir, year, channel, ('' if nominal else 'syst_')+syst)
                            sourceFilePaths = np.append(sourceFilePaths, glob.glob(os.path.join(outputDir, fileNamePrefix_+pattern+'_'+year+'.root')))
                sourceFilePaths.flatten()
                # print str(len(sourceFilePaths))
                # print sourceFilePaths
                if len(sourceFilePaths) == 0:
                    continue

                # In the following few lines, we sort the root files which are to be hadded by the number of events stored in their analysis trees
                # (there is a bug in hadd that would lead to not correctly added trees if the first file in the list has no events; in case that all trees are empty, we have nothing to fear)
                numbersOfEntries = np.array([])
                for rootFileName in sourceFilePaths:
                    rootFile = root.TFile.Open(rootFileName, 'READ')
                    numbersOfEntries = np.append(numbersOfEntries, rootFile.Get('AnalysisTree').GetEntries())
                numbersOfEntries.flatten()
                # print numbersOfEntries
                sourceFilePaths = sourceFilePaths[np.argsort(-numbersOfEntries)]
                # print sourceFilePaths

                command_string = 'nice -n 10 hadd '
                if args.force:
                    command_string += '-f '
                command_string += targetFilePath+' '+' '.join([x for x in sourceFilePaths])
                logFilePath = os.path.join(logDir, 'log.'+key+'.txt')
                # print command_string
                hadd_tasks.append([command_string, logFilePath])

# print hadd_tasks
# for i in hadd_tasks:
#     print i
# sys.exit()

FNULL = open(os.devnull, 'w')

def hadd_task(args): # args = [command, logfile]
    process = subprocess.Popen([args[0]+' > '+args[1]], shell=True, stdout=FNULL, stderr=FNULL)
    process.wait()

p = multiprocessing.Pool(multiprocessing.cpu_count() / 2) # only use half of all CPU threads to be friendly to other users
p.map(hadd_task, hadd_tasks)

#!/usr/bin/env python

import sys
import argparse
import os
import subprocess


#------------------#
# GLOBAL VARIABLES #
#------------------#

lumis = {
    '2016': '35.92',
    '2017': '41.53',
    '2018': '59.74',
    'run2': '137.19',
}

years = ['2016', '2017', '2018']
channels = ['ele', 'muo']
combos = list()

#--------#
# PARSER #
#--------#

if not sys.argv[1:]: sys.exit('No arguments provided. Exit.')
parser = argparse.ArgumentParser()
parser.add_argument('--all', action='store_true')
parser.add_argument('--runii', action='store_true')
parser.add_argument('-c', '--channels', choices=channels, nargs='*', default=[])
parser.add_argument('-y', '--years', choices=years, nargs='*', default=[])
parser.add_argument('-s', '--singleeps', action='store_true')
parser.add_argument('-l', '--legend', action='store_true')
parser.add_argument('-n', '--no', choices=['data', 'qcd'], nargs='*', default=[])
args = parser.parse_args(sys.argv[1:])

if args.all == True:
    if len(args.channels) or len(args.years):
        sys.exit('Do not use "--all" option jointly with --channels and/or --years options.')
    for year in years:
        for channel in channels:
            combos.append((year, channel))
elif len(args.channels)+len(args.years) == 0:
    print 'No years or channels given.'
else:
    for year in (args.years if len(args.years) else years):
        for channel in (args.channels if len(args.channels) else channels):
            combos.append((year, channel))
if args.runii == True:
    combos.append(('run2', 'both'))

if not len(combos):
    sys.exit('Nothing to plot. Exit.')
print 'Working on:'
print combos

#---------#
# PROGRAM #
#---------#

uhh2Dir = os.environ.get('CMSSW_BASE')+'/src/UHH2/'
mainselDir = uhh2Dir+'HighPtSingleTop/output/mainsel/'
templateDir = uhh2Dir+'HighPtSingleTop/sframeplotter/'
workDir = templateDir+'workdir/'
if not os.path.exists(workDir): os.mkdir(workDir)
sframeplotterBase = os.environ.get('CMSSW_BASE')+'/../SFramePlotter/'

FNULL = open(os.devnull, 'w')

for year, channel in combos:
    template_name = 'template.steer'
    if len(args.no):
        if 'data' in args.no: template_name.replace('.', '_woData.')
        if 'qcd' in args.no: template_name.replace('.', '_woQCD.')
    print 'Using template:', template_name
    template_file = open(templateDir+template_name, 'r')
    steerFilePath = workDir+'_'.join(['mainsel', year, channel])+'.steer'
    fCycleName = mainselDir+year+'/'+channel+'/nominal/hadded/uhh2.AnalysisModuleRunner'
    outputDir = mainselDir+year+'/'+channel+('/plots_single/' if args.singleeps else '/plots/')
    fOutputPsFile = outputDir+'_'.join(['mainsel', year, channel])+'.ps'
    if not os.path.exists(outputDir):
        print 'Create new directory:', outputDir
        os.mkdir(outputDir)
    with open(steerFilePath, 'w') as steerFile:
        print 'Create new steer file:', steerFilePath
        for line in template_file:
            newline = line
            newline = newline.replace('<<<fCycleName>>>', fCycleName)
            newline = newline.replace('<<<fOutputPsFile>>>', fOutputPsFile)
            newline = newline.replace('<<<fLumi>>>', lumis[year])
            newline = newline.replace('<<<bSingleEPS>>>', 'true' if args.singleeps else 'false')
            newline = newline.replace('<<<bDrawLegend>>>', 'true' if args.legend else 'false')
            steerFile.write(newline)
    template_file.close()
    # Plots does not except absolute file paths to steer file, thus get relative path of steer file with working directory = sframeplotterBase
    steerFilePathRelative = steerFilePath.replace(uhh2Dir, '../'+os.environ.get('CMSSW_BASE').split('/')[-1]+'/src/UHH2/')
    subprocess.Popen(('Plots -f '+steerFilePathRelative), shell=True, cwd=sframeplotterBase, stdout=FNULL, stderr=FNULL)
    # subprocess.Popen(('echo '+steerFilePathRelative), shell=True, cwd=sframeplotterBase)#, stdout=FNULL, stderr=FNULL)

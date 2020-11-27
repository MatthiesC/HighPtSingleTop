#!/usr/bin/env python

import os
import subprocess

lumis = {
    '2016': '35.92',
    '2017': '41.53',
    '2018': '59.74',
    'run2': '137.19',
}


combos = list()
years = ['2016', '2017', '2018']
channels = ['ele', 'muo']
for year in years:
    for channel in channels:
        combos.append((year, channel))
combos.append(('run2', 'both'))

mainselDir = os.environ.get('CMSSW_BASE')+'/src/UHH2/HighPtSingleTop/output/mainsel/'


FNULL = open(os.devnull, 'w')

for year, channel in combos:
    template_file = open('template.steer', 'r')
    outFileName = '_'.join(['mainsel', year, channel])+'.steer'
    fCycleName = mainselDir+year+'/'+channel+'/nominal/hadded/uhh2.AnalysisModuleRunner'
    outputDir = mainselDir+year+'/'+channel+'/nominal/plots/'
    fOutputPsFile = outputDir+'_'.join(['mainsel', year, channel])+'.ps'
    if not os.path.exists(outputDir):
        os.mkdir(outputDir)
    with open(outFileName, 'w') as outFile:
        for line in template_file:
            newline = line
            newline = newline.replace('<<<fCycleName>>>', fCycleName)
            newline = newline.replace('<<<fOutputPsFile>>>', fOutputPsFile)
            newline = newline.replace('<<<fLumi>>>', lumis[year])
            outFile.write(newline)
    template_file.close()
    subprocess.Popen(['Plots -f '+outFileName], shell=True, stdout=FNULL, stderr=FNULL)

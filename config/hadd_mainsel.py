#!/usr/bin/env python

import os
import sys
import argparse
import glob
from ROOT import TFile
import subprocess


def check_ttree(fileName, treeName='AnalysisTree'):

    file = TFile.Open(fileName, 'READ')
    tree = file.Get(treeName)
    entries = tree.GetEntriesFast()

    return entries > 0


channels = ['ele', 'muo']
years = ['2016', '2017', '2018']

if not sys.argv[1:]: sys.exit('No arguments provided. Exit.')
parser = argparse.ArgumentParser()
parser.add_argument('--all', action='store_true')
parser.add_argument('-c', '--channels', choices=channels, nargs='*', default=[])
parser.add_argument('-y', '--years', choices=years, nargs='*', default=[])
parser.add_argument('--runii', action='store_true', help='Do not hadd individual mainsels but hadd full Run 2. Must be used jointly with "--all" option.')
args = parser.parse_args(sys.argv[1:])

if args.all == True:
    if len(args.channels) or len(args.years):
        sys.exit('Do not use "--all" option jointly with other options.')
else:
    channels = args.channels
    years = args.years
if args.runii == True and args.all == False:
    sys.exit('"--runii" option only allowed if using "--all" option.')


list_of_all_hadd_commands = list()
list_of_all_hadd_dirs = list()
list_of_all_log_dirs = list()

dict_of_target_files = dict()

for channel in channels:

    dict_of_target_files[channel] = dict()

    for year in years:

        dict_of_target_files[channel][year] = dict()

        syst_name = 'nominal'

        # configDir = os.environ.get('CMSSW_BASE')+'/src/UHH2/HighPtSingleTop/config/config_mainsel_'+year+'_'+channel+'/'
        outputDir = os.environ.get('CMSSW_BASE')+'/src/UHH2/HighPtSingleTop/output/mainsel/'+year+'/'+channel+'/'+syst_name+'/'
        haddDir = outputDir+'hadded/'
        logDir = haddDir+'log/'
        list_of_all_hadd_dirs.append(haddDir)
        list_of_all_log_dirs.append(logDir)
        prefix = 'uhh2.AnalysisModuleRunner.'

        decays = ['Had', 'Ele', 'Muo', 'Tau']
        topdecays = ['TopTo'+x for x in decays]
        wdecays = ['WTo'+x for x in decays]
        bothdecays = list()
        for t in topdecays:
            for w in wdecays:
                bothdecays.append(t+'_'+w)
        signaldecays = list()
        bkgdecays = list()
        for b in bothdecays:
            if channel == 'ele' and 'ToHad' in b and 'ToEle' in b:
                signaldecays.append(b)
            elif channel == 'muo' and 'ToHad' in b and 'ToMuo' in b:
                signaldecays.append(b)
            else:
                bkgdecays.append(b)


        rootFiles = dict()

        rootFiles['DATA'] = dict()
        rootFiles['DATA']['sourceFiles'] = glob.glob(outputDir+prefix+'DATA.DATA*.root')
        rootFiles['DATA']['targetFile'] = 'DATA.DATA.root'

        rootFiles['QCD'] = dict()
        rootFiles['QCD']['sourceFiles'] = glob.glob(outputDir+prefix+'MC.QCD*.root')
        rootFiles['QCD']['targetFile'] = 'MC.QCD.root'

        rootFiles['Diboson'] = dict()
        rootFiles['Diboson']['sourceFiles'] = glob.glob(outputDir+prefix+'MC.Diboson*.root')
        rootFiles['Diboson']['targetFile'] = 'MC.Diboson.root'

        rootFiles['DYJets'] = dict()
        rootFiles['DYJets']['sourceFiles'] = glob.glob(outputDir+prefix+'MC.DYJets*.root')
        rootFiles['DYJets']['targetFile'] = 'MC.DYJets.root'

        rootFiles['WJetsLight'] = dict()
        rootFiles['WJetsLight']['sourceFiles'] = glob.glob(outputDir+prefix+'MC.WJetsLight*.root')
        rootFiles['WJetsLight']['targetFile'] = 'MC.WJetsLight.root'

        rootFiles['WJetsHeavy'] = dict()
        rootFiles['WJetsHeavy']['sourceFiles'] = glob.glob(outputDir+prefix+'MC.WJetsHeavy*.root')
        rootFiles['WJetsHeavy']['targetFile'] = 'MC.WJetsHeavy.root'

        rootFiles['WJets'] = dict()
        rootFiles['WJets']['sourceFiles'] = glob.glob(outputDir+prefix+'MC.WJets*.root')
        rootFiles['WJets']['targetFile'] = 'MC.WJets.root'

        rootFiles['TTbar'] = dict()
        rootFiles['TTbar']['sourceFiles'] = glob.glob(outputDir+prefix+'MC.TTbar*.root')
        rootFiles['TTbar']['targetFile'] = 'MC.TTbar.root'

        rootFiles['ST_otherChannels'] = dict()
        rootFiles['ST_otherChannels']['sourceFiles'] = glob.glob(outputDir+prefix+'MC.ST_tChannel*.root') + glob.glob(outputDir+prefix+'MC.ST_sChannel*.root')
        rootFiles['ST_otherChannels']['targetFile'] = 'MC.ST_otherChannels.root'

        rootFiles['ST_tW_DR_NoFullyHadronic'] = dict()
        rootFiles['ST_tW_DR_NoFullyHadronic']['sourceFiles'] = glob.glob(outputDir+prefix+'MC.ST_tW_DR_NoFullyHadronic*.root')
        rootFiles['ST_tW_DR_NoFullyHadronic']['targetFile'] = 'MC.ST_tW_DR_NoFullyHadronic.root'

        rootFiles['ST_tW_DR_NoFullyHadronic_Sig'] = dict()
        temp = [outputDir+prefix+'MC.ST_tW_DR_NoFullyHadronic_'+x+'_*.root' for x in signaldecays]
        temp2 = list()
        for t in temp:
            temp2 = temp2 + glob.glob(t)
        rootFiles['ST_tW_DR_NoFullyHadronic_Sig']['sourceFiles'] = temp2
        rootFiles['ST_tW_DR_NoFullyHadronic_Sig']['targetFile'] = 'MC.ST_tW_DR_NoFullyHadronic_Sig.root'

        rootFiles['ST_tW_DR_NoFullyHadronic_Bkg'] = dict()
        temp = [outputDir+prefix+'MC.ST_tW_DR_NoFullyHadronic_'+x+'_*.root' for x in bkgdecays]
        temp2 = list()
        for t in temp:
            temp2 = temp2 + glob.glob(t)
        rootFiles['ST_tW_DR_NoFullyHadronic_Bkg']['sourceFiles'] = temp2
        rootFiles['ST_tW_DR_NoFullyHadronic_Bkg']['targetFile'] = 'MC.ST_tW_DR_NoFullyHadronic_Bkg.root'

        rootFiles['ST_tW_DR_inclusiveDecays'] = dict()
        rootFiles['ST_tW_DR_inclusiveDecays']['sourceFiles'] = glob.glob(outputDir+prefix+'MC.ST_tW_DR_inclusiveDecays*.root')
        rootFiles['ST_tW_DR_inclusiveDecays']['targetFile'] = 'MC.ST_tW_DR_inclusiveDecays.root'

        rootFiles['ST_tW_DR_inclusiveDecays_Sig'] = dict()
        temp = [outputDir+prefix+'MC.ST_tW_DR_inclusiveDecays_'+x+'_*.root' for x in signaldecays]
        temp2 = list()
        for t in temp:
            temp2 = temp2 + glob.glob(t)
        rootFiles['ST_tW_DR_inclusiveDecays_Sig']['sourceFiles'] = temp2
        rootFiles['ST_tW_DR_inclusiveDecays_Sig']['targetFile'] = 'MC.ST_tW_DR_inclusiveDecays_Sig.root'

        rootFiles['ST_tW_DR_inclusiveDecays_Bkg'] = dict()
        temp = [outputDir+prefix+'MC.ST_tW_DR_inclusiveDecays_'+x+'_*.root' for x in bkgdecays]
        temp2 = list()
        for t in temp:
            temp2 = temp2 + glob.glob(t)
        rootFiles['ST_tW_DR_inclusiveDecays_Bkg']['sourceFiles'] = temp2
        rootFiles['ST_tW_DR_inclusiveDecays_Bkg']['targetFile'] = 'MC.ST_tW_DR_inclusiveDecays_Bkg.root'


        # Reorder sourceFiles such that the first file in the list has an AnalysisTree (if there is at least one file having an AnalysisTree).
        # This is needed for hadd to work properly.
        for key in rootFiles.keys():
            rootFiles[key]['reorderedSourceFiles'] = list()
            # rootFiles[key]['targetFile'] = haddDir+prefix+rootFiles[key]['targetFile']
            dict_of_target_files[channel][year][key] = rootFiles[key]['targetFile']

        for key in rootFiles.keys():
            position = -1
            for i in range(len(rootFiles[key]['sourceFiles'])):
                if check_ttree(rootFiles[key]['sourceFiles'][i]) and position == -1:
                    position = i
                    rootFiles[key]['reorderedSourceFiles'].append(rootFiles[key]['sourceFiles'][i])
                    break
            for i in range(len(rootFiles[key]['sourceFiles'])):
                if position != -1 and i != position:
                    rootFiles[key]['reorderedSourceFiles'].append(rootFiles[key]['sourceFiles'][i])
                elif position == -1:
                    rootFiles[key]['reorderedSourceFiles'].append(rootFiles[key]['sourceFiles'][i])
                    position = 0

            command_string = 'nice -n 10 hadd '+haddDir+prefix+rootFiles[key]['targetFile']+' '+' '.join(rootFiles[key]['reorderedSourceFiles'])
            hadd_logfile = logDir+'log.'+rootFiles[key]['targetFile'].replace('.root','.txt')
            rootFiles[key]['hadd_command'] = command_string
            rootFiles[key]['hadd_logfile'] = hadd_logfile
            list_of_all_hadd_commands.append((command_string, hadd_logfile))



for haddDir in list_of_all_hadd_dirs:
    if not os.path.exists(haddDir):
        os.mkdir(haddDir)
for logDir in list_of_all_log_dirs:
    if not os.path.exists(logDir):
        os.mkdir(logDir)


FNULL = open(os.devnull, 'w')
processes = list()


if args.runii == False:

    for haddDir in list_of_all_hadd_dirs:
        if not os.path.exists(haddDir):
            os.mkdir(haddDir)
    for logDir in list_of_all_log_dirs:
        if not os.path.exists(logDir):
            os.mkdir(logDir)

    for command, logfile in list_of_all_hadd_commands:
        subprocess.Popen([command+' > '+logfile], shell=True, stdout=FNULL, stderr=FNULL)

else:

    mainselDir = os.environ.get('CMSSW_BASE')+'/src/UHH2/HighPtSingleTop/output/mainsel/'
    run2Dir = mainselDir+'run2/both/nominal/'#+channel+'/'+syst_name+'/'
    haddDir = run2Dir+'hadded/'
    logDir = haddDir+'log/'
    prefix = 'uhh2.AnalysisModuleRunner.'
    os.system('mkdir -p '+logDir)

    list_of_run2_hadd_commands = list()
    list_of_run2_log_files = list()

    for key in dict_of_target_files['ele']['2016']:
        # print key
        target_file_name = dict_of_target_files['ele']['2016'][key]
        # print target_file_name
        command_string = 'nice -n 10 hadd '+haddDir+prefix+target_file_name+' '+mainselDir+'201{6,7,8}/{ele,muo}/nominal/hadded/'+prefix+target_file_name
        # print command_string
        log_file = logDir+'log.'+target_file_name.replace('.root','.txt')
        # print command_string
        # print log_file
        subprocess.Popen([command_string+' > '+log_file], shell=True, stdout=FNULL, stderr=FNULL)

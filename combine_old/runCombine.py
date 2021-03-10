import ROOT as root
import os
import sys
from collections import defaultdict
import argparse



opt_year = ['2016', '2017']
opt_channel = ['ele', 'muo']
opt_category = ['ttag', 'wtag']

if not sys.argv[1:]: sys.exit('No arguments provided. Exit.')
parser = argparse.ArgumentParser()
parser.add_argument('-y', '--year', choices=opt_year)
parser.add_argument('-c', '--channel', choices=opt_channel)
parser.add_argument('-a', '--category', choices=opt_category)
args = parser.parse_args(sys.argv[1:])

year = args.year
channel = args.channel
category = args.category

workdirName = '_'.join(['workdir', year, channel, category])

combineChannelNames = ['ChannelXYZ']

processes = ['ST_tW_DR_NoFullyHadronic_Sig', 'ST_tW_DR_NoFullyHadronic_Bkg', 'TTbar', 'ST_otherChannels', 'WJets', 'DYJets', 'Diboson']
mc_norm = {
    'ST_tW_DR_NoFullyHadronic_Sig': 2.0,
    'ST_tW_DR_NoFullyHadronic_Bkg': 2.0,
    'TTbar': 1.2,
    'ST_otherChannels': 2.0,
    'WJets': 1.2,
    'DYJets': 1.2,
    'Diboson': 2.0,
    'QCD': 2.0,
    }

# systematics_regular = ['jec', 'jer', 'pileup', 'mur', 'muf']
# systematics_ignored = []
systs = ['jec', 'jer', 'ttag', 'wtag', 'pileup', 'prefiring', 'muonid', 'muoniso', 'muontrigger', 'electronid', 'electronreco', 'electrontrigger', 'mur', 'muf']


def create_workdir():
    if not os.path.exists(workdirName): os.makedirs(workdirName)
#     if not os.path.exists(workdir_path+"plots/prefit"): os.makedirs(workdir_path+"plots/prefit")
#     if not os.path.exists(workdir_path+"plots/postfit"): os.makedirs(workdir_path+"plots/postfit")


def write_histogram(rootInFilePath, inHistName, outHistName, rootOutFile):
    rootInFile = root.TFile(rootInFilePath, 'READ')
    inHist = rootInFile.Get(inHistName)
    #rate = inHist.Integral()
    rate = -1 # for a binned shape analysis, combine will get the rate from the input histograms on its own (-1 must be set here)
    rootOutFile.cd()
    inHist.Write(outHistName)
    rootInFile.Close()
    return rate # could be used later if needed


def write_input_rootfile_and_datacard():

    analysis_dir = '/nfs/dust/cms/user/matthies/HighPtSingleTop/102X_v2/mainsel/'+year+'/'+channel+'/'
    filePrefix = 'uhh2.AnalysisModuleRunner.'

    rootFileName = 'input_hists_for_datacard.root'
    rootFile = root.TFile(rootFileName, 'RECREATE')

    histDir = 'DNNHists_'+('TopTag' if category=='ttag' else 'WTag')+'/'
    histName = 'DNNoutput_'+category+'__binary_50bins'
    histPath = histDir+histName

    combineChannelName = 'ChannelXYZ'

    write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'DATA.DATA.root', histPath, combineChannelName+'__data_obs', rootFile)

    for proc in processes:

        write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc, rootFile)
        write_histogram(analysis_dir+'/syst_jec_up/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_jecUp', rootFile)
        write_histogram(analysis_dir+'/syst_jec_down/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_jecDown', rootFile)
        write_histogram(analysis_dir+'/syst_jer_up/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_jerUp', rootFile)
        write_histogram(analysis_dir+'/syst_jer_down/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_jerDown', rootFile)
        write_histogram(analysis_dir+'/syst_pileup_up/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_pileupUp', rootFile)
        write_histogram(analysis_dir+'/syst_pileup_down/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_pileupDown', rootFile)
        write_histogram(analysis_dir+'/syst_mur_up/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_murUp', rootFile)
        write_histogram(analysis_dir+'/syst_mur_down/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_murDown', rootFile)
        write_histogram(analysis_dir+'/syst_muf_up/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_mufUp', rootFile)
        write_histogram(analysis_dir+'/syst_muf_down/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_mufDown', rootFile)
        if year in ['2016', '2017']:
            write_histogram(analysis_dir+'/syst_prefiring_up/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_prefiringUp', rootFile)
            write_histogram(analysis_dir+'/syst_prefiring_down/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_prefiringDown', rootFile)
        else:
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_prefiringUp', rootFile)
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_prefiringDown', rootFile)
        if category == 'ttag':
            write_histogram(analysis_dir+'/syst_ttag_up/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_ttagUp', rootFile)
            write_histogram(analysis_dir+'/syst_ttag_down/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_ttagDown', rootFile)
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_wtagUp', rootFile)
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_wtagDown', rootFile)
        elif category == 'wtag':
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_ttagUp', rootFile)
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_ttagDown', rootFile)
            write_histogram(analysis_dir+'/syst_wtag_up/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_wtagUp', rootFile)
            write_histogram(analysis_dir+'/syst_wtag_down/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_wtagDown', rootFile)
        if channel == 'ele':
            write_histogram(analysis_dir+'/syst_electronid_up/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_electronidUp', rootFile)
            write_histogram(analysis_dir+'/syst_electronid_down/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_electronidDown', rootFile)
            write_histogram(analysis_dir+'/syst_electronreco_up/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_electronrecoUp', rootFile)
            write_histogram(analysis_dir+'/syst_electronreco_down/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_electronrecoDown', rootFile)
            write_histogram(analysis_dir+'/syst_electrontrigger_up/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_electrontriggerUp', rootFile)
            write_histogram(analysis_dir+'/syst_electrontrigger_down/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_electrontriggerDown', rootFile)
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_muonidUp', rootFile)
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_muonidDown', rootFile)
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_muonisoUp', rootFile)
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_muonisoDown', rootFile)
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_muontriggerUp', rootFile)
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_muontriggerDown', rootFile)
        elif channel == 'muo':
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_electronidUp', rootFile)
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_electronidDown', rootFile)
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_electronrecoUp', rootFile)
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_electronrecoDown', rootFile)
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_electrontriggerUp', rootFile)
            write_histogram(analysis_dir+'/nominal/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_electrontriggerDown', rootFile)
            write_histogram(analysis_dir+'/syst_muonid_up/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_muonidUp', rootFile)
            write_histogram(analysis_dir+'/syst_muonid_down/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_muonidDown', rootFile)
            write_histogram(analysis_dir+'/syst_muoniso_up/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_muonisoUp', rootFile)
            write_histogram(analysis_dir+'/syst_muoniso_down/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_muonisoDown', rootFile)
            write_histogram(analysis_dir+'/syst_muontrigger_up/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_muontriggerUp', rootFile)
            write_histogram(analysis_dir+'/syst_muontrigger_down/hadded/'+filePrefix+'MC.'+proc+'.root', histPath, combineChannelName+'__'+proc+'_muontriggerDown', rootFile)

    rootFile.Close()



    dcFileName = 'datacard.txt'
    with open(dcFileName, 'w') as dcFile:
        dcFile.write('Automatically generated Combine DataCard\n')

        dcFile.write('imax {}\n'.format(len(combineChannelNames)))
        dcFile.write('jmax {}\n'.format(len(processes)-1))
        dcFile.write('kmax *\n')

        dcFile.write('shapes * * {} $CHANNEL__$PROCESS $CHANNEL__$PROCESS_$SYSTEMATIC \n'.format(rootFileName))

        dcFile.write('bin {}\n'.format(" ".join([c for c in combineChannelNames])))
        # dcFile.write('observation {}\n'.format(" ".join([str(observed[c]) for c in combineChannelNames])))
        dcFile.write('observation {}\n'.format(" ".join(['-1' for c in combineChannelNames])))

        dcFile.write('bin {}\n'.format(" ".join([(" "+c)*len(processes) for c in combineChannelNames])))
        dcFile.write('process {}\n'.format((" "+" ".join([process for process in processes]))*len(combineChannelNames)))
        dcFile.write('process {}\n'.format(" ".join([str(x) for x in range(len(processes)) * len(combineChannelNames)])))
        #dcFile.write('rate {}\n'.format(" ".join([str(rates[c][process]) + " " + " ".join([str(rates[c][process]) for process in processes]) for c in combineChannelNames]))) # fixME
        dcFile.write('rate '+'-1 '*(len(processes)*len(combineChannelNames))+'\n')

        dcFile.write('lumi lnN {}\n'.format(" ".join([str(1.025) for process in processes for c in combineChannelNames])))
        for process in processes:
            dcFile.write('rate_{} lnN {}\n'.format(process, (" -"*processes.index(process) + ' ' + str(mc_norm[process]) + ' -'*(len(processes) - processes.index(process) - 1))*len(combineChannelNames)))
        for syst in systs:
            dcFile.write('{} shape {}\n'.format(syst, " 1"*len(processes)*len(combineChannelNames)))

        dcFile.write('* autoMCStats 0 0 1\n')


def execute_combine():

    os.system('text2workspace.py datacard.txt -o workspace.root -m 173')
    os.system('combine -M FitDiagnostics workspace.root -m 173 --rMin -20 --rMax 20 -t -1 --expectSignal=1 --saveShapes --saveWithUncertainties')
    os.system('combineTool.py -M Impacts -d workspace.root -m 173 --rMin -20 --rMax 20 -t -1 --expectSignal=1 --robustFit 1 --doInitialFit')
    os.system('combineTool.py -M Impacts -d workspace.root -m 173 --rMin -20 --rMax 20 -t -1 --expectSignal=1 --robustFit 1 --doFits')
    os.system('combineTool.py -M Impacts -d workspace.root -m 173 --rMin -20 --rMax 20 -t -1 --expectSignal=1 --robustFit 1 --output impacts.json')
    os.system('plotImpacts.py -i impacts.json -o impacts')


    # os.system("combine -M MultiDimFit "+dcFileName+" --saveFitResult")
    # os.system("text2workspace.py "+dcFileName)
    # os.system("PostFitShapesFromWorkspace -d "+dcFileName+" -w "+dcFileName.replace(".txt", ".root")+" -f multidimfit.root:fit_mdf -o "+"out_"+binRange+".root --postfit --sampling")
    # os.chdir('../../../')


if __name__=='__main__':

    create_workdir()
    os.chdir(workdirName)
    write_input_rootfile_and_datacard()
    execute_combine()
    os.chdir('..')

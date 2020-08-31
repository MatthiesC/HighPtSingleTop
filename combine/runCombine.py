import ROOT as root
import os
from collections import defaultdict


#processes = ['ST_tW_signal', 'ST_tW_bkg', 'ST_non-tW_t-Ch', 'ST_non-tW_s-Ch', 'TTbar', 'WJets', 'DYJets']#, 'Diboson']
processes = ['ST_tW_signal', 'ST_tW_bkg', 'TTbar', 'WJets', 'DYJets']
mc_norm = {
    'ST_tW_signal': 2.0,
    'ST_tW_bkg': 2.0,
    #'ST_non-tW_t-Ch': 2.0,
    #'ST_non-tW_s-Ch': 2.0,
    'TTbar': 2.0,
    'WJets': 2.0,
    'DYJets': 2.0,
    #'Diboson': 2.0
    }

systs = { # syst_name, syst_dir
    'JEC': 'syst/jec/',
    'JER': 'syst/jer/',
}


def create_paths(workdir_path):
    if not os.path.exists(workdir_path): os.makedirs(workdir_path)
    if not os.path.exists(workdir_path+"plots/prefit"): os.makedirs(workdir_path+"plots/prefit")
    if not os.path.exists(workdir_path+"plots/postfit"): os.makedirs(workdir_path+"plots/postfit")


def write_histogram(rootInFilePath, inHistName, outHistName, rootOutFile):
    rootInFile = root.TFile(rootInFilePath, 'READ')
    inHist = rootInFile.Get(inHistName)
    #rate = inHist.Integral()
    rate = -1 # for a binned shape analysis, combine will get the rate from the input histograms on its own (-1 must be set here)
    rootOutFile.cd()
    inHist.Write(outHistName)
    rootInFile.Close()
    return rate # could be used later if needed


def run_one_bin(binRange, yearEleMuo="2016muo"):

    print('Working on: '+binRange)

    workdir_path = "./workdir/"
    workdir_path += yearEleMuo+"/"
    workdir_path += binRange+"/"
    create_paths(workdir_path)
    os.chdir(workdir_path)

    channels = {
        'OneTopTag': 'DNNHists_1TopTag_'+str(binRange)+'/dnn_output_20bins'
        }

    #channels = {
    #    'SR_NoXJet': 'DNNHists_NoXJet_YES_'+binRange+'/dnn_output_20bins',
    #    'CR_0bXJet': 'DNNHists_0bXJet_'+binRange+'/dnn_output_20bins',
    #    'CR_1bXJet': 'DNNHists_1bXJet_'+binRange+'/dnn_output_20bins'
    #    }

    rootFileName = 'inputHistograms_'+str(binRange)+'.root'
    rootFile = root.TFile(rootFileName, 'RECREATE')
    analysis_dir = '/nfs/dust/cms/user/matthies/HighPtSingleTop/102X_v1/MainSelection/2016/Muon/'
    filePrefix = 'uhh2.AnalysisModuleRunner.'

    observed = dict()
    rates = defaultdict(dict)

    for channel, inHistName in channels.iteritems():
        for proc in processes:
            rootInFilePath = analysis_dir + filePrefix + 'MC.' + proc + '.root'
            rates[channel][proc] = write_histogram(rootInFilePath, inHistName, channel+'__'+proc, rootFile)
            for syst_name, syst_dir in systs.iteritems():
                rootInFilePath = analysis_dir + syst_dir + 'up/'  + filePrefix + 'MC.' + proc + '.root'
                write_histogram(rootInFilePath, inHistName, channel+'__'+proc+'_'+syst_name+'Up', rootFile)
                rootInFilePath = analysis_dir + syst_dir + 'down/'  + filePrefix + 'MC.' + proc + '.root'
                write_histogram(rootInFilePath, inHistName, channel+'__'+proc+'_'+syst_name+'Down', rootFile)
        rootInFilePath = analysis_dir + filePrefix + 'DATA.DATA.root'
        observed[channel] = write_histogram(rootInFilePath, inHistName, channel+'__data_obs', rootFile)
    rootFile.Close()

    dcFileName = 'datacard_'+binRange+'.txt'
    with open(dcFileName, 'w') as dcFile:
        dcFile.write('Automatically generated Combine DataCard\n')

        dcFile.write('imax {}\n'.format(len(channels)))
        dcFile.write('jmax {}\n'.format(len(processes)-1))
        dcFile.write('kmax *\n')

        dcFile.write('shapes * * {} $CHANNEL__$PROCESS $CHANNEL__$PROCESS_$SYSTEMATIC \n'.format(rootFileName))

        dcFile.write('bin {}\n'.format(" ".join([channel for channel in channels])))
        dcFile.write('observation {}\n'.format(" ".join([str(observed[channel]) for channel in channels])))

        dcFile.write('bin {}\n'.format(" ".join([(" "+channel)*len(processes) for channel in channels])))
        dcFile.write('process {}\n'.format((" "+" ".join([process for process in processes]))*len(channels)))
        dcFile.write('process {}\n'.format(" ".join([str(x) for x in range(len(processes)) * len(channels)])))
        #dcFile.write('rate {}\n'.format(" ".join([str(rates[channel][process]) + " " + " ".join([str(rates[channel][process]) for process in processes]) for channel in channels]))) # fixME
        dcFile.write('rate '+'-1 '*(len(processes)*len(channels))+'\n')

        dcFile.write('lumi lnN {}\n'.format(" ".join([str(1.025) for process in processes for channel in channels])))
        for process in processes:
            dcFile.write('rate_{} lnN {}\n'.format(process, (" -"*processes.index(process) + ' ' + str(mc_norm[process]) + ' -'*(len(processes) - processes.index(process) - 1))*len(channels)))
        for syst in systs:
            dcFile.write('{} shape {}\n'.format(syst, " 1"*len(processes)*len(channels)))

        dcFile.write('* autoMCStats 0 0 1\n')

    #os.system("combine -M MultiDimFit "+dcFileName+" | grep 'r :'")
    os.system("combine -M MultiDimFit "+dcFileName+" --saveFitResult")
    os.system("text2workspace.py "+dcFileName)
    os.system("PostFitShapesFromWorkspace -d "+dcFileName+" -w "+dcFileName.replace(".txt", ".root")+" -f multidimfit.root:fit_mdf -o "+"out_"+binRange+".root --postfit --sampling")
    os.chdir('../../../')


if __name__=='__main__':

    ptBins = ['Pt200to250', 'Pt250to300', 'Pt300to350', 'Pt350to400', 'Pt400to450', 'Pt450to500', 'Pt500to550', 'Pt550to600', 'Pt600to680', 'Pt680to800', 'Pt800to1000']
    #ptBins = ['Pt200to250']

    for bin in ptBins:
        run_one_bin(bin)

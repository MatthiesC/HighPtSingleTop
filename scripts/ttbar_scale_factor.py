import ROOT as root
import sys

# hadd root files first via Plots from SFramePlotter

pt_threshold = float(sys.argv[1]) # lower t jet pt threshold from which you want to start to count events
directory = str(sys.argv[2]) # directory where root files are stored
filename_prefix = 'uhh2.AnalysisModuleRunner.'

processes = {
    'DATA': {
        'file': 'DATA.DATA',
        },
    'tW signal': {
        'file': 'MC.ST_tW_signal',
        },
    'tW background': {
        'file': 'MC.ST_tW_other',
        },
    't-channel': {
        'file': 'MC.ST_non-tW_t-Ch',
        },
    's-channel': {
        'file': 'MC.ST_non-tW_s-Ch',
        },
    'ttbar': {
        'file': 'MC.TTbar',
        },
    'W+jets': {
        'file': 'MC.WJets',
        },
    'DY+jets': {
        'file': 'MC.DYJets',
        },
    'VV': {
        'file': 'MC.Diboson',
        },
    'QCD': {
        'file': 'MC.QCD',
        },
}


def get_contents(pt_low, rootHist):

    start_bin = rootHist.GetXaxis().FindBin(pt_low)

    contents = 0

    for i in range(rootHist.GetSize()):
        if i < start_bin: continue
        contents += rootHist.GetBinContent(i)

    return contents


for p in processes.keys():

    sum_of_weights = None

    rootFile = root.TFile.Open(directory+'/'+filename_prefix+processes[p]['file']+'.root', 'READ')

    countingHist = rootFile.Get('3_OneTopTag_TopTagHists_Full/top_pt_1GeV')

    sum_of_weights = get_contents(pt_threshold, countingHist)

    processes[p]['events'] = sum_of_weights


events_dataAndMC = 0
events_ttbar = processes['ttbar']['events']
events_data = processes['DATA']['events']

for p in processes.keys():
    events_dataAndMC += processes[p]['events']

expected_ttbar = events_data - (events_dataAndMC - events_data - events_ttbar)

scale_factor = expected_ttbar / events_ttbar



for p in processes.keys():

    print p, ' '*(15-len(p)), ':     ', processes[p]['events']

print 'Total MC:', str(events_dataAndMC - events_data)

print 'Scale factor for TTbar:', str(scale_factor)


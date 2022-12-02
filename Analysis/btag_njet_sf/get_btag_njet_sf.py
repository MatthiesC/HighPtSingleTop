from ROOT import TFile, TH1, TMap, TString, TVectorF

import os

from copy import deepcopy


# Hypernews for reference: https://cms-talk.web.cern.ch/t/phase-space-selection-for-btagging-sf-application/10018


all_channels = ['ele', 'muo']
# all_channels = ['ele']
all_years = ['UL16preVFP', 'UL16postVFP', 'UL17', 'UL18']
# all_years = ['UL18']

inBasePath = os.path.join(os.environ.get('CMSSW_BASE'), 'src/UHH2/HighPtSingleTop/output/Analysis/mainsel')

processes = [
'ST_tW_DR',
'ST_tW_DS',
'ST_tChannel',
'ST_sChannel',
'TTbar',
'WJetsToLNu',
'DYJetsToLL',
'Diboson',
# 'QCD', # ignored because of low stat
]

# region = '3_VjetsSF' # presel wihtout division into b-tags, t-tags, or W-tags (t-tag would also indirectly cut on number of b-tags since t-tag almost ensures that there is a b-tag; thus, just look at plain presel and not into 1t, 0t1W etc. regions)
# region = '4_Region_1b1t'


sf_outfile = TFile.Open('btag_njet_sf.root', 'RECREATE')

# map = TMap()

for channel in all_channels:
    print 'Channel:', channel
    for year in all_years:
        print 'Year:', year
        for process in processes:
            print 'Process:', process

            # process = processes[0]

            inFilePath = os.path.join(inBasePath, year, channel, 'nominal/hadded/uhh2.AnalysisModuleRunner.MC.'+process+'.root')
            file = TFile.Open(inFilePath, 'READ')
            # hist_collection = '3_VjetsSF_AK4'
            hist_collection = '2_VjetsSF_AK4'
            hist_with_btag_sf = deepcopy(file.Get(hist_collection+'/number_puppijets_central_wo_njet_sf'))
            hist_without_btag_sf = deepcopy(file.Get(hist_collection+'/number_puppijets_central_wo_btag_sf_wo_njet_sf'))

            # hist_with_btag_sf = deepcopy(file.Get(region+'_AK4/number_puppijets'))
            # hist_without_btag_sf = deepcopy(file.Get(region+'_AK4/number_puppijets_central'))
            # hist_without_btag_sf.Add(file.Get(region+'_AK4/number_puppijets'), -1.)

            # for proc in processes[1:]:
            #
            #     inFilePath = os.path.join(inBasePath, year, channel, 'nominal/hadded/uhh2.AnalysisModuleRunner.MC.'+proc+'.root')
            #     file = TFile.Open(inFilePath)
            #     # hist_with_btag_sf.Add(file.Get('3_VjetsSF_AK4/number_puppijets_central_wo_njet_sf'))
            #     # hist_without_btag_sf.Add(file.Get('3_VjetsSF_AK4/number_puppijets_central_wo_btag_sf_wo_njet_sf'))
            #
            #     hist_with_btag_sf.Add(file.Get(region+'_AK4/number_puppijets'))
            #     hist_without_btag_sf.Add(file.Get(region+'_AK4/number_puppijets_central'))
            #     hist_without_btag_sf.Add(file.Get(region+'_AK4/number_puppijets'), -1.)




            # iterate through bins; i = n_jets
            n_jets_min = 1
            n_jets_max = 8 # last bin will incluce n_jets >= n_jets_max
            offset = 1
            for n_jets in range(n_jets_min, n_jets_max + 1):
                i_bin = n_jets + offset

                nevents_with_btag_sf = hist_with_btag_sf.GetBinContent(i_bin)
                nevents_without_btag_sf = hist_without_btag_sf.GetBinContent(i_bin)

                if n_jets == n_jets_max:
                    for i in range(i_bin + 1, hist_with_btag_sf.GetNbinsX() + 1): # include overflow
                        nevents_with_btag_sf += hist_with_btag_sf.GetBinContent(i)
                        nevents_without_btag_sf += hist_without_btag_sf.GetBinContent(i)

                # print nevents_with_btag_sf
                # print nevents_without_btag_sf
                # print nevents_with_btag_sf / nevents_without_btag_sf

                sf = nevents_without_btag_sf / nevents_with_btag_sf
                print sf

                sf_outfile.cd()
                sf_name = '_'.join(['sf', year, channel, process, 'njets'+str(n_jets)])
                sf_vec = TVectorF(1) # float vector with one element
                sf_vec[0] = sf
                # map.Add(TString(sf_name), sf)
                sf_outfile.WriteObject(sf_vec, sf_name)

# sf_outfile.cd()
# sf_outfile.WriteObject(map, 'map')
sf_outfile.Close()

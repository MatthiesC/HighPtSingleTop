#!/bin/bash

wget https://github.com/cms-jet/deepAK8ScaleFactors/blob/master/DeepAK8V2_Top_W_SFs.csv

# wget won't work since you need to login first
# Thus, these .csv files are part of the git repo
#wget https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation2016Legacy/DeepJet_2016LegacySF_V1.csv
#wget https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation2016Legacy/DeepJet_2016LegacySF_V1_TuneCP5.csv
#wget https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation94X/DeepFlavour_94XSF_V4_B_F.csv
#wget https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation102X/DeepJet_102XSF_V2.csv

python removeSplittedJESVariationsFromDeepJetCSVFiles.py DeepJet_2016LegacySF_V1.csv __reduced
python removeSplittedJESVariationsFromDeepJetCSVFiles.py DeepJet_2016LegacySF_V1_TuneCP5.csv __reduced
python removeSplittedJESVariationsFromDeepJetCSVFiles.py DeepFlavour_94XSF_V4_B_F.csv __reduced
python removeSplittedJESVariationsFromDeepJetCSVFiles.py DeepJet_102XSF_V2.csv __reduced

#!/bin/bash

mkdir -p 201X
cd 201X
wget https://raw.githubusercontent.com/cms-jet/deepAK8ScaleFactors/master/DeepAK8V2_Top_W_SFs.csv
cd -

# wget won't work since you need to login first
# Thus, these .csv files are part of the git repo
#wget https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation2016Legacy/DeepJet_2016LegacySF_V1.csv
#wget https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation2016Legacy/DeepJet_2016LegacySF_V1_TuneCP5.csv
#wget https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation94X/DeepFlavour_94XSF_V4_B_F.csv
#wget https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation102X/DeepJet_102XSF_V2.csv

python removeSplittedJESVariationsFromDeepJetCSVFiles.py 2016/DeepJet_2016LegacySF_V1.csv __reduced
python removeSplittedJESVariationsFromDeepJetCSVFiles.py 2016/DeepJet_2016LegacySF_V1_TuneCP5.csv __reduced
python removeSplittedJESVariationsFromDeepJetCSVFiles.py 2017/DeepFlavour_94XSF_V4_B_F.csv __reduced
python removeSplittedJESVariationsFromDeepJetCSVFiles.py 2018/DeepJet_102XSF_V2.csv __reduced

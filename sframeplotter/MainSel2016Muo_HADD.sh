#!/bin/bash

workDir=/nfs/dust/cms/user/matthies/HighPtSingleTop/102X_v1/MainSelection/2016/Muon
preFix=uhh2.AnalysisModuleRunner.

declare -a systs=( \
    #/ \ # nominal
    /syst/jec/up/ \
    /syst/jec/down/ \
    /syst/jer/up/ \
    /syst/jer/down/ \
)

declare -a arr=( \
    DATA.DATA \
    MC.ST_tW_signal \
    MC.ST_tW_merged3 \
    MC.ST_tW_merged2 \
    MC.ST_tW_merged1 \
    MC.ST_tW_merged0 \
    MC.ST_tW_bkg \
    MC.ST_tW_bkg_TopToHadAndWToTau \
    MC.ST_tW_bkg_Else \
    MC.ST_tW_DS_signal \
    MC.ST_tW_DS_merged3 \
    MC.ST_tW_DS_merged2 \
    MC.ST_tW_DS_merged1 \
    MC.ST_tW_DS_merged0 \
    MC.ST_tW_DS_bkg \
    MC.ST_tW_DS_bkg_TopToHadAndWToTau \
    MC.ST_tW_DS_bkg_Else \
    MC.ST_non-tW \
    MC.ST_non-tW_t-Ch \
    MC.ST_non-tW_s-Ch \
    MC.TTbar \
    MC.WJets \
    MC.WJetsHeavy \
    MC.WJetsLight \
    MC.DYJets \
    MC.Diboson \
    MC.QCD \
)

for s in "${systs[@]}"
do
    for i in "${arr[@]}"
    do
	hadd -f ${workDir}${s}${preFix}${i}.root ${workDir}${s}${preFix}${i}*.root
    done
done

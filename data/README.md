# Sources for scale factor files

## DeepJet b-tagging

You need to use the files which provide the "iterativefit" scale factors. Note that there are different files depending on the event tune (TuneCP5 or other).

- 2016: https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation2016Legacy
- 2017: https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation94X
- 2018: https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation102X

## HOTVR t-tagging

These files need to be placed into the `UHH2/HOTVR` repository at `HOTVR/data/`.

https://twiki.cern.ch/twiki/bin/view/CMS/JetTopTagging

https://github.com/cms-jet/TopTaggingScaleFactors/

## DeepAK8 W-tagging

TODO: Check if these scale factors are valid for AK8 PUPPI jets, too.

https://twiki.cern.ch/twiki/bin/viewauth/CMS/DeepAK8Tagging2018WPsSFs

https://github.com/cms-jet/deepAK8ScaleFactors/

## Trigger

Using these trigger paths for the muon channel (SingleMuon stream):

- 2016: `HLT_IsoMu24_v* || HLT_IsoTkMu24_v*`
- 2017: `HLT_IsoMu27_v*`
- 2018: `HLT_IsoMu24_v*`

Using these trigger paths for the electron channel (SingleElectron and SinglePhoton streams):

- 2016: `HLT_Ele27_WPTight_Gsf_v* || HLT_Photon175_v*`
- 2017: `HLT_Ele35_WPTight_Gsf_v* || HLT_Photon200_v*`
- 2018: `HLT_Ele32_WPTight_Gsf_v* || HLT_Photon200_v*`

All of these paths have official scale factors, which have been derived by Anna B. / Arne R. (see B2G-20-006).

- Approval of electron scale factors: https://indico.cern.ch/event/879922/
- Approval of muon scale factors: https://indico.cern.ch/event/912058/

## Lepton ID/isolation/reconstruction scale factors

Using the official scale factors. Files copied from `UHH2/common/data/`.


# Lumi and golden JSON files

Look at https://twiki.cern.ch/twiki/bin/view/CMS/TWikiLUM

To get the lumi values yourself: go to lxplus.cern.ch and install brilcalc: https://cms-service-lumi.web.cern.ch/cms-service-lumi/brilwsdoc.html

Now execute:

- 2016 07Aug2017: `brilcalc lumi --normtag /cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/normtag_PHYSICS.json -i /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/ReReco/Final/Cert_271036-284044_13TeV_ReReco_07Aug2017_Collisions16_JSON.txt`
- 2016 23Sep2016: `brilcalc lumi --normtag /cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/normtag_PHYSICS.json -i /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/ReReco/Final/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt`
- 2017: `brilcalc lumi --normtag /cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/normtag_PHYSICS.json -i /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions17/13TeV/ReReco/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON_v1.txt`
- 2018: `brilcalc lumi --normtag /cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/normtag_PHYSICS.json -i /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions18/13TeV/ReReco/Cert_314472-325175_13TeV_17SeptEarlyReReco2018ABC_PromptEraD_Collisions18_JSON.txt`

Results with the above commands:

| YEAR | nfill | nrun | nls    | ncms   | totdelivered(/ub)     | totrecorded(/ub)      |
|------|-------|------|--------|--------|-----------------------|-----------------------|
| 2016 07Aug2017 | 144   | 393  | 232259 | 232241 | 37461527655.495368958 | 35921875594.646492004 |
| 2016 23Sep2016 | 144   | 393  | 232259 | 232241 | 37461527655.495368958 | 35921875594.646492004 |
| 2017 | 175   | 474  | 206561 | 205442 | 44170828381.230857849 | 41528995401.957756042 |
| 2018 | 196   | 478  | 234186 | 233794 | 62223541021.630874634 | 59740565201.545639038 |

To convert the JSON files to ROOT files usable in UHH2, follow instructions here: https://github.com/UHH2/UHH2/wiki/Creating-&-using-luminosity-ROOT-file-in-SFrame (but use `normtag_PHYSICS.json` as recommended in above TWiki)

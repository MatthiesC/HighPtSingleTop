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

# How to use Keras + lwtnn

## Get model from Keras

Train neural network with help of SingleTopClassifier repository.

Once the training is finished, copy the following files to a sub-directory of `KerasDNNModels`:

- `parameters.json`
- `model_arch.json`
- `model_weights.h5`
- For multi-class DNNs in addition: `encoder_classes.npy`

## Install lwtnn

Luckily, lwtnn already is a package within CMSSW_10_2_X versions. So, for your UHH2 code, you can just do `#include "lwtnn/..."` after including lwtnn into your BuildFile and Makefile:

- https://github.com/UHH2/lwtnnExample/blob/master/BuildFile.xml#L5
- https://github.com/UHH2/lwtnnExample/blob/master/Makefile#L4-L7

However, you must use a newer version of lwtnn to convert your above Keras output files into lwtnn format. For this, install CMSSW_11_0_0_pre7. For details, read this: https://github.com/UHH2/UHH2/wiki/Keras-Neural-Network-application-in-UHH2-module-with-lwtnn#install-cmssw-11x-version

## Convert Keras output files to lwtnn format

In case that you login again after installation of CMSSW_11_0_0_pre7 (i.e. without having the environment set up), do:

```
ssh naf-cms-el7
export SCRAM_ARCH=slc7_amd64_gcc700
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd </your/path/to/>CMSSW_11_0_0_pre7/src
cmsenv
export lwtnn_point=$(scram tool tag lwtnn LWTNN_BASE)
cd </your/path/to/>HighPtSingleTop/data/KerasDNNModels/
```

Extract `variables.json` from `parameters.json` via:

```
python </your/path/to/>HighPtSingleTop/scripts/readVariables.py
```

Now, do for example:

```
python3 $lwtnn_point/converters/keras2json.py model_arch.json variables.json model_weights.h5 > neural_net.json
```

Test your newly created `neural_net.json` file:

```
$lwtnn_point/bin/lwtnn-test-arbitrary-net neural_net.json
```

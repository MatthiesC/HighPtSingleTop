#!/bin/bash

cd config_presel_UL16preVFP
sframe_batch.py $1 parsedConfigFile_presel_UL16preVFP.xml
cd ..

cd config_presel_UL16postVFP
sframe_batch.py $1 parsedConfigFile_presel_UL16postVFP.xml
cd ..

cd config_presel_UL17
sframe_batch.py $1 parsedConfigFile_presel_UL17.xml
cd ..

cd config_presel_UL18
sframe_batch.py $1 parsedConfigFile_presel_UL18.xml
cd ..

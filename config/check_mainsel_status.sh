#!/bin/bash

cd config_mainsel_2016_ele
sframe_batch.py parsedConfigFile_mainsel_2016_ele.xml
cd ..
cd config_mainsel_2016_muo
sframe_batch.py parsedConfigFile_mainsel_2016_muo.xml
cd ..
cd config_mainsel_2017_ele
sframe_batch.py parsedConfigFile_mainsel_2017_ele.xml
cd ..
cd config_mainsel_2017_muo
sframe_batch.py parsedConfigFile_mainsel_2017_muo.xml
cd ..
cd config_mainsel_2018_ele
sframe_batch.py parsedConfigFile_mainsel_2018_ele.xml
cd ..
cd config_mainsel_2018_muo
sframe_batch.py parsedConfigFile_mainsel_2018_muo.xml
cd ..

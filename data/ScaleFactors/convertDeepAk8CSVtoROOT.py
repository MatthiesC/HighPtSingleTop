#!/usr/bin/env python

import sys
import ROOT as root
from collections import OrderedDict

class ScaleFactor:

    '''Container class'''

    list_object = list()
    list_year = list()
    list_version = list()
    list_mistagging_rate = list()
    list_pt_intervals = list()

    def __init__(self, csvRow=None):

        self.object = csvRow[0]
        self.year = csvRow[1]
        self.version = csvRow[2]
        self.mistagging_rate = csvRow[3]
        self.pt_low = int(csvRow[4])
        self.pt_high = int(csvRow[5])
        self.sf = float(csvRow[6])
        self.sf_err_down = float(csvRow[7])
        self.sf_err_up = float(csvRow[8])

        pairs = list()
        pairs.append([self.list_object, self.object])
        pairs.append([self.list_year, self.year])
        pairs.append([self.list_version, self.version])
        pairs.append([self.list_mistagging_rate, self.mistagging_rate])
        pairs.append([self.list_pt_intervals, (self.pt_low, self.pt_high)])
        for x,y in pairs:
            if(y not in x): x.append(y)


infilename = sys.argv[1]

dummy = None
sf_list = list()
with open(infilename, 'r') as infile:
    firstline = True;
    for line in infile:
        if(firstline):
            firstline = False
            continue
        row = line.split(',')
        for i in range(len(row)):
            row[i] = row[i].strip()
        # print(row)
        sf = ScaleFactor(row)
        sf_list.append(sf)
        dummy = sf

print "Found these possible values:"
print "Objects:", dummy.list_object
print "Years:", dummy.list_year
print "Versions:", dummy.list_version
print "Mistagging rates:", dummy.list_mistagging_rate
print "Pt intervals:" , dummy.list_pt_intervals

sf_dict = dict()
for o in dummy.list_object:
    sf_dict[o] = dict()
    for y in dummy.list_year:
        sf_dict[o][y] = dict()
        for v in dummy.list_version:
            sf_dict[o][y][v] = dict()
            for m in dummy.list_mistagging_rate:
                sf_dict[o][y][v][m] = dict()

# print sf_dict

for sf in sf_list:
    pt_bin_key = str(sf.pt_low)+'to'+str(sf.pt_high)
    values = dict()
    values["sf"] = sf.sf
    values["sf_err_up"] = sf.sf_err_up
    values["sf_err_down"] = sf.sf_err_down
    sf_dict[sf.object][sf.year][sf.version][sf.mistagging_rate][pt_bin_key] = values

print sf_dict


### TODO: convert sf_dict to root file!!!

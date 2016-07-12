import os,sys

from Setup import *

datasetpath="/cms/scratch/SNU/datasets_v7-6-6/dataset_hntest.txt"


path=open(datasetpath,'r')

xsec_filled=False
validName=True
valid_cv=True
for line in path:
    if "DataSetName" in line:
        splitline= line.split()
        if len(splitline) == 4:
            if"/" in splitline[3]:
                validName=False
    if "xsec" in line:
        splitline= line.split()
        if len(splitline) == 4:
            if float(splitline[3]) > 0.:
                if float(splitline[3]) != 1.:
                    xsec_filled=True
    if "catversion" in line:
        splitline= line.split()
        if len(splitline) == 4:
            if splitline[3] != version:
                valid_cv=False
if not xsec_filled:
    print "Please fill xsec in " + datasetpath + " note the xsec cannot be 1. or 0."
    sys.exit()

if not validName:
    print "Please change DataSetName. This must be one string and contains no characters"
    sys.exit()
if not valid_cv:
    print "Check catversion in " + datasetpath + " this disagrees with version in Setup.py"
    sys.exit()

print datasetpath + " passes checks."
updateinput(datasetpath, version)


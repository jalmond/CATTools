import os

sample =["SingleElectron","SingleMuon","MuonEG","DoubleEG" , "DoubleMuon"]
period = ["G-03Feb2017-v1","H-03Feb2017_ver2-v1","H-03Feb2017_ver3-v1"]
for s in sample:
    for p in period:
        os.system("crab status -d crab_v8-0-7_" + s + "_Run2016"+p)

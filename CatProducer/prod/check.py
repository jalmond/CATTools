import os

sample =["SingleElectron","SingleMuon","MuonEG","DoubleEG" , "DoubleMuon", "JetHT"]
period = ["B-03Feb2017_ver2-v2","C-03Feb2017-v1", "D-03Feb2017-v1","E-03Feb2017-v1","F-03Feb2017-v1", "G-03Feb2017-v1","H-03Feb2017_ver2-v1","H-03Feb2017_ver3-v1"]
for s in sample:
    for p in period:
        os.system("crab status -d crab_v8-0-7_2018_v3_" + s + "_Run2016"+p)
        os.system("crab resubmit -d crab_v8-0-7_2018_v3_" + s + "_Run2016"+p)

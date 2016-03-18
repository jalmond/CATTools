### FILE TO SET UP VARIABLES USED IN Run*.py scripts
import os

##########################################
#### VARIABLES THAT NEED TO BE SET BY USER
##########################################                                                                                                                                                                                                    
######## FullRun = True for running all steps and all samples: makes skims/eff lumi file etc
FullRun=False

####### Settings for making skims/sktrees
MakeSKTrees=False
RunSkims=False
if FullRun == True:
    MakeSKTrees=True
    RunSkims=True

##### Set RunALLSamples=True true to simply add ALL samples to production list                                                                                                                                        
RunALLSamples=True

###### If you want to debug you can set KeepWorkDir=True and the dir will not be deleted
KeepWorkDir=False

#### WHAT VERSION OF CATUPLES ARE YOU RUNNING 
version = "v7-6-3"


#### For data only:
###### Set periods to be processed. IF datasampledir is empty then all periods are automatically ran
data_periods = ["C" ]
###### this overwrites sampledir in Run*data*.py (if this is empty ALL datasets are run
datasampledir = ["DoubleMuon"]
                 #"DoubleEG" ,
                 #"MuonEG",
                 #"SingleMuon",
                 #"SingleElectron",
                 #"SinglePhoton"]

                
#### For MC only
mcsampledir = ["WZ_TuneCUETP8M1_13TeV-pythia8"]

if FullRun==True:
    RunALLSamples=True

if RunALLSamples:
    datasampledir=[]
    mcsampledir=[]

### Set ouput directory at kisti
kisti_output_default="/cms/scratch/jalmond/Cattuples/cat76/cattools/src/CATTools/CommonTools/test/snu/KistiProductionForSNU/"+version+"/"


### Set if you are running full production on kisti site to transfer to snu                                                                                                                                                                  
snu_lqpath="/HeavyNeutrino/13TeV/LQAnalyzer_cat/LQanalyzer/"
username_snu="jalmond"


##########################################
###### VARIABLES THAT ARE NOT SET BY USER
##########################################
host=os.getenv("HOSTNAME")
user=os.getenv("USER") 
latest_version="v7-6-3"

if version != latest_version:
    update = raw_input("You requested to run on old version of catuples. " + latest_version + " is the latest version while you are running on " + version + ". To continue type Y")
    if update != "Y":
        quit()



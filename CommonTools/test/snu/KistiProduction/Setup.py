### FILE TO SET UP VARIABLES USED IN Run*.py scripts
import os

##########################################
#### VARIABLES THAT NEED TO BE SET BY USER
##########################################                                                                                                                                                                                                     

######## FullRun = True for running all steps: makes skims/eff lumi file etc
FullRun = False

#### WHAT VERSION OF CATUPLES ARE YOU RUNNING 
version = "v7-6-1"

#### For data only:
###### Set periods to be processed. IF datasampledir is empty then all periods are automatically ran
data_periods = ["C" , "D"]
###### this overwrites sampledir in Run*data*.py (if this is empty ALL datasets are run
datasampledir = ["DoubleMuon"]
                 #"DoubleEG" ,
                 #"MuonEG",
                 #"SingleMuon",
                 #"SingleElectron",
                 #"SinglePhoton"]

                
#### For MC only
mcsampledir = ["WZ_TuneCUETP8M1_13TeV-pythia8"]

##### Set RunALL=True true to simply add ALL samples to production list
RunALL=False
if RunALL:
    datasampledir=[]
    mcsampledir=[]

### Set ouput directory at kisti
kisti_output_default="/tmp_cms/jalmond_temp/"+version+"/"


### Set if you are running full production on kisti site to transfer to snu                                                                                                                                                                  
snu_lqpath="/HeavyNeutrino/13TeV/LQAnalyzer_cat/LQanalyzer/"
username_snu="jalmond"


##########################################
###### VARIABLES THAT ARE NOT SET BY USER
##########################################
host=os.getenv("HOSTNAME")
user=os.getenv("USER") 
latest_version="v7-6-1"

if version != latest_version:
    update = raw_input("You requested to run on old version of catuples. " + latest_version + " is the latest version while you are running on " + version + ". To continue type Y")
    if update != "Y":
        quit()

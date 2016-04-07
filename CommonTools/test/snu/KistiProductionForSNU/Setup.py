### FILE TO SET UP VARIABLES USED IN Run*.py scripts
import os

##########################################
#### VARIABLES THAT NEED TO BE SET BY USER
##########################################                                                                                                                                                                                                    

##### Set RunALLSamples=True true to simply add ALL samples to production list                                                                                                                                        
RunALLSamples=False
PrivateSample=True

###### If you want to debug you can set KeepWorkDir=True and the dir will not be deleted
KeepWorkDir=False

#### WHAT VERSION OF CATUPLES ARE YOU RUNNING 
version = "v7-6-3"


#### For data only:
###### Set periods to be processed. IF datasampledir is empty then all periods are automatically ran
data_periods = ["C" ]
###### this overwrites sampledir in Run*data*.py (if this is empty ALL datasets are run
datasampledir = ["DoubleMuon"]

                
#### For MC only
mcsampledir = ["tthwA_3l4j_hc130A30_3mu"]


if RunALLSamples:
    datasampledir=[]
    mcsampledir=[]

### Set ouput directory at kisti
cmssw_dir=os.getenv("CMSSW_BASE")
kisti_output_default=cmssw_dir+"/src/CATTools/CommonTools/test/snu/KistiProductionForSNU/"+version+"/"


### Set if you are running full production on kisti site to transfer to snu                                                                                                                                                                  
snu_lqpath="/HeavyNeutrino/13TeV/LQAnalyzer_cat/LQanalyzer/"
username_snu=os.getenv("USER")


##########################################
###### VARIABLES THAT ARE NOT SET BY USER
##########################################
host=os.getenv("HOSTNAME")
k_user=os.getenv("USER") 
latest_version="v7-6-3"

if version != latest_version:
    update = raw_input("You requested to run on old version of catuples. " + latest_version + " is the latest version while you are running on " + version + ". To continue type Y")
    if update != "Y":
        quit()



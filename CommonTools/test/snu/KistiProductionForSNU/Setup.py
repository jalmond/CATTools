### FILE TO SET UP VARIABLES USED IN Run*.py scripts
import os,sys

##########################################
#### VARIABLES THAT NEED TO BE SET BY USER
##########################################                                                                                                                                                                                                    

##### Set RunALLSamples=True true to simply add ALL samples to production list                                                                                                                                        
RunALLSamples=False
PrivateSample=True

####### For now keep these the same  #########
copy_cluster=False
copy_cms1=True
##############################################
#RunALLSamples=False
#PrivateSample=True
#
#
###### If you want to debug you can set KeepWorkDir=True and the dir will not be deleted
KeepWorkDir=False

#### WHAT VERSION OF CATUPLES ARE YOU RUNNING 
version = "v7-6-6"


#### For data only:
###### Set periods to be processed. IF datasampledir is empty then all periods are automatically ran
data_periods = ["C" ]
###### this overwrites sampledir in Run*data*.py (if this is empty ALL datasets are run
datasampledir = ["DoubleMuon"]

                
#### For MC only
mcsampledir = ["hntest"]



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
latest_version="v7-6-6"

if version != latest_version:
    update = raw_input("You requested to run on old version of catuples. " + latest_version + " is the latest version while you are running on " + version + ". To continue type Y")
    if update != "Y":
        quit()


def updateinput(datasetpath, datasetfile, version):
    #os.system('mail  -s "new sample '+ version + '"  jalmond@cern.ch < ' + datasetpath)
    os.system('scp ' + datasetpath + ' ' + username_snu + '@cms3.snu.ac.kr:~/')
    os.system('scp  sendmail.sh  ' + username_snu + '@cms3.snu.ac.kr:~/')
    print "ssh " + username_snu+ "@cms3.snu.ac.kr 'source sendmail.sh'"
    currentdir=cmssw_dir+"/src/CATTools/CommonTools/test/snu/KistiProductionForSNU/"
    forcesend = open(currentdir+"forcesend.sh","w")
    forcesend.write("ssh " + username_snu+ "@cms3.snu.ac.kr 'source sendmail.sh' \n")
    forcesend.write("ssh " + username_snu+ "@cms3.snu.ac.kr 'rm " + datasetfile+ "'\n")
    forcesend.write("ssh " + username_snu+ "@cms3.snu.ac.kr 'rm sendmail.sh'\n")
    forcesend.close()

    os.system("source " + currentdir+"/forcesend.sh")
    os.system("rm "  + currentdir+"/forcesend.sh")

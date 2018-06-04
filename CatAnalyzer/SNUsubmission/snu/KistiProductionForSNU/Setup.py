### FILE TO SET UP VARIABLES USED IN Run*.py scripts
import os,sys

##########################################
#### VARIABLES THAT NEED TO BE SET BY USER
##########################################                                                                                                                                                                                                    

##### Set RunALLSamples=True true to simply add ALL samples to production list                                                                                                                                      
RunALLSamples=False
PrivateSample=True

runSYSTsamples=False

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
version = "v8-0-7"


#### For data only:
###### Set periods to be processed. IF datasampledir is empty then all periods are automatically ran
data_periods = [ ]
###### this overwrites sampledir in Run*data*.py (if this is empty ALL datasets are run
datasampledir = []

                
#### For MC only
mcsampledir = ["WZG","WWG","ttZToLL_M-10","ggHtoWW","vbfHtoWW","qcd_15to20_bctoe","qcd_20to30_bctoe","qcd_30to80_bctoe" ,"qcd_80to170_bctoe","qcd_170to250_bctoe","qcd_250toinf_bctoe","ww_ds"] 
mcsampledir = [ "ggHtoZZ","TG","TTG","tZq","vbhHtoZZ", "ZZTo2L2Nu_Powheg","ZZTo2L2Q_Powheg","ggZZto2e2mu","ggZZto2e2nu","ggZZto2e2tau","ggZZto2mu2nu","ggZZto2mu2tau","ggZZto4e","ggZZto4mu","ggZZto4tau","ggWWto2L2Nu","WZto2L2Q_amcatnlo","QCD_DoubleEMEnriched_30-40_mgg80toinf","QCD_DoubleEMEnriched_30-inf_mgg40to80","QCD_DoubleEMEnriched_40-inf_mgg80toinf" ,"DYtoEE"]




### Loop to run multipl Signal MC
TChannel=False
if TChannel:
    mcsampledir=[]
    hn_mass=["100","200","500","1100"]
    lep_channel=["emem","epep","mummum","mupmup"]
    lep_channel=["mummum"]
    for lch in lep_channel:
        for mass in hn_mass: 
            mcsampledir.append("HNTChannel_"+lch +mass)


PrivateSChannel=False
if PrivateSChannel:
    mcsampledir=[]
    hn_mass=[ "40", "50" , "60", "100", "200","500", "1100","1500" ]
    lep_channel=["EmEm","EmMum","EmMup","EpMum","EpMup","MumEm","MumEp", "MumMum","MumMup","MupEm","MupEp","MupMum","MupMup"]
    lep_channel=["EpEp","EmEp","EpEm"]
    for lch in lep_channel:
        for mass in hn_mass:
            mcsampledir.append("HN"+lch +"_"+mass)

OfficialSChannel=False
if OfficialSChannel:
    mcsampledir=[]
    lep_channel = ["EpMup", "EpEp", "MupMup", "MupEp", "EmMum", "EmEm", "MumMum", "MumEm"]
    
    hn_mass = [ "50", "100", "200", "500", "1100"]
    
    for lch in lep_channel:
        for mass in hn_mass:
            mcsampledir.append("HNMoriondLL"+lch +"_"+mass)


OfficialTChannel=False
if OfficialTChannel:
    mcsampledir=[]
    lep_channel = ["EpMup", "EpEp", "MupMup", "MupEp", "EmMum", "EmEm", "MumMum", "MumEm"]

    hn_mass = [ "100", "200", "500", "1100"]

    for lch in lep_channel:
        for mass in hn_mass:
            mcsampledir.append("HNMoriondLL_Tchannel_"+lch +"_"+mass)



if RunALLSamples:
    datasampledir=[]
    mcsampledir=[]

### Set ouput directory at kisti
cmssw_dir=os.getenv("CMSSW_BASE")
kisti_output_default=str(cmssw_dir)+"/src/CATTools/CatAnalyzer/SNUsubmission/snu/KistiProductionForSNU/"+str(version)+"/"


### Set if you are running full production on kisti site to transfer to snu                                                                                                                                                                  
snu_lqpath="/HeavyNeutrino/13TeV/LQAnalyzer_cat/LQanalyzer/"
username_snu=os.getenv("USER")

##########################################
###### VARIABLES THAT ARE NOT SET BY USER
##########################################
host=os.getenv("HOSTNAME")
k_user=os.getenv("USER") 
latest_version="v8-0-7"

if version != latest_version:
    update = raw_input("You requested to run on old version of catuples. " + latest_version + " is the latest version while you are running on " + version + ". To continue type Y")
    if update != "Y":
        quit()


def updateinput(datasetpath, datasetfile, version):
    #os.system('mail  -s "new sample '+ version + '"  jalmond@cern.ch < ' + datasetpath)
    os.system('scp ' + datasetpath + ' ' + username_snu + '@147.47.242.42:~/')
    os.system('scp  sendmail.sh  ' + username_snu + '@147.47.242.42:~/')
    print "ssh " + username_snu+ "@147.47.242.42 'source sendmail.sh'"
    currentdir=cmssw_dir+"/src/CATTools/CatAnalyzer/SNUsubmission/snu/KistiProductionForSNU/"
    forcesend = open(currentdir+"forcesend.sh","w")
    forcesend.write("ssh " + username_snu+ "@147.47.242.42 'source sendmail.sh' \n")
    forcesend.write("ssh " + username_snu+ "@147.47.242.42 'rm " + datasetfile+ "'\n")
    forcesend.write("ssh " + username_snu+ "@147.47.242.42 'rm sendmail.sh'\n")
    forcesend.close()

    os.system("source " + currentdir+"/forcesend.sh")
    os.system("rm "  + currentdir+"/forcesend.sh")

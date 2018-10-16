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
version = "v8-0-8"


#### For data only:
###### Set periods to be processed. IF datasampledir is empty then all periods are automatically ran
data_periods = [ ]
###### this overwrites sampledir in Run*data*.py (if this is empty ALL datasets are run
datasampledir = []

                
#### For MC only

tmpmcsampledir = ["WZG","WWG","ttZToLL_M-10","ggHtoWW","vbfHtoWW","qcd_15to20_bctoe","qcd_20to30_bctoe","qcd_30to80_bctoe" ,"qcd_80to170_bctoe","qcd_170to250_bctoe","qcd_250toinf_bctoe","ww_ds", "ggHtoZZ","TG","TTG","tZq","vbhHtoZZ", "ZZTo2L2Nu_Powheg","ZZTo2L2Q_Powheg","ggZZto2e2mu","ggZZto2e2nu","ggZZto2e2tau","ggZZto2mu2nu","ggZZto2mu2tau","ggZZto4e","ggZZto4mu","ggZZto4tau","ggWWto2L2Nu","WZto2L2Q_amcatnlo","QCD_DoubleEMEnriched_30-40_mgg80toinf","QCD_DoubleEMEnriched_30-inf_mgg40to80","QCD_DoubleEMEnriched_40-inf_mgg80toinf" ,"DYtoEE",
"TT_powheg",
"TTLL_powheg",
"WJets",
"SingleTop_s",
"SingleTop_t",
"SingleTbar_t",
"SingleTop_tW",
"SingleTbar_tW",
"SingleTop_tW_noHadron",
"SingleTbar_tW_noHadron",
"ZZTo4L_powheg",
"WWTo2L2Nu",
"WWToLNuQQ",
"WZTo3LNu_powheg",
"WpWpQCD",
"WpWpEWK",
"WWW",
"WWZ",
"WZZ",
"ZZZ",
"ttZ",
"ttZ",
"WGtoLNuG",
"ZGto2LG",
"ttH_bb",
"ttH_nonbb",
"GG_HToMuMu",
"VBF_HToMuMu",
"QCD_Pt-15to20_MuEnriched",
"QCD_Pt-20to30_MuEnriched",
"QCD_Pt-30to50_MuEnriched",
"QCD_Pt-50to80_MuEnriched",
"QCD_Pt-80to120_MuEnriched",
"QCD_Pt-120to170_MuEnriched",
"QCD_Pt-170to300_MuEnriched",
"QCD_Pt-300to470_MuEnriched",
"QCD_Pt-470to600_MuEnriched",
"QCD_Pt-600to800_MuEnriched",
"QCD_Pt-800to1000_MuEnriched",
"QCD_Pt-1000toInf_MuEnriched",
"QCD_Pt-20to30_EMEnriched",
"QCD_Pt-30to50_EMEnriched",
"QCD_Pt-50to80_EMEnriched",
"QCD_Pt-80to120_EMEnriched",
"QCD_Pt-120to170_EMEnriched",
"QCD_Pt-170to300_EMEnriched",
"QCD_Pt-300toInf_EMEnriched",
"ZToEE_M_120_200",
"ZToEE_M_1400_2300",
"ZToEE_M_200_400",
"ZToEE_M_2300_3500",
"ZToEE_M_3500_4500",
"ZToEE_M_400_800",
"ZToEE_M_4500_6000",
"ZToEE_M_50_120",
"ZToEE_M_6000_Inf",
"ZToEE_M_800_1400",
"ZToEE_M_120_200",
"ZToMuMu_M_1400_2300",
"ZToMuMu_M_200_400",
"ZToMuMu_M_2300_3500",
"ZToMuMu_M_3500_4500",
"ZToMuMu_M_400_800",
"ZToMuMu_M_4500_6000",
"ZToMuMu_M_50_120",
"ZToMuMu_M_6000_Inf",
"ZToMuMu_M_800_1400",
"DY_pt_0to50",                                                                                                                                                                                             
"DY_pt_50to100",                                                                                                                                                                                           
"DY_pt_100to250",                                                                                                                                                                                          
"DY_pt_250to400",                                                                                                                                                                                        
"DY_pt_400to650",                                                                                                                                                                                          
"DY_pt_650toinf"   ]


flavours = ["MuMu"]
zmasses = ["400", "600", "800", "1000", "1200", "1400", "1600", "1800", "2000", "2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000", "4200", "4400", "4600", "4800", "5000"]
nmasses  = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000", "1100", "1200", "1300", "1400", "1500", "1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400"]

tmpmcsampledir = []                                                                                                                                                                                           
for flavour in flavours:
    for zmass in zmasses:
        for nmass in nmasses:
            if (((float)(zmass)/2. - 1) < ((float)(nmass))):
                continue
            tmpmcsampledir.append("ZprimetoNN_WR_"+flavour+"_Z"+zmass+"_N"+nmass)                                                                                                                             

tmpmcsampledir  =["QCD_Pt-300toInf_EMEnriched"]

mcsampledir=[]

for x in tmpmcsampledir:    
    sample_exists=False
    if not os.path.exists("/cms/scratch/SNU/datasets_v8-0-8/dataset_"+x+".txt"):
        print x + " does not exist"
        continue

    check_exists = open("/cms/scratch/SNU/datasets_v8-0-8/dataset_"+x+".txt","r")

    for l in check_exists:
        if ".root" in l:
            sample_exists=True
    check_exists.close()
    
    if not sample_exists:
        print x + " does not exist"
    else:
        mcsampledir.append(x)
        


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
latest_version="v8-0-8"

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

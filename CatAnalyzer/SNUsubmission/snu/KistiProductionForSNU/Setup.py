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


import os,sys
import ROOT

def GetListOfDataSets(catversion, sample_type):

    dlist=[]

    if sample_type == "mc":
        url = 'https://docs.google.com/spreadsheets/d/e/2PACX-1vTWct60gAWZoN7tmVAw_WJmQmDWdrpXluF1xz8cn4sL8NCll3Vb8ppN2254P10zmnZ_oqF1f8XSHuav/pub?gid=238920808&single=true&output=csv'
    if sample_type == "signal":
        url = 'https://docs.google.com/spreadsheets/d/e/2PACX-1vTWct60gAWZoN7tmVAw_WJmQmDWdrpXluF1xz8cn4sL8NCll3Vb8ppN2254P10zmnZ_oqF1f8XSHuav/pub?gid=602011091&single=true&output=csv'

    import csv

    from urllib import urlopen

    cr = csv.reader(urlopen(url).readlines())
    for row in cr:
        if row[0] == "END":
            return dlist
        if not row[0] == "User":
            if len(row) > 3:
                len_alias = len(row[1])
                len_dn = len(row[2])
                len_xsec = len(row[3])
                len_1 = 30 - len_alias
                len_2 = 180 - len_dn
                len_3 = 10 - len_xsec
                #print "[Alias,dataset,xsec]: " + row[1]+ " "*len_1+ row[2] +" "*len_2 +row[3]                                                                                                              

                if row[0] == "":
                    dlist.append([row[1],row[2],row[3],"jalmond"])
                else:
                    dlist.append([row[1],row[2],row[3],row[0]])




    return dlist


catversion="v8-0-8"
datasetlist=GetListOfDataSets(catversion,"mc")
datasetlist_signal=GetListOfDataSets(catversion,"signal")
datasetlist=datasetlist+datasetlist_signal


rerunALL=False

mcsampledir=[]
for x in datasetlist:    
    if x == ['','','','']:
        continue
    
    alias = x[0]
    datasetname = x[1]
    part_datasetname=  datasetname

    if "/" in datasetname:
        datasetname=datasetname.replace("/"," " )
        datasetname_split= datasetname.split()
        if len(datasetname_split) > 0:
            part_datasetname= datasetname_split[0]


    user_name= x[3]
    if not os.path.exists("//xrootd/store/user/"+user_name+"/"+part_datasetname):
        print part_datasetname + " missing fom cattool list, cannot make flatcat"
        continue
    sample_exists=False
    if os.path.exists("/xrootd/store/user/"+user_name+"/cattoflat/MC/"+catversion+"/"+part_datasetname):
        listOfFile = os.listdir("/xrootd/store/user/"+user_name+"/cattoflat/MC/"+catversion+"/"+part_datasetname)
        for entry in listOfFile:
            if ".root" in entry:
                sample_exists=True    
        continue
    else:
        print "/xrootd/store/user/"+user_name+"/cattoflat/MC/"+catversion+"/"+part_datasetname + " missing"
    if not sample_exists:
        print alias+ " does not exist"
        mcsampledir.append(alias)
        

if rerunALL:
    mcsampledir=[]

    for x in datasetlist:
        if x == ['','','','']:
            continue
    
        alias = x[0]
        datasetname = x[1]
        part_datasetname=  datasetname

        if "/" in datasetname:
            datasetname=datasetname.replace("/"," " )
            datasetname_split= datasetname.split()
            if len(datasetname_split) > 0:
                part_datasetname= datasetname_split[0]
                
        sample_exists=False
        if not os.path.exists("/cms/scratch/SNU/datasets_v8-0-8/dataset_"+alias+".txt"):
            print alias + " does not exist"
            continue
        
        if not sample_exists:
            print alias+ " does not exist"
            mcsampledir.append(alias)

        check_exists = open("/cms/scratch/SNU/datasets_v8-0-8/dataset_"+alias+".txt","r")
        for l in check_exists:
            if ".root" in l:
                sample_exists=True
        check_exists.close()
        if  sample_exists:
            mcsampledir.append(alias)

    
for x in mcsampledir:
    print x
    


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

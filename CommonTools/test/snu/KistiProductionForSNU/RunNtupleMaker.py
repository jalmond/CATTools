import os, getpass, sys
import time
from functions import *
from Setup import *


######## True runs default list of all samples available
ALLSamples= False
if len(mcsampledir) == 0:
    ALLSamples=True
    
####### make sure this file is being run at kisti                                                                                                                                                                                           
host=os.getenv("HOSTNAME")
if not "ui10" in host:
    quit()

if os.path.exists("cat.txt"):
    os.system("rm cat.txt")
os.system("source /cms/home/jalmond/Cattuples/cat76/cattools/src/CATTools/CommonTools/test/snu/catversion.sh > cat.txt")


catfile = open("cat.txt",'r')
vcat=""
for line in catfile:
    vcat = line    
    if vcat == "":
        print "version not set"
        quit()
    if not "v7-" in vcat:
        print "version does not have v7- in name " 
        quit()

print "Cat version = " + version 


if not user in kisti_output_default:
    print "kisti_output_default should container username in the path. Fix this."
    quit()

if not (os.path.exists(kisti_output_default)):
    os.system("mkdir " + kisti_output_default)
    if not (os.path.exists(kisti_output_default)):
        print "Problem making directory for kisti_output_default. Process is quitting. Before running again type"
        print "mkdir " + kisti_output_default
        quit()
else:
    os.system("rm -r " + kisti_output_default)
    os.system("mkdir " + kisti_output_default)

print "Output directory is " + kisti_output_default        

if (MakeSKTrees == True) or (RunSkims==True):
## Check Branch for SKtrees is up to date to make skims
    os.system("ssh " +  username_snu +"@cms3.snu.ac.kr cat /home/" + username_snu+ snu_lqpath + "/bin/Branch.txt > check_snu_branch.txt")
    os.system("ssh " +  username_snu +"@cms3.snu.ac.kr cat /home/" + username_snu+ snu_lqpath + "/bin/CATVERSION.txt > check_catversion_branch.txt")
    snubranch = open("check_snu_branch.txt",'r')
    snu_br_uptodate=False

    for line in snubranch:
        if version in line:
            snu_br_uptodate=True

    snu_cat_uptodate=False
    snucat = open("check_catversion_branch.txt",'r')
    for line in snucat:
        if version in line:
            snu_cat_uptodate=True
            
    if snu_br_uptodate == False:
        print "Branch on snu is not compatable with " + version + " please update snu branch first"
        quit()

    if snu_cat_uptodate== False:
        print "CATVERSION on snu is not compatable with " + version + " please update cat version first"
        quit()

    os.system("rm check_catversion_branch.txt")
    os.system("rm check_snu_branch.txt")
    if MakeSKTrees == True:
        os.system("ls /tmp/ > check_snu_connection.txt")
        snu_connect = open("check_snu_connection.txt",'r')
        connected_cms4=False
        for line in snu_connect:
            if "ssh-jalmond@cms4" in line:
                connected_cms4=True
        os.system("rm check_snu_connection.txt")
        if connected_cms4 == False:
            print "No connection to cms3: please make connection in screen and run script again"
            quit()
    

os.system("ls /tmp/ > check_snu_connection.txt")
snu_connect = open("check_snu_connection.txt",'r')
connected_cms3=False
for line in snu_connect:
    if "ssh-jalmond@cms3" in line:
        connected_cms3=True
            
os.system("rm check_snu_connection.txt")    
if connected_cms3 == False:    
    print "No connection to cms3: please make connection in screen and run script again"
    quit()



## Make a list of samples to process

sampledir = ["WZ_TuneCUETP8M1_13TeV-pythia8", 
             "WJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8",
             "TTJets_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
             "ZZ_TuneCUETP8M1_13TeV-pythia8",
             "WW_TuneCUETP8M1_13TeV-pythia8",
             "WpWpJJ_EWK_TuneCUETP8M1_13TeV-madgraph-pythia8",
             "WW_DoubleScattering_13TeV-pythia8",
             "WZZ_TuneCUETP8M1_13TeV-amcatnlo-pythia8",
             "DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8",
             "DYJetsToLL_M-10to50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8",
             "TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8", 
             "TTZToQQ_TuneCUETP8M1_13TeV-amcatnlo-pythia8", 
             "TTWJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8",
             "TTWJetsToQQ_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8", 
             "ttHTobb_M125_13TeV_powheg_pythia8", 
             "ttHToNonbb_M125_13TeV_powheg_pythia8", 
             "GluGlu_HToMuMu_M125_13TeV_powheg_pythia8",
             "VBF_HToMuMu_M125_13TeV_powheg_pythia8",
             "QCD_Pt-300toInf_EMEnriched_TuneCUETP8M1_13TeV_pythia8", 
             "QCD_Pt-120to170_EMEnriched_TuneCUETP8M1_13TeV_pythia8", 
             "QCD_Pt-170to300_EMEnriched_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt-470to600_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt-80to120_EMEnriched_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt-50to80_EMEnriched_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt-20to30_EMEnriched_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt-800to1000_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt-20to30_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt-20toInf_MuEnrichedPt15_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt-170to300_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt-30to50_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt-300to470_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt-50to80_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt-80to120_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt-120to170_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8",
             "GJet_Pt-20to40_DoubleEMEnriched_MGG-80toInf_TuneCUETP8M1_13TeV_Pythia8",
             "GJet_Pt-40toInf_DoubleEMEnriched_MGG-80toInf_TuneCUETP8M1_13TeV_Pythia8",
             "TTGJets_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8",
             "WGToLNuG_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
             "ZGTo2LG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8",
             "ST_t-channel_top_4f_leptonDecays_13TeV-powheg-pythia8_TuneCUETP8M1",
             "ST_t-channel_antitop_4f_leptonDecays_13TeV-powheg-pythia8_TuneCUETP8M1",
             "ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1",
             "ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1",
             "ST_s-channel_4f_leptonDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1",
             "TTJets_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8",
             "QCD_Pt-40toInf_DoubleEMEnriched_MGG-80toInf_TuneCUETP8M1_13TeV_Pythia8",
             "QCD_Pt-30toInf_DoubleEMEnriched_MGG-40to80_TuneCUETP8M1_13TeV_Pythia8",
             "QCD_Pt-30to40_DoubleEMEnriched_MGG-80toInf_TuneCUETP8M1_13TeV_Pythia8",
             "QCD_Pt_20to30_bcToE_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt_80to170_bcToE_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt_15to20_bcToE_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt_30to80_bcToE_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt_170to250_bcToE_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt_250toInf_bcToE_TuneCUETP8M1_13TeV_pythia8",
             "DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
             "ZZTo4L_13TeV_powheg_pythia8",
             "WZJets_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8",
             "ZZTo4L_13TeV-amcatnloFXFX-pythia8",
             "WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8",
             "TT_TuneCUETP8M1_13TeV-powheg-pythia8"]


sampledir=[ "WpWpJJ_QCD_TuneCUETP8M1_13TeV-madgraph-pythia8",
            "QCD_Pt-600to800_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8",
            "QCD_Pt-1000toInf_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8"     ,
            "QCD_Pt-30to50_EMEnriched_TuneCUETP8M1_13TeV_pythia8",
            "QCD_Pt_30to80_bcToE_TuneCUETP8M1_13TeV_pythia8",
                         "QCD_Pt_15to20_bcToE_TuneCUETP8M1_13TeV_pythia8"]
                
 
#samples with fullgen entries in the name will store all gen information. All others will store just first 30 gen particles
fullgen = ["DY", "TTJets"]


if not ALLSamples == True:
    sampledir = mcsampledir

# njob set to 40: if n root files < 40 njobs = #rootfiles
njob=60
    
skip_first=0
samples_processed=0
for i in sampledir:
    samples_processed=samples_processed+1 
    if samples_processed < skip_first+1:
        continue

    runfullgen = False
    for j in fullgen:
        if j in i:
            runfullgen = True
        

    njob=50
    output=i
    kisti_output=kisti_output_default+output+"/"
    print "Making dir: " + kisti_output

    if not (os.path.exists(kisti_output)):
        os.system("mkdir " + kisti_output)
    else:
        os.system("rm " + kisti_output + "/*")
    
    os.system("xrd cms-xrdr.sdfarm.kr ls /xrd/store/group/CAT/" + output + " > " + kisti_output+ "/"+ output + "_getversion.txt")
    os.system("sed -r 's/^.{43}//' " +  kisti_output+ "/"+output + "_getversion.txt  > " +kisti_output+ "/"+output + "_getversion_skim.txt")

    fr_1end = open(kisti_output+ "/"+output+"_getversion_skim.txt",'r')
    versionpath =""
    iline_version=0
    for linerp in fr_1end:
        if version in linerp:
            if iline_version < 1:
                s = linerp.replace("/", " ")
                splitline  = s.split()
                versionpath = splitline[5]
            iline_version= iline_version+1
    fr_1end.close()

    os.system("xrd cms-xrdr.sdfarm.kr ls /xrd/store/group/CAT/" + output + "/" + versionpath + " > " + kisti_output+ "/"+ output + ".txt")

    os.system("sed -r 's/^.{43}//' " +  kisti_output+ "/"+output + ".txt  > " + kisti_output + "/"+output + "_skim.txt") 
    os.system("cut -d/ -f 8 " + kisti_output+ "/"+output  + "_skim.txt  > " + kisti_output + "/"+output + "_end.txt")
    


    ## Get the tag of the production: using the newest tag of version, it is automatic
    fr_end = open(kisti_output+ "/"+output+"_end.txt",'r')
    tagpath =""
    iline=0
    newest_check = 0
    check_date=0
    check_yr=0
    check_m=0
    check_d=0
    check_tag=0
    sample_exists=0
    for linerp in fr_end:
        tag=linerp.strip()
        if "_" in tag:
            sample_exists=1
            rmstr = len(tag) - 7
            split_tag_date = tag[:rmstr]
            split_tag_tag = tag[7:]
            split_tag_date_yr = split_tag_date[:4]
            split_tag_date_m = split_tag_date[2:2]
            split_tag_date_d = split_tag_date[4:]
            if split_tag_date_yr >= check_yr:
                if split_tag_date_yr > check_yr:
                    check_tag=0
                if split_tag_date_m >= check_m:
                    if split_tag_date_m >= check_m:
                        check_tag=0
                    if split_tag_date_d >= check_d:
                        if split_tag_date_d > check_d:
                            check_tag=0
                        if split_tag_tag > check_tag:
                            tagpath = linerp.strip()
                            check_yr=split_tag_date_yr
                            check_m=split_tag_date_m
                            check_d=split_tag_date_d
                            check_tag=split_tag_tag
    fr_end.close()
    print "tagpath = " + tagpath
    

    if sample_exists == 0:
        continue

    os.system("xrd cms-xrdr.sdfarm.kr ls /xrd/store/group/CAT/" + output  + "/" + versionpath + "/" + tagpath + "/0000/ > " + kisti_output+ "/"+output + "_tmpfull.txt")
    os.system("sed -r 's/^.{43}//' " +  kisti_output+ "/"+output  + "_tmpfull.txt  > " +kisti_output+ "/"+output  + "_full.txt")

    ## Set the number of jobs and files per job
    fr = open(kisti_output+ "/"+output +"_full.txt",'r')
    count=0
    nfilesperjob=0
    for line in fr:
        if ".root" in line:
            count+=1
    fr.close()

    if njob > count:
        njob=count
    for j in range(1,count+1):
        if not j%njob:
            nfilesperjob+=1
            
    print "Number of jobs to process is " + str(njob)            

    files_torun= nfilesperjob*njob
    remainder= count - files_torun

    print "Each job will process  " + str(nfilesperjob) + "/" + str(count) + " files"

    counter_written=0
    for j in range(1,njob+1):
    
        wfr = open(kisti_output+ "/"+output + "_" +str(j) + "_full.txt",'w')
        counter=0
        rfr  = open(kisti_output+ "/"+output + "_full.txt",'r')
        for line in rfr:
            if ".root" in line:
                counter+=1
            
                if j == njob:
                    if counter > (j-1)*nfilesperjob:
                        if counter <= (j)*nfilesperjob:
                            wfr.write(line)
                            counter_written+=1
                        
                else:
                    if counter > (j-1)*nfilesperjob:
                        if counter <= j*nfilesperjob:
                            wfr.write(line)
                            counter_written+=1


        wfr.close()
        rfr.close()    

    for j in range(1,remainder+1):
         wfr = open(kisti_output+ "/"+output + "_" +str(j) + "_full.txt",'a')
         rfr  = open(kisti_output+ "/"+output + "_full.txt",'r')
         counter = 0
         for line in rfr:             
             if ".root" in line:
                  counter+=1
                  if counter == njob*nfilesperjob + j:
                      wfr.write(line)           

         wfr.close()
         rfr.close()

    for j in range(1,njob+1):
        runscript=output + "_flatntupleMaker_"+str(j) +".py"
        configfile=open(kisti_output+ "/" + runscript,'w')
        configfile.write(makeNtupleMaker(output,kisti_output+ "/"+output + "_" + str(j) + "_full.txt", kisti_output,j, runfullgen))
        configfile.close()
    
        log = kisti_output + "/Job_" + str(j) + ".log"
        runcommand="cmsRun " + kisti_output + "/" + runscript + "&>" + log + "&"
        os.system(runcommand)
        
        
    import platform
    check_job_finished=0
    while check_job_finished == 0:        
        os.system("ps ux  | grep 'cmsRun' &> " + kisti_output + "/pslog")
        print "ps ux  | grep 'cmsRun' &> " + kisti_output + "/pslog"
        filename = kisti_output +'/pslog'    
        n_previous_jobs=0
        pslogfile = open(filename, 'r')
        for line in pslogfile:
            if not "grep" in line:
                n_previous_jobs+=1
        pslogfile.close()    
        if n_previous_jobs > 0:
            check_job_finished=0
        else:
            check_job_finished=1
            print "ssh " + username_snu  + "@cms3.snu.ac.kr mkdir /data2/DATA/cattoflat/MC/" + version + "/" + i 
            os.system("ssh " + username_snu  + "@cms3.snu.ac.kr rm -r /data2/DATA/cattoflat/MC/" + version +"/" + i )
            os.system("ssh " + username_snu  + "@cms3.snu.ac.kr mkdir /data2/DATA/cattoflat/MC/" + version)
            os.system("ssh " + username_snu  + "@cms3.snu.ac.kr mkdir /data2/DATA/cattoflat/MC/" + version + "/" + i )
            os.system("ssh " + username_snu  + "@cms3.snu.ac.kr chmod -R 777 /data2/DATA/cattoflat/MC/" + version)
 

            print "scp " +kisti_output + "/*.root " + " " + username_snu  + "@cms3.snu.ac.kr:/data2/DATA/cattoflat/MC/" + version + "/"  +i
            os.system("scp " +kisti_output + "/*.root " + username_snu  + "@cms3.snu.ac.kr:/data2/DATA/cattoflat/MC/"  + version + "/" +i) 

        os.system("rm " + kisti_output + "/pslog")        
        time.sleep(30.) 
        
    if KeepWorkDir != True:
        os.system("rm -r " + kisti_output)


if ALLSamples == True:
    os.system("source runEffLumi.sh")
if RunSkims == True:
    os.system("source runSkimMC.sh&")
if MakeSKTrees == True:
    os.system("source runMCSKTree.sh&")


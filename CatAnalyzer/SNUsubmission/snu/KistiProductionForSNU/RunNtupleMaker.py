#!/usr/bin/python

import os, getpass, sys
import time
from functions import *
from Setup import *
import string
from CheckDatasetFile import *

import datetime


timestamp=datetime.datetime.now()
timestamp=(str(timestamp).split())
timestamp=timestamp[0]

if os.path.exists("outputlog.txt"):
    os.system("rm outputlog.txt")


def CheckJobStatusAfterCrash(dirname, v):
    
    outlogfile = open( "outputlog.txt", "w")

    if not os.path.exists(dirname):
        outlogfile.close()    
        return
    
    if os.path.exists(dirname + "/check_list.txt"):
        os.system("rm " + dirname +"/check_list.txt")

    outlogfile.write("ls " + dirname + "/* > " + dirname+ "/check_list.txt\n")
    
    os.system("ls " + dirname + "/* > " + dirname+ "/check_list.txt")
    logfile = open( dirname+"/check_list.txt", "r")
    njobs_x=0
    for i in logfile:
        if "_cfg.py" in i:
            njobs_x = njobs_x +1
    
            
    outlogfile.write("Number of jobs =  " + str(njobs_x)+"\n")
    os.system("rm " + dirname+"/check_list.txt")

    if os.path.exists(dirname+"/check_finished.txt"):
        os.system("rm " + dirname +"/check_finished.txt")
    os.system("grep FINISHED " + dirname + "/*.log > " + dirname +"/check_finished.txt")
    nfinishedjobs=0
    for j in range(0, njobs_x):
        jobcheck= "job_" + str(j) +".log"
        checkfile = open(dirname+"/check_finished.txt", "r")
        for line in checkfile:
            if jobcheck in line:
                nfinishedjobs=nfinishedjobs+1

    outlogfile.write("Number of finished jobs = " + str(nfinishedjobs)+"\n")
    os.system("rm " + dirname +"/check_finished.txt")
    
    if nfinishedjobs != njobs_x:
        "Job is running already. Adding to list"
        outlogfile.close()    
        return True
    else: 
        outlogfile.write("Removing dir since no jobs are running"+"\n")
        os.system("rm -r " + dirname)
        outlogfile.close()    
        return True
    
    outlogfile.close()    
    return False

def CheckFailedJobStatus(submitted_list, v, flist):
    mod_list =string.replace(submitted_list,"!", " ")
    split_list = mod_list.split()
    outlogfile = open( "outputlog.txt", "w")

    outlogfile.write("List of jobs submitted = "+"\n")
    for x in split_list:
         outlogfile.write("x"+"\n")
    
    outlogfile.write("Checking submitted jobs:"+"\n")

    failed_list=flist

    anyFailed=False
    for i in split_list:
        outlogfile.write("Sample " + i+"\n")
        if os.path.exists("SNU_" + v+ "_" +i +"/check_list.txt"):
            os.system("rm SNU_" + v+ "_" +i +"/check_list.txt")
        
        os.system("ls SNU_" + v + "_" + i +"/* >  SNU_" + v+ "_" +i +"/check_list.txt")
        
        logfile = open( "SNU_" + v+ "_" +i +"/check_list.txt", "r")
        njobs_x=0
        for j in logfile:
            if "_cfg.py" in j:
                njobs_x = njobs_x +1
        outlogfile.write("Number of jobs =  " + str(njobs_x)+"\n")

        os.system("rm SNU_" + v+ "_" +i +"/check_list.txt"+"\n")
        os.system("grep TERMINATED " + "SNU_" + v+ "_" +i +"/*.log > SNU_" + v+ "_" +i +"/check_finished2.txt")

        if os.path.exists("SNU_" + v+ "_" +i +"/check_finished.txt"):
            os.system("rm SNU_" + v+ "_" +i +"/check_finished.txt")
        os.system("grep FINISHED SNU_" + v+ "_" +i +"/*.log > SNU_" + v+ "_" +i +"/check_finished.txt")
        nfinishedjobs=0
        for j in range(0, njobs_x):
            jobcheck= "job_" + str(j) +".log"
            checkfile = open("SNU_" + v+ "_" +i +"/check_finished.txt", "r")
            for line in checkfile:
                if jobcheck in line:
                    nfinishedjobs=nfinishedjobs+1
        outlogfile.write("Number of finished jobs = " + str(nfinishedjobs)+"\n")

        os.system("rm SNU_" + v+ "_" +i +"/check_finished.txt")

        if int(nfinishedjobs) != int(njobs_x):
            for k in range(0, njobs_x):
                jobcheck2= "job_" + str(k) +".log"
                checkfile2 = open("SNU_" + v+ "_" +i +"/check_finished2.txt", "r")
                for line2 in checkfile2:
                    if jobcheck2 in line2:
                        failtag=i + "_" + str(k) +"!"
                        if not failtag in failed_list:
                            failed_list = failed_list + i + "_" + str(k) +"!"
                            anyFailed=True
                        
        os.system("rm SNU_" + v+ "_" +i +"/check_finished2.txt")
    if anyFailed == True:
        outlogfile.write("Some jobs have failed. Full list printed at end"+"\n")

    outlogfile.close()            
    return failed_list   


def CheckJobStatus(submitted_list, v):
    
    outlogfile = open( "outputlog.txt", "w")

    mod_list =string.replace(submitted_list,"!", " ")
    split_list = mod_list.split()
    
    outlogfile.write("List of jobs submitted = "+"\n")

    for x in split_list:
        outlogfile.write(x+"\n")

    outlogfile.write("Checking submitted jobs:"+"\n")

    new_submitted_list=submitted_list
    for i in split_list:
        outlogfile.write("Sample " + i +"\n")
         
        if os.path.exists("SNU_" + v+ "_" +i +"/check_list.txt"):
            os.system("rm SNU_" + v+ "_" +i +"/check_list.txt")

        os.system("ls SNU_" + v + "_" + i +"/* >  SNU_" + v+ "_" +i +"/check_list.txt")

        logfile = open( "SNU_" + v+ "_" +i +"/check_list.txt", "r")
        njobs_x=0
        for j in logfile:
            if "_cfg.py" in j:
                njobs_x = njobs_x +1
        outlogfile.write( "Number of jobs =  " + str(njobs_x)+"\n")

        os.system("rm SNU_" + v+ "_" +i +"/check_list.txt")

        if os.path.exists("SNU_" + v+ "_" +i +"/check_finished.txt"):
            os.system("rm SNU_" + v+ "_" +i +"/check_finished.txt")

        os.system("grep FINISHED " + "SNU_" + v+ "_" +i +"/*.log > SNU_" + v+ "_" +i +"/check_finished.txt")
        os.system("grep TERMINATED " + "SNU_" + v+ "_" +i +"/*.log > SNU_" + v+ "_" +i +"/check_finished2.txt")
        
        nfinishedjobs=0
        for k in range(0, njobs_x):
            jobcheck= "job_" + str(k) +".log"
            checkfile = open("SNU_" + v+ "_" +i +"/check_finished.txt", "r")
            for line in checkfile:
                if jobcheck in line:
                    nfinishedjobs=nfinishedjobs+1

        outlogfile.write("Number of finished jobs = " + str(nfinishedjobs)+"\n")

        FailedJobs=False
        if int(nfinishedjobs) != int(njobs_x):
            for k in range(0, njobs_x):
                jobcheck2= "job_" + str(k) +".log"
                checkfile2 = open("SNU_" + v+ "_" +i +"/check_finished2.txt", "r")
                for line2 in checkfile2:
                    if jobcheck2 in line2:
                        FailedJobs=True
                        nfinishedjobs=nfinishedjobs+1
                    
        os.system("rm SNU_" + v+ "_" +i +"/check_finished.txt")


        if int(nfinishedjobs) == int(njobs_x):
            outlogfile.write("Job " + i + " is finished. Copying to SNU"+"\n")
            
            if FailedJobs == True:
                for k in range(0, njobs_x):
                    jobcheck2= "job_" + str(k) +".log"
                    checkfile2 = open("SNU_" + v+ "_" +i +"/check_finished2.txt", "r")
                    for line2 in checkfile2:
                        if jobcheck2 in line2:
                            outlogfile.write("Deleting root file of failed job" +"\n")

                            os.system("rm SNU_" + v+ "_" +i +"/ntuple_" +  str(k) + ".root")

            
            if copy_cluster:
                outlogfile.write("ssh " + username_snu  + "@147.47.242.67 mkdir  /data4/DATA/FlatCatuples/MC/" + v + "/" + i+"\n")
                os.system("ssh " + username_snu  + "@147.47.242.67 rm -r  /data4/DATA/FlatCatuples/MC/" + v +"/" + i )
                os.system("ssh " + username_snu  + "@147.47.242.67 mkdir  /data4/DATA/FlatCatuples/MC/" + v)
                os.system("ssh " + username_snu  + "@147.47.242.67 mkdir  /data4/DATA/FlatCatuples/MC/" + v + "/" + i )
                
                outlogfile.write("scp SNU_" + v+ "_" +i +"/*.root " + " " + username_snu  + "@147.47.242.67:/data4/DATA/FlatCatuples/MC/" + v + "/"  +i+"\n")
                os.system("scp SNU_" + v+ "_" +i +"/*.root " + username_snu  + "@147.47.242.67:/data4/DATA/FlatCatuples/MC/"  + v + "/" +i)


            if copy_cms1:
                outlogfile.write("ssh " + username_snu  + "@147.47.242.42 mkdir /data2/DATA/cattoflat/MC/" + v + "/" + i+"\n")
                os.system("ssh " + username_snu  + "@147.47.242.42 rm -r /data2/DATA/cattoflat/MC/" + v +"/" + i )
                os.system("ssh " + username_snu  + "@147.47.242.42 mkdir /data2/DATA/cattoflat/MC/" + v)
                os.system("ssh " + username_snu  + "@147.47.242.42 mkdir /data2/DATA/cattoflat/MC/" + v + "/" + i )

                outlogfile.write("scp  /xrootd/store/user/" + k_user + "/flatcat/SNU_" + v+ "_" +i +"/*.root " + " " + username_snu  + "@147.47.242.42:/data2/DATA/cattoflat/MC/" + v + "/"  +i+"\n")
                os.system("scp  /xrootd/store/user/" + k_user + "/flatcat/SNU_" + v+ "_" +i +"/*.root " + username_snu  + "@147.47.242.42:/data2/DATA/cattoflat/MC/"  + v + "/" +i) 
             
            outlogfile.write("submitted_list = " + submitted_list + " is ammended to: "+"\n")
            new_submitted_list = string.replace(submitted_list, i+"!" , "")
            outlogfile.write(new_submitted_list+"\n")
            outlogfile.write("Deleting directory SNU_" + v+ "_" +i +"/"+"\n")
            print "mv   SNU_" + v+ "_" +i +"/  SNU_" + v+ "_" +i + str(timestamp)+"/"
            os.system("mv   SNU_" + v+ "_" +i +"/  SNU_" + v+ "_" +i + str(timestamp)+"/")
            outlogfile.close()    
            return new_submitted_list
                                      
        os.system("rm SNU_" + v+ "_" +i +"/check_finished2.txt")

    outlogfile.close()    
    return new_submitted_list


outlogfile = open( "outputlog.txt", "w")

######## True runs default list of all samples available
ALLSamples= False
if len(mcsampledir) == 0:
    ALLSamples=True
    

host=os.getenv("HOSTNAME")
if not "ui10" in host:
    quit()

if os.path.exists("cat.txt"):
    os.system("rm cat.txt")

if not os.path.exists(cmssw_dir+"/src/CATTools/CatAnalyzer/SNUsubmission/snu/KistiProductionForSNU/jobcheck"):
    os.system("mkdir " + str(cmssw_dir)+"/src/CATTools/CatAnalyzer/SNUsubmission/snu/KistiProductionForSNU/jobcheck")

if not "v8-" in version:
    print "cat version does not have v8- in name " 
    quit()

print "Cat version = " + version 


if not k_user in kisti_output_default:
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


os.system("cat ~/.ssh/config > check_connection.txt")

ch_connect = open("check_connection.txt",'r')

cpath="/tmp/"
for line in ch_connect:
    if "ControlPath" in line:
        if "~/ssh" in line:
            cpath="~/"
        elif "/tmp/" in line:
            cpath="/tmp/"
        else:
            print "Modify the cms21 connection since  ControlPath in ~/.ssh/cofig is set to something other than tmp or home dir"
            
ch_connect.close()
os.system("rm check_connection.txt")


os.system("ls " + cpath + " > check_snu_connection.txt")
snu_connect = open("check_snu_connection.txt",'r')
connected_cms3=False
connected_cluster=False
for line in snu_connect:
    if "ssh-"+k_user+"@147.47.242.42" in line:
        connected_cms3=True
    if "ssh-"+k_user+"@147.47.242.67" in line:
        connected_cluster=True

            
os.system("rm check_snu_connection.txt")    
if connected_cms3 == False:    
    print "No connection to cms3: please make connection in screen and run script again"
    quit()

if copy_cluster:
    if connected_cluster == False:
        print "No connection to snu cluster: please make connection in screen and run script again"
        quit()


## Make a list of samples to process
validation_sampledir=["DYJets" , "DYJets_10to50","TT_powheg", "TTLJ_powheg","TTLL_powheg","WW","WZ", "ZZ", "WWW","WZZ","WWZ","ZZZ", "SingleTbar_t","SingleTbar_tW",  "SingleTbar_tW_noHadron","SingleTop_tW_noHadron","ttH_nonbb", "ttH_bb","WJets",  "WJets_MG", "TTJets_aMC" , "WZTo3LNu_powheg", "WpWpEWK","WpWpQCD","ZZTo4L_powheg", "ttW","ttZ" ,"GG_HToMuMu","VBF_HToMuMu", "QCD_Pt-1000toInf_MuEnriched","QCD_Pt-120to170_EMEnriched","QCD_Pt-120to170_MuEnriched","QCD_Pt-15to20_MuEnriched",    "QCD_Pt-170to300_EMEnriched","QCD_Pt-170to300_MuEnriched","QCD_Pt-20to30_EMEnriched","QCD_Pt-20to30_MuEnriched","QCD_Pt-300to470_MuEnriched","QCD_Pt-300toInf_EMEnriched",     "QCD_Pt-30to50_EMEnriched","QCD_Pt-30to50_MuEnriched","QCD_Pt-470to600_MuEnriched","QCD_Pt-50to80_EMEnriched","QCD_Pt-50to80_MuEnriched","QCD_Pt-600to800_MuEnriched", "QCD_Pt-800to1000_MuEnriched","QCD_Pt-80to120_EMEnriched","QCD_Pt-80to120_MuEnriched",  "WWTo2L2Nu","WWToLNuQQ", "SingleTop_s", "SingleTop_t","SingleTop_tW", "WGtoLNuG", "ttZToLL_M-1to10","ttWToLNu","WgstarToLNuEE","WgstarToLNuMuMu","ZGto2LG" , "WZTo3LNu_amcatnlo", "DYJets_MG_10to50", "DYJets_MG","ttbb","TTJets_MG", "TTTT" ]
                      

runSYSTsamples=False
if runSYSTsamples:
    validation_sampledir += ["TTLL_powheg_scaledown","TTLL_powheg_scaleup","TT_powheg_mtop1695","TT_powheg_mtop1755", "TTLJ_powheg_scaledown","TTLJ_powheg_scaleup"]


duplicate_samples=False
samplelist_string=""
for icheck in validation_sampledir:
    additionstring="_"+icheck+"_:"
    if additionstring in samplelist_string:
        print "Sample " + icheck + " is in list twice"
        quit()
    else:   
        samplelist_string+="_"+icheck+"_:"
    

sampledir=validation_sampledir

if ALLSamples == True:
    for i in sampledir:
        tmpdatasetpath = "/cms/scratch/SNU/datasets_" +version + "/dataset_" + i + ".txt"
        if not os.path.exists(tmpdatasetpath):
            print tmpdatasetpath + " does not exist"
            quit()
            
#, "HN_ee_40", "HN_ee_100", "HN_ee_500", "HN_ee_1500","HN_mm_40","HN_mm_100","HN_mm_500","HN_mm_1500"]
            
#sampledir = ["HN_ee_schan_lll_ss_40" , "HN_ee_schan_lll_ss_100", "HN_ee_schan_lll_ss_500", "HN_ee_schan_lll_ss_1500" ,"HN_mm_schan_lll_ss_40" , "HN_mm_schan_lll_ss_100", "HN_mm_schan_lll_ss_500", "HN_mm_schan_lll_ss_1500",   "HN_ee_schan_lll_os_40" , "HN_ee_schan_lll_os_100", "HN_ee_schan_lll_os_500", "HN_ee_schan_lll_os_1500", "HN_mm_schan_lll_os_40" , "HN_mm_schan_lll_os_100", "HN_mm_schan_lll_os_500", "HN_mm_schan_lll_os_1500","HN_ee_schan_ll_os_40" , "HN_ee_schan_ll_os_100", "HN_ee_schan_ll_os_500", "HN_ee_schan_ll_os_1500" ,"HN_mm_schan_ll_os_40" , "HN_mm_schan_ll_os_100", "HN_mm_schan_ll_os_500", "HN_mm_schan_ll_os_1500"]


#samples with fullgen entries in the name will store all gen information. All others will store just first 30 gen particles
fullgen = ["QCD_"]

##### *name*
signalsample = ["Major", "SNU", "HN"]
allweights = ["DY"]

if not ALLSamples == True:
    sampledir = mcsampledir

# njob set to 40: if n root files < 40 njobs = #rootfiles
njob=100
njobs_submitted=0
string_of_submitted=""
string_of_failed=""
skip_first=0
dataset_tag=""

if False:
    if RunALLSamples:
        if "jalmond" in username_snu:
            os.system("scp -r /cms/scratch/SNU/datasets_" +version + "/ " + username_snu  + "@147.47.242.42:/data1/LQAnalyzer_rootfiles_for_analysis/DataSetLists/")
            os.system("ssh " + username_snu  + "@147.47.242.42 chmod -R 777  /data1/LQAnalyzer_rootfiles_for_analysis/DataSetLists/datasets_" +version + "/")
            
            
for i in sampledir:
    print i 
    if not RunALLSamples:
        CheckDatasetFile(i,False)
                
    if "DY" in i or "TT" in i:
        njob=200
    else:
        njob=100
    datasetpath = "/cms/scratch/SNU/datasets_" +version + "/dataset_" + i + ".txt"
    
    datasetfile = open(datasetpath, 'r')
    nrootfiles=0
    for line in datasetfile:
        if ".root" in line:
            nrootfiles=nrootfiles+1
        if "DataSetName" in line:
            splitline  = line.split()
            datasetname= splitline[3].replace("/"," ")
            split_datasetname = datasetname.split()
            dataset_tag =split_datasetname[0]
    if nrootfiles ==0:
        continue

    runfullgen = True
    for j in fullgen:
        if j in dataset_tag:
            runfullgen = False

    issignal = False
    for j in signalsample:
         if j in dataset_tag:
             issignal= True

    AllWeights=False
    for k in allweights:
        if k in dataset_tag:
            if "DY" in dataset_tag:
                if "amcatnlo" in dataset_tag:
                    AllWeights= True
                else:
                    AllWeights= False
            AllWeights= True

    output= dataset_tag
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
    
    if sample_exists == 0:
        if PrivateSample == False:
            with open("log.txt" , "a"    ) as f:
                f.write("No sample found for " + str(i))
            continue
        
    os.system("xrd cms-xrdr.sdfarm.kr ls /xrd/store/group/CAT/" + output  + "/" + versionpath + "/" + tagpath + " > " + kisti_output+ "/"+output + "_resub.txt")
    fr_check_resubmit = open(kisti_output+ "/"+output +"_resub.txt",'r')
    
    isresubmitted=True
    nresub=0
    nchecker=0
    while isresubmitted:
        for line_rs in fr_check_resubmit:
            job_x=False
            if ("/000" + str(nchecker)) in line_rs:
                nresub=nresub+1
                nchecker=nchecker+1
                job_x=True
                break
        if not job_x:    
            isresubmitted=False
        
    fr_check_resubmit.close()
    
    if nresub > 1:
        print "nresub = " + str(nresub)
    if os.path.exists( kisti_output+ "/"+output + "_tmpfull.txt"):
        os.system("rm  " + kisti_output+ "/"+output + "_tmpfull.txt")
    for x in range(0,nresub+1):
        os.system("xrd cms-xrdr.sdfarm.kr ls /xrd/store/group/CAT/" + output  + "/" + versionpath + "/" + tagpath + "/000" + str(x) + "/  >> " + kisti_output+ "/"+output + "_tmpfull.txt")

    os.system("sed -r 's/^.{43}//' " +  kisti_output+ "/"+output  + "_tmpfull.txt  > " +kisti_output+ "/"+output  + "_full.txt")

    ## Set the number of jobs and files per job
    file_to_run=kisti_output+ "/"+output +"_full.txt"
    if PrivateSample == True:
        file_to_run=datasetpath

    fr_txt = open(kisti_output+ "/"+output +"_full.txt",'r')
    count_txt=0
    for line_txt in fr_txt:
        if ".root" in line_txt:
            count_txt+=1
    fr_txt.close()


    fr_xrd = open(datasetpath,'r')
    count_xrd=0
    for line_xrd in fr_xrd:
        if ".root" in line_xrd:
            count_xrd+=1
    fr_xrd.close()

    check_file=False
    if check_file:
        if count_xrd != count_txt:
            print str(count_xrd) +" :  "  + str(count_txt)
            print "##############################################"
            print "#rootfiles in " + datasetpath + " and " + kisti_output+ "/"+output +"_full.txt differ" 
            print "##############################################"
            
            fr = open(file_to_run,'r')
            for line in fr:
                if ".root" in line:
                    does_exist=False
                    fr_xrd = open(datasetpath,'r')
                    for line_xrd in fr_xrd:
                        if line_xrd in line:
                            does_exist=True
                    fr_xrd.close()
                    if not does_exist:
                        with open(datasetpath, "a") as myfile:
                            myfile.write("root://cms-xrdr.sdfarm.kr:1094//"+line)
                        with open("log.txt" , "a"    ) as f:
                            f.write(" appening to + " + datasetpath + "--> " + line)
    
            fr.close()   
            continue
        else:
            print "##############################################"
            continue
        
        
    fr = open(file_to_run,'r')
    count=0
    nfilesperjob=0
    for line in fr:
        if ".root" in line:
            count+=1
    fr.close()


    if count == 0:
        print "No files in list"
        quit()
    else:
        print str(count)


    if njob > count:
        njob=count

    for k in range(1,count+1):
        if not k%njob:
            nfilesperjob+=1
            
    if nfilesperjob > 20:
        nfilesperjob = 20
        njob=0
        for l in range(1,count+1):
            if not l%nfilesperjob:
                njob+=1

        files_torun= nfilesperjob*njob
        remainder= count - files_torun
        if remainder != 0:
            njob = njob +1
    else:
        files_torun= nfilesperjob*njob
        remainder= count - files_torun
        extrajobs=0
        while remainder > 0:
            extrajobs=extrajobs+1
            remainder = remainder - nfilesperjob
            
        njob=njob + extrajobs   
    


    print "#job = " + str(njob) + " and nfilesperjob = " + str(nfilesperjob) + " : total number of files = " + str(count)
    print "Number of jobs to process is " + str(njob)
    print "Each job will process  " + str(nfilesperjob) + "/" + str(count) + " files"
    
    jobname = "SNU_" + version + "_" + dataset_tag
    datasetlist= "dataset_" + i + ".txt"
    cfgfile="run_ntupleMaker_snu_mc_cfg.py"

    #### QCD SAMPLES: Slim but only 30 gen kept
    if runfullgen == False:
        cfgfile="run_ntupleMaker_snu_mc_cfg.py"
    #### DY sample: PDF + scale weights stored, slim made
    if AllWeights == True:
        cfgfile="run_ntupleMaker_snu_mc_allweights_cfg.py"   
    ##### Everything stored
    if issignal == True:
         cfgfile="run_ntupleMaker_snu_mc_signal_cfg.py"
    if PrivateSample == True:
        cfgfile="run_ntupleMaker_snu_mc_signal_cfg.py"
    
    print "using " + cfgfile    
    isjobrunning=False
    print "Running : CheckJobStatusAfterCrash"
    isjobrunning = CheckJobStatusAfterCrash(jobname, version)

    if isjobrunning == True:
        
        print "CheckJobStatusAfterCrash = True"
        string_of_submitted= string_of_submitted +  dataset_tag + "!"
        continue

    print "CheckJobStatusAfterCrash = False"
    
    os.system("mkdir  /xrootd/store/user/" + k_user + "/flatcat/")
    
    runcommand="./create-batch_snu   --jobName " + jobname + " --fileList   /cms/scratch/SNU/datasets_" +version + "/" + datasetlist +"  --maxFiles " + str(nfilesperjob) + "  --cfg ../" + cfgfile  + "   --queue batch6  --transferDest /store/user/"+k_user+"/flatcat/"+ jobname 
    print "Running:"
    print  runcommand
    os.system(runcommand)
    
    string_of_submitted= string_of_submitted +  dataset_tag + "!"
    
    njobs_submitted= int(njobs_submitted)+int(njob)
    
    if not os.path.exists(str(cmssw_dir)+"/src/CATTools/CatAnalyzer/SNUsubmission/snu/KistiProductionForSNU/jobcheck"):
        os.system("mkdir " + str(cmssw_dir)+"/src/CATTools/CatAnalyzer/SNUsubmission/snu/KistiProductionForSNU/jobcheck")

    check_njob_submitted=0
    while check_njob_submitted == 0:
        import platform
        
        os.system("condor_q "+k_user+" &>  jobcheck/runningcheck.txt")
        fcheck = open("jobcheck/runningcheck.txt",'r')
        for line in fcheck:
            if "completed" in line:
                if "removed" in line:
                    linesplit = line.split()
                    print linesplit[0]
                    njobs_submitted = int(linesplit[0])
        print "Number of subjobs submitted = " + str(njobs_submitted)
    
        if int(njobs_submitted) > 400:
            print "nsubjobs > 10" 
            print "waiting 1 minute before checking if #subjobs  < 10. If this is true will submit more."
            time.sleep(60.)
            string_of_failed=CheckFailedJobStatus(string_of_submitted, version,string_of_failed)
            string_of_submitted=CheckJobStatus(string_of_submitted, version)

        if int(njobs_submitted) < 400:
            print "Number of jobs < 10. Will check if any jobs are finished"
            check_njob_submitted = 1
            string_of_failed=CheckFailedJobStatus(string_of_submitted, version,string_of_failed)
            string_of_submitted=CheckJobStatus(string_of_submitted, version)
        
    ###    check if jobs are complete: If true send to SNU and delete local dir 

print "###############################################################################################"
print "All samples are submitted. Now checking if jobs are finished......."            
print "###############################################################################################"
FilesAllCopied=False
while FilesAllCopied == False:
    string_of_failed=CheckFailedJobStatus(string_of_submitted, version,string_of_failed)
    tmp_string_of_submitted=string_of_submitted 
    outlogfile.write("string_of_submitted " + string_of_submitted + "\n")
    string_of_submitted=CheckJobStatus(tmp_string_of_submitted, version)
    outlogfile.write("After job check: string_of_submitted " + string_of_submitted+"\n")
  
    if string_of_submitted == "":
        FilesAllCopied = True
    else:
        time.sleep(10.)
outlogfile.close()


breakdown_failed_list =string.replace(string_of_failed,"!", " ")
split_breakdown_failed_list=breakdown_failed_list.split() 
for s in split_breakdown_failed_list:
    print "Job " + s + " Failed"





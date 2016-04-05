#!/usr/bin/python

import os, getpass, sys
import time
from functions import *
from Setup import *
import string


def CheckJobStatusAfterCrash(dirname, v):
    
    print "CheckJobStatusAfterCrash:"
    if not os.path.exists(dirname):
        return
    
    if os.path.exists(dirname + "/check_list_data.txt"):
        os.system("rm " + dirname +"/check_list_data.txt")

    os.system("ls " + dirname + "/* > " + dirname+ "/check_list_data.txt")
    logfile = open( dirname+"/check_list_data.txt", "r")
    njobs_x=0
    for i in logfile:
        if "_cfg.py" in i:
            njobs_x = njobs_x +1
    
    print "number of jobs =  " + str(njobs_x)
    os.system("rm " + dirname+"/check_list_data.txt")

    if os.path.exists(dirname+"/check_finished_data.txt"):
        os.system("rm " + dirname +"/check_finished_data.txt")
    os.system("grep FINISHED " + dirname + "/*.log > " + dirname +"/check_finished_data.txt")
    nfinishedjobs=0
    for j in range(0, njobs_x):
        jobcheck= "job_" + str(j) +".log"
        checkfile = open(dirname+"/check_finished_data.txt", "r")
        for line in checkfile:
            if jobcheck in line:
                nfinishedjobs=nfinishedjobs+1
    print "Number of finished jobs = " + str(nfinishedjobs)
    os.system("rm " + dirname +"/check_finished_data.txt")
    
    if nfinishedjobs != njobs_x:
        "Job is running already. Adding to list"
        return True
    else: 
        print "Removing dir since no jobs are running"
        return True
        #os.system("rm -rf " + dirname)
         
    
        
    return False







def CheckJobStatus(submitted_list, v):

    mod_list =string.replace(submitted_list,"!", " ")
    split_list = mod_list.split()
    print "CheckJobStatus:"
    print "List of jobs submitted = "
    for x in split_list:
        print x


    new_submitted_list=submitted_list  
    print "Checking submitted jobs:"
    for i in split_list:
        period_list =string.replace(i,"_", " ")
        period_split_list = period_list.split()
        
        print period_split_list[0]
        print period_split_list[1]

        if os.path.exists("SNU_" + v+ "_" +i +"/check_list_data.txt"):
            os.system("rm SNU_" + v+ "_" +i +"/check_list_data.txt")

        os.system("ls SNU_" + v + "_" + i +"/* >  SNU_" + v+ "_" +i +"/check_list_data.txt")

        logfile = open( "SNU_" + v+ "_" +i +"/check_list_data.txt", "r")
        njobs_x=0
        for j in logfile:
            if "_cfg.py" in j:
                njobs_x = njobs_x +1
        print "number of jobs =  " + str(njobs_x)
        os.system("rm SNU_" + v+ "_" +i +"/check_list_data.txt")

        if os.path.exists("SNU_" + v+ "_" +i +"/check_finished_data.txt"):
            os.system("rm SNU_" + v+ "_" +i +"/check_finished_data.txt")

        os.system("grep FINISHED " + "SNU_" + v+ "_" +i +"/*.log > SNU_" + v+ "_" +i +"/check_finished_data.txt")
        os.system("grep TERMINATED " + "SNU_" + v+ "_" +i +"/*.log > SNU_" + v+ "_" +i +"/check_finished2_data.txt")
        
        nfinishedjobs=0
        for k in range(0, njobs_x):
            jobcheck= "job_" + str(k) +".log"
            checkfile = open("SNU_" + v+ "_" +i +"/check_finished_data.txt", "r")
            for line in checkfile:
                if jobcheck in line:
                    nfinishedjobs=nfinishedjobs+1

        print "Number of finished jobs = " + str(nfinishedjobs)
        os.system("rm SNU_" + v+ "_" +i +"/check_finished_data.txt")


        FailedJobs=False
        if int(nfinishedjobs) != int(njobs_x):
            for k in range(0, njobs_x):
                jobcheck2= "job_" + str(k) +".log"
                checkfile2 = open("SNU_" + v+ "_" +i +"/check_finished2_data.txt", "r")
                for line2 in checkfile2:
                    if jobcheck2 in line2:
                        FailedJobs=True
                        nfinishedjobs=nfinishedjobs+1

        if FailedJobs == True:
            print "Quiting script cos a job failed"
            quit()

  
        if int(nfinishedjobs) == int(njobs_x):
            print "Job " + i + " is finished. Copying to SNU"

            print "ssh " + username_snu  + "@cms3.snu.ac.kr mkdir /data2/DATA/cattoflat/Data/" + v + "/" + period_split_list[0]
            os.system("ssh " +  username_snu +"@cms3.snu.ac.kr mkdir /data2/DATA/cattoflat/Data/" + str(v))
            os.system("ssh " + username_snu  + "@cms3.snu.ac.kr mkdir /data2/DATA/cattoflat/Data/" + str(v) +"/" + period_split_list[0])
            os.system("ssh " + username_snu  + "@cms3.snu.ac.kr rm -r /data2/DATA/cattoflat/Data/" + str(v) +"/" + period_split_list[0] + "/period" + period_split_list[1] )
            os.system("ssh " + username_snu  + "@cms3.snu.ac.kr mkdir /data2/DATA/cattoflat/Data/" + str(v) +"/" + period_split_list[0] + "/period" + period_split_list[1] )
            

            print "scp SNU_" + v+ "_" +i +"/*.root " + " " + username_snu  + "@cms3.snu.ac.kr:/data2/DATA/cattoflat/Data/"  + str(v) + "/"  +"/" + period_split_list[0] + "/period" + period_split_list[1]

            os.system("scp SNU_" + v+ "_" +i +"/*.root " + " " + username_snu  + "@cms3.snu.ac.kr:/data2/DATA/cattoflat/Data/"  + str(v) + "/"  +"/" + period_split_list[0] + "/period" + period_split_list[1] )
            os.system("ssh " + username_snu  + "@cms3.snu.ac.kr chmod -R 777 /data2/DATA/cattoflat/Data/" + v)

            print "submitted_list = " + submitted_list + " is set to: "
            new_submitted_list = string.replace(submitted_list, i+"!" , "")
            print new_submitted_list
            print "Deleting directory SNU_" + v+ "_" +i +"/"
            os.system("rm -r   SNU_" + v+ "_" +i +"/")
            return new_submitted_list


    return new_submitted_list

######## True runs default list of all samples available
ALLSamples= False
periods = []
if len(datasampledir) == 0:
    ALLSamples=True
    periods = ["C" , "D"]
else:
    periods = data_periods
    

if len(periods) ==0:
    print "Now data period was chosed to process. Fix this."
    quit()
####### make sure this file is being run at kisti                                                                                                                                                                                           
host=os.getenv("HOSTNAME")
if not "ui10" in host:
    quit()

if os.path.exists("cat.txt"):
    os.system("rm cat.txt")
os.system("source "+cmssw_dir+"/src/CATTools/CommonTools/test/snu/catversion.sh > cat.txt")



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

    

os.system("ls /tmp/ > check_snu_connection.txt")
snu_connect = open("check_snu_connection.txt",'r')
connected_cms3=False
for line in snu_connect:
    if "ssh-" +k_user +"@cms3" in line:
        connected_cms3=True
            
os.system("rm check_snu_connection.txt")    
if connected_cms3 == False:    
    print "No connection to cms3: please make connection in screen and run script again"
    quit()



## Make a list of samples to process


sampledir = ["SingleMuon","DoubleMuon", "MuonEG", "SinglePhoton", "DoubleEG", "SingleElectron"]

sampledir = [ "SingleElectron"]

#samples with fullgen entries in the name will store all gen information. All others will store just first 30 gen particles
periods = ["C", "D"]

if not ALLSamples == True:
    sampledir = datasampledir


rereco=False
rereco_tag = "05Oct2015"

# njob set to 40: if n root files < 40 njobs = #rootfiles
njob=30
njobs_submitted=0
string_of_submitted=""
skip_first=0
samples_processed=0
dataset_tag=""
for i in sampledir:
    
    runtrig=False
    if "DoubleEG" in i:
        runtrig=True
        #runtrig true means more triggers are available in catuples              

    if "SingleEl" in i:
        runtrig=True
    
    for period in periods:
        print "period = " + period
        samples_processed=samples_processed+1
        if samples_processed < skip_first+1:
            continue

        njob=30
        output=i
        kisti_output=kisti_output_default+output+"/"
        print "Making dir: " + kisti_output

        if not (os.path.exists(kisti_output)):
            os.system("mkdir " + kisti_output)
        else:
            os.system("rm " + kisti_output + "/*")

        datasetpath = cmssw_dir+"/src/CATTools/CatAnalyzer/data/dataset/dataset_" + i + "_Run2015"+ period + ".txt"                              
    
        datasetfile = open(datasetpath, 'r')
        for line in datasetfile:
            if "DataSetName" in line:
                splitline  = line.split()
                datasetname= splitline[3].replace("/"," ")
                split_datasetname = datasetname.split()
                dataset_tag =split_datasetname[0]

        samples_processed=samples_processed+1 
        if samples_processed < skip_first+1:
            continue


        os.system("xrd cms-xrdr.sdfarm.kr ls /xrd/store/group/CAT/" + output + " > " + kisti_output+ "/"+ output + "_getversion.txt")
        os.system("sed -r 's/^.{43}//' " +  kisti_output+ "/"+output + "_getversion.txt  > " +kisti_output+ "/"+output + "_getversion_skim.tx\
t")

        fr_1end = open(kisti_output+ "/"+output+"_getversion_skim.txt",'r')
        versionpath =""
        iline_version=0
        period_tag = "2015" + period

        for linerp in fr_1end:
            if version in linerp:
                if period_tag in linerp:
                    if rereco:
                        if rereco_tag in linerp:
                            if iline_version < 1:
                                s = linerp.replace("/", " ")
                                splitline  = s.split()
    
                                versionpath = splitline[5]
                            iline_version= iline_version+1
                    else:
                        if not rereco_tag in linerp:
                            if iline_version < 1:
                                s = linerp.replace("/", " ")
                                splitline  = s.split()
                                versionpath = splitline[5]
                                iline_version= iline_version+1
        fr_1end.close()

        os.system("xrd cms-xrdr.sdfarm.kr ls /xrd/store/group/CAT/" + output + "/" + versionpath + " > " + kisti_output+ "/"+ output + ".txt")
        print "xrd cms-xrdr.sdfarm.kr ls /xrd/store/group/CAT/" + output + "/" + versionpath + " > " + kisti_output+ "/"+ output + ".txt"
        os.system("sed -r 's/^.{43}//' " +  kisti_output+ "/"+output + ".txt  > " +kisti_output+ "/"+output + "_skim.txt")
        os.system("cut -d/ -f 8 " + kisti_output+ "/"+output  + "_skim.txt  > " + kisti_output+ "/"+output + "_end.txt")


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
                        if split_tag_date_m > check_m:
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

        print "njob = " + str(njob) + " and nfilesperjob = " + str(nfilesperjob) + " : total number of files = " + str(count)
        
        print "Number of jobs to process is " + str(njob)

        print "Each job will process  " + str(nfilesperjob) + "/" + str(count) + " files"
    

        jobname = "SNU_" + version + "_" + i + "_" + period

        datasetlist= "dataset_" + i + "_Run2015" + period +".txt"
        cfgfile="run_ntupleMaker_snu_data_cfg.py"
        if runtrig == True:
            cfgfile="run_ntupleMaker_snu_data_fulltrig_cfg.py"


        isjobrunning=False
        print "Running : CheckJobStatusAfterCrash"
        isjobrunning = CheckJobStatusAfterCrash(jobname, version)

        if isjobrunning == True:
        
            print "CheckJobStatusAfterCrash = True"
            string_of_submitted= string_of_submitted +  i + "_" + period+ "!"
            continue

        print "CheckJobStatusAfterCrash = False"
        runcommand="create-batch  --jobName " + jobname + " --fileList  ../../../../CatAnalyzer/data/dataset/" + datasetlist +"  --maxFiles " + str(nfilesperjob) + "  --cfg ../" + cfgfile  + "   --queue batch6  --transferDest /xrootd/store/user/" + k_user + "/"
        
        print runcommand
        os.system(runcommand)
    
        string_of_submitted= string_of_submitted +  i + "_" + period+ "!"
    
        njobs_submitted= int(njobs_submitted)+int(njob)
    
        check_njob_submitted=0
        while check_njob_submitted == 0:
            import platform
        
            os.system("condor_q " + k_user + " &>  jobcheck/runningcheck_data.txt")
            fcheck = open("jobcheck/runningcheck_data.txt",'r')
            for line in fcheck:
                if "completed" in line:
                    if "removed" in line:
                        linesplit = line.split()
                        print linesplit[0]
                        njobs_submitted = int(linesplit[0])
            print "Number of subjobs submitted = " + str(njobs_submitted)
    
            if int(njobs_submitted) > 1000:
                print "nsubjobs > 1000" 
                print "waiting 1 minute before checking if #subjobs  < 1000. If this is true will submit more."
                time.sleep(60.)
                string_of_submitted=CheckJobStatus(string_of_submitted, version)

            if int(njobs_submitted) < 1000:
                print "Number of jobs < 1000. Will check if any jobs are finished"
                check_njob_submitted = 1
                string_of_submitted=CheckJobStatus(string_of_submitted, version)
        
    ###    check if jobs are complete: If true send to SNU and delete local dir 

print "All samples are submitted. Now checking if jobs are finished."            
FilesAllCopied=False
while FilesAllCopied == False:
    tmp_string_of_submitted = string_of_submitted
    print "string_of_submitted = " + string_of_submitted
    string_of_submitted=CheckJobStatus(tmp_string_of_submitted, version)
    print "After CheckJobStatus: string_of_submitted = " + string_of_submitted

    if string_of_submitted == "":
        FilesAllCopied = True
    else:
        time.sleep(60.)


        


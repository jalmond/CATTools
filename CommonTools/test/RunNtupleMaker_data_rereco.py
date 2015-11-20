import os, getpass, sys
import time
from functions import *


## SET the production version  to process
version = "v7-4-5"


## Check Branch for SKtrees is up to date to make skims                                                                                                                                                                                                                    
os.system("ssh jalmond@cms3.snu.ac.kr cat /home/jalmond/HeavyNeutrino/13TeV/LQAnalyzer_cat/LQanalyzer/bin/Branch.txt > check_snu_branch.txt")
os.system("ssh jalmond@cms3.snu.ac.kr cat /home/jalmond/HeavyNeutrino/13TeV/LQAnalyzer_cat/LQanalyzer/bin/CATVERSION.txt > check_catversion_branch.txt")
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

os.system("ls /tmp > check_snu_connection.txt")
snu_connect = open("check_snu_connection.txt",'r')
connected_cms3=False
connected_cms4=False
for line in snu_connect:
    if "ssh-jalmond@cms3" in line:
        connected_cms3=True
    else:
        print "No connection to cms3: please make connection in screen and run script again"
        quit()
    if "ssh-jalmond@cms4" in line:
        connected_cms4=True
    else:
        print "No connection to cms3: please make connection in screen and run script again"
        quit()
os.system("rm check_snu_connection.txt")


## Make a list of samples to process                                                                                                                                                                                                                                       


## Make a list of samples to process

sampledir = ["DoubleMuon",
             "DoubleEG" ,
             "MuonEG",
             "SingleMuon",
             "SingleElectron"]


periods = ["C" , "D"]

rereco=True
rereco_tag = "05Oct2015"


if rereco:
    print "Running on rereco samples"

# njob set to 40: if n root files < 40 njobs = #rootfiles
njob=40

skip_first=0
samples_processed=0
for i in sampledir:
    
    
    for period in periods:
        print "period = " + period
        samples_processed=samples_processed+1 
        if samples_processed < skip_first+1:
            continue

        njob=40
        output=i
        print "Making dir: " + output

        if not (os.path.exists(output)):
            os.system("mkdir " + output)
        else:
            os.system("rm " + output + "/*")
    
        os.system("xrd cms-xrdr.sdfarm.kr ls /xrd/store/group/CAT/" + output + " > " + output+ "/"+ output + "_getversion.txt")
        os.system("sed -r 's/^.{43}//' " +  output+ "/"+output + "_getversion.txt  > " +output+ "/"+output + "_getversion_skim.txt")

        fr_1end = open(output+ "/"+output+"_getversion_skim.txt",'r')
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

        os.system("xrd cms-xrdr.sdfarm.kr ls /xrd/store/group/CAT/" + output + "/" + versionpath + " > " + output+ "/"+ output + ".txt")
        print "xrd cms-xrdr.sdfarm.kr ls /xrd/store/group/CAT/" + output + "/" + versionpath + " > " + output+ "/"+ output + ".txt"
        os.system("sed -r 's/^.{43}//' " +  output+ "/"+output + ".txt  > " +output+ "/"+output + "_skim.txt") 
        os.system("cut -d/ -f 8 " + output+ "/"+output  + "_skim.txt  > " + output+ "/"+output + "_end.txt")
    


        ## Get the tag of the production: using the newest tag of version, it is automatic
        fr_end = open(output+ "/"+output+"_end.txt",'r')
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

        os.system("xrd cms-xrdr.sdfarm.kr ls /xrd/store/group/CAT/" + output  + "/" + versionpath + "/" + tagpath + "/0000/ > " + output+ "/"+output + "_tmpfull.txt")
        os.system("sed -r 's/^.{43}//' " +  output+ "/"+output  + "_tmpfull.txt  > " +output+ "/"+output  + "_full.txt")

        ## Set the number of jobs and files per job
        fr = open(output+ "/"+output +"_full.txt",'r')
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
    
            wfr = open(output+ "/"+output + "_" +str(j) + "_full.txt",'w')
            counter=0
            rfr  = open(output+ "/"+output + "_full.txt",'r')
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
            wfr = open(output+ "/"+output + "_" +str(j) + "_full.txt",'a')
            rfr  = open(output+ "/"+output + "_full.txt",'r')
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
            configfile=open(output+ "/" + runscript,'w')
            configfile.write(makeNtupleMakerData(output,output+ "/"+output + "_" + str(j) + "_full.txt", output,j))
            configfile.close()
            
            log = output + "/Job_" + str(j) + ".log"
            runcommand="cmsRun " + output + "/" + runscript + "&>" + log + "&"
            os.system(runcommand)
        
        
        import platform
        check_job_finished=0
        while check_job_finished == 0:        
            os.system("ps ux  | grep 'cmsRun' &> " + output + "/pslog")
            print "ps ux  | grep 'cmsRun' &> " + output + "/pslog"
            filename = output +'/pslog'    
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
                print "ssh jalmond@cms3.snu.ac.kr mkdir /data2/DATA/cattoflat/Data/" + str(i)
                os.system("ssh jalmond@cms3.snu.ac.kr mkdir /data2/DATA/cattoflat/Data/" + str(version))
                os.system("ssh jalmond@cms3.snu.ac.kr mkdir /data2/DATA/cattoflat/Data/" + str(version) + "/" + i )
                if rereco:
                    os.system("ssh jalmond@cms3.snu.ac.kr rm -r /data2/DATA/cattoflat/Data/" + str(version) +"/" + i + "/period" + str(period) +"_rereco/" )
                    os.system("ssh jalmond@cms3.snu.ac.kr mkdir /data2/DATA/cattoflat/Data/" + str(version) + "/" + i + "/period" + str(period) +"_rereco/" )
                else:
                    os.system("ssh jalmond@cms3.snu.ac.kr rm -r /data2/DATA/cattoflat/Data/" + str(version) +"/" + i + "/period" + str(period)  )
                    os.system("ssh jalmond@cms3.snu.ac.kr mkdir /data2/DATA/cattoflat/Data/" + str(version) + "/" + i + "/period" + str(period)  )

                
                print "scp " +output + "/*.root " + " jalmond@cms3.snu.ac.kr:/data2/DATA/cattoflat/Data/" + str(version) + "/"  + str(i) + "/period" + period+"_rereco/"
                if rereco:
                    os.system("scp " +output + "/*.root " + " jalmond@cms3.snu.ac.kr:/data2/DATA/cattoflat/Data/"  + str(version) + "/" +i  +"/period" + period +"_rereco/")
                else:
                    os.system("scp " +output + "/*.root " + " jalmond@cms3.snu.ac.kr:/data2/DATA/cattoflat/Data/"  + str(version) + "/" +i  +"/period" + period +"/")

            os.system("rm " + output + "/pslog")        
            time.sleep(30.) 
        
        
        os.system("rm -r " + output)

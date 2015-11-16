import os, getpass, sys
import time
from functions import *


## SET the production version  to process
version = "v7-4-5"

## Make a list of samples to process

sampledir = ["WZ_TuneCUETP8M1_13TeV-pythia8", 
             "WJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8",
             "TTJets_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
             "ZZ_TuneCUETP8M1_13TeV-pythia8",
             "WW_TuneCUETP8M1_13TeV-pythia8",
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
             "QCD_Pt-600to800_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8", 
             "QCD_Pt-120to170_EMEnriched_TuneCUETP8M1_13TeV_pythia8", 
             "QCD_Pt-1000toInf_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8", 
             "QCD_Pt-170to300_EMEnriched_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt-470to600_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt-80to120_EMEnriched_TuneCUETP8M1_13TeV_pythia8",
             "QCD_Pt-30to50_EMEnriched_TuneCUETP8M1_13TeV_pythia8",
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
             "ST_t-channel_top_4f_leptonDecays_13TeV-powheg-pythia8_TuneCUETP8M1",
             "ST_t-channel_antitop_4f_leptonDecays_13TeV-powheg-pythia8_TuneCUETP8M1",
             "ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1",
             "ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1",
             "TTJets_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8",
             "TT_TuneCUETP8M1_13TeV-powheg-scaleup-pythia8",
             "TT_TuneCUETP8M1_13TeV-powheg-scaledown-pythia8",
             "TT_TuneCUETP8M1_13TeV-powheg-pythi8"]

sampledir = ["WW_TuneCUETP8M1_13TeV-pythia8"]

             



# njob set to 40: if n root files < 40 njobs = #rootfiles
njob=40
    
skip_first=0
samples_processed=0
for i in sampledir:
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
    for linerp in fr_1end:
        if version in linerp:
            if iline_version < 1:
                s = linerp.replace("/", " ")
                splitline  = s.split()
                versionpath = splitline[5]
            iline_version= iline_version+1
    fr_1end.close()

    os.system("xrd cms-xrdr.sdfarm.kr ls /xrd/store/group/CAT/" + output + "/" + versionpath + " > " + output+ "/"+ output + ".txt")

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
        configfile.write(makeNtupleMaker(output,output+ "/"+output + "_" + str(j) + "_full.txt", output,j))
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
            print "ssh jalmond@cms3.snu.ac.kr mkdir /data2/DATA/cattoflat/MC/" + version + "/" + i 
            os.system("ssh jalmond@cms3.snu.ac.kr rm -r /data2/DATA/cattoflat/MC/" + version +"/" + i )
            os.system("ssh jalmond@cms3.snu.ac.kr mkdir /data2/DATA/cattoflat/MC/" + version)
            os.system("ssh jalmond@cms3.snu.ac.kr mkdir /data2/DATA/cattoflat/MC/" + version + "/" + i )

            print "scp " +output + "/*.root " + " jalmond@cms3.snu.ac.kr:/data2/DATA/cattoflat/MC/" + version + "/"  +i
            os.system("scp " +output + "/*.root " + " jalmond@cms3.snu.ac.kr:/data2/DATA/cattoflat/MC/"  + version + "/" +i) 

        os.system("rm " + output + "/pslog")        
        time.sleep(30.) 
        
        
    os.system("rm -r " + output)

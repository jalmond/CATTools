import os, getpass, sys
import time
from functions import *

sampledir = ["TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8", "TTZToQQ_TuneCUETP8M1_13TeV-amcatnlo-pythia8"]
sampledir = ["WZ_TuneCUETP8M1_13TeV-pythia8", "WJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8", "TTJets_TuneCUETP8M1_13TeV-madgraphMLM-pythia8", "ZZ_TuneCUETP8M1_13TeV-pythia8", "WW_TuneCUETP8M1_13TeV-pythia8", "DoubleMuon", "DoubleEG" , "SingleMuon"]

version = "v7-4-1"

# njob set to 30: if n root files < 30 njobs = #rootfiles
njob=30
    
for i in sampledir:
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
    
    fr_end = open(output+ "/"+output+"_end.txt",'r')
    tagpath =""
    iline=0
    for linerp in fr_end:
        if iline < 1:
            tagpath = linerp.strip()
            iline= iline+1
    fr_end.close()

    os.system("xrd cms-xrdr.sdfarm.kr ls /xrd/store/group/CAT/" + output  + "/" + versionpath + "/" + tagpath + "/0000/ > " + output+ "/"+output + "_tmpfull.txt")
    os.system("sed -r 's/^.{43}//' " +  output+ "/"+output  + "_tmpfull.txt  > " +output+ "/"+output  + "_full.txt")


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
                        wfr.write(line)
                        counter_written+=1
                else:
                    if counter > (j-1)*nfilesperjob:
                        if counter <= j*nfilesperjob:
                            wfr.write(line)
                            counter_written+=1
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
            print "ssh jalmond@cms3.snu.ac.kr mkdir /data2/DATA/cattoflat/MC/" + i 
            os.system("ssh jalmond@cms3.snu.ac.kr mkdir /data2/DATA/cattoflat/MC/" + i )
            print "scp " +output + "/*.root " + " jalmond@cms3.snu.ac.kr:/data2/DATA/cattoflat/MC/" +i
            os.system("scp " +output + "/*.root " + " jalmond@cms3.snu.ac.kr:/data2/DATA/cattoflat/MC/" +i) 

        os.system("rm " + output + "/pslog")        
        time.sleep(30.) 
        
        
    os.system("rm -r " + output)

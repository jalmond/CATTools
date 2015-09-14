import os, getpass, sys

from functions import *

samplelist="Samplelist"
sampledir= "DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/v7-4-1_RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v3/"
sampledir = ["WZ_TuneCUETP8M1_13TeV-pythia8"]
sampledir = ["TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8", "TTZToQQ_TuneCUETP8M1_13TeV-amcatnlo-pythia8"]
for i in sampledir:
    output=i

    if not (os.path.exists(output)):
        os.system("mkdir " + output)
    else:
        os.system("rm " + output + "/*")

    os.system("xrd cms-xrdr.sdfarm.kr ls /xrd/store/group/CAT/" + sampledir + " > " + output +  samplelist + ".txt")
    os.system("sed -r 's/^.{43}//' " +  output +  samplelist + ".txt  > " +output +  samplelist + "_skim.txt") 
    os.system("cut -d/ -f 8 " + output +  samplelist + "_skim.txt  > " + output +  samplelist + "_end.txt")

    
    fr_end = open(output+ samplelist +"_end.txt",'r')
    tagpath =""
    iline=0
    for linerp in fr_end:
        if iline < 1:
            tagpath = linerp.strip()
            print "ranpath = " + linerp.strip()
            iline= iline+1
    fr_end.close()

    os.system("xrd cms-xrdr.sdfarm.kr ls /xrd/store/group/CAT/" + sampledir + tagpath + "/0000/ > " + output +  samplelist + "_tmpfull.txt")
    os.system("sed -r 's/^.{43}//' " +  output +  samplelist + "_tmpfull.txt  > " +output +  samplelist + "_full.txt")

    print "Running : xrd cms-xrdr.sdfarm.kr ls /xrd/store/group/CAT/" + sampledir + tagpath + "/0000/ > " + output +  samplelist + "_full.txt"


    fr = open(output+ samplelist +"_full.txt",'r')
    count=0
    nfilesperjob=0
    for line in fr:
        count+=1
    fr.close()


    njob=30
    for i in range(1,count):
        if not i%njob:
            nfilesperjob+=1

    files_torun= nfilesperjob*10
    remainder= count - files_torun

    print "Each job will process  " + str(nfilesperjob) + "/" + str(nfilesperjob+1) + " files"

    counter_written=0
    for i in range(1,njob+1):
    
        wfr = open(output+ samplelist+ str(i) + "_full.txt",'w')
        counter=0
        rfr  = open(output+ samplelist+ "_full.txt",'r')
        for line in rfr:
            counter+=1

            if i == njob:
                if counter > (i-1)*nfilesperjob:
                    wfr.write(line)
                    counter_written+=1
                else:
                    if counter > (i-1)*nfilesperjob:
                        if counter <= i*nfilesperjob:
                            wfr.write(line)
                            counter_written+=1
        wfr.close()
        rfr.close()    

    for i in range(1,njob+1):
        runscript="Z_flatntupleMaker_"+str(i) +".py"
        configfile=open(output+ runscript,'w')
        configfile.write(makeNtupleMaker(sampledir,output+ samplelist+ str(i) + "_full.txt", output,i))
        configfile.close()
    
        log = output + "/Job_" + str(i) + ".log"
        runcommand="cmsRun " + output + runscript + "&>" + log + "&"
        os.system(runcommand)
        
        
    import platform
    check_job_finished=0
    while check_job_finished == 0:        
        os.system("ps ux  | grep 'cmsRun' &> " + output + "/pslog")
        filename = output +'/pslog'    
        n_previous_jobs=0
        for line in open(filename, 'r'):
            n_previous_jobs+=1

        if n_previous_jobs > 0:
            check_job_finished=0
        else:
            check_job_finished=1
            os.system("ssh jalmond@cms3.snu.ac.kr; mkdir /data2/DATA/cattoflat/MC/" + i )
            os.system("scp " +output + "/*.root " + " jalmond@cms3.snu.ac.kr:/data2/DATA/cattoflat/MC/" +i) 

        os.system("rm " + output + "/pslog")
        time.sleep(30.) 
        
        
    #os.system("rm -r " + output)

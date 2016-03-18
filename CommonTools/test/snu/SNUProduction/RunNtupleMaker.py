import os, getpass, sys
import time
from functions import *

## sample directory
sample= "/cms/scratch/jskim/rootfiles/cattuples/"
## output name can be anything, it creates a tmp dir
output="WZ"
outputdir="/tmp/test1/"

if not (os.path.exists(outputdir)):
    os.system("mkdir " + outputdir)

# njob set to 40: if n root files < 40 njobs = #rootfiles
njob=40
    
if not (os.path.exists(output)):
    os.system("mkdir " + output)
else:
    os.system("rm " + output + "/*")
    
os.system("ls " + sample + " > " + output+ "/"+ output + "_full.txt")

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

        os.system("mv " +output + "/*.root " + outputdir)

    os.system("rm " + output + "/pslog")        
    time.sleep(30.) 
        
        
os.system("rm -r " + output)



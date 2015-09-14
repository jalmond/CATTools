import os, getpass, sys

from functions import *

samplelist="Samplelist"
sampledir="/cms/scratch/CAT/DoubleMuon/v7-3-2_Run2015B-PromptReco-v1/150806_165018/0000/"
output="DoubleMuon/"


if not (os.path.exists(output)):
    os.system("mkdir " + output)
else:
    os.system("rm " + output + "/*")

os.system("ls " + sampledir +" > " + output +  samplelist + ".txt")


fr = open(output+ samplelist +".txt",'r')
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
    
    wfr = open(output+ samplelist+ str(i) + ".txt",'w')
    counter=0
    rfr  = open(output+ samplelist+ ".txt",'r')
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
    configfile.write(makeNtupleMaker(sampledir,output+ samplelist+ str(i) + ".txt", output,i))
    configfile.close()
    
    log = output + "/Job_" + str(i) + ".log"
    runcommand="cmsRun " + output + runscript + "&>" + log + "&"
    os.system(runcommand)




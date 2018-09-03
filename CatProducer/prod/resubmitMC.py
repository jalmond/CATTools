import os,sys

onlystatus=False

os.system("bash /cvmfs/cms.cern.ch/crab3/crab.sh ")

os.system("ls -l > log")
logfile = open("log","r")
for l in logfile:
    splitline = l.split()
    for s  in splitline:
        if not "crab" in s:
            continue
        if ".py" in s:
            continue
        if onlystatus:
             os.system("crab status -d " + s)
             continue

        os.system("crab status -d  " +  s +">> jobstatus.log")   

        jobfile = open("jobstatus.log","r")
        
        doresubmit=False
        for line in jobfile:
            if "FAILED" in line:

                doresubmit=True
        jobfile.close()

        os.system("rm jobstatus.log")
        if doresubmit:
            os.system("crab resubmit -d " + s)


        

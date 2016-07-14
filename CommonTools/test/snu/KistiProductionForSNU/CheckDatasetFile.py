import os,sys

from Setup import *

datasetpath="/cms/scratch/SNU/datasets_v7-6-6/"
datasetfilename="dataset_hntest.txt"
datasetpath=datasetpath+datasetfilename

path=open(datasetpath,'r')

xsec_filled=False
validName=True
valid_cv=True
for line in path:
    if "DataSetName" in line:
        splitline= line.split()
        if len(splitline) == 4:
            if"/" in splitline[3]:
                validName=False
    if "xsec" in line:
        splitline= line.split()
        if len(splitline) == 4:
            if float(splitline[3]) > 0.:
                if float(splitline[3]) != 1.:
                    xsec_filled=True
    if "catversion" in line:
        splitline= line.split()
        if len(splitline) == 4:
            if splitline[3] != version:
                valid_cv=False
if not xsec_filled:
    print "Please fill xsec in " + datasetpath + " note the xsec cannot be 1. or 0."
    sys.exit()

if not validName:
    print "Please change DataSetName. This must be one string and contains no characters"
    sys.exit()
if not valid_cv:
    print "Check catversion in " + datasetpath + " this disagrees with version in Setup.py"
    sys.exit()

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
            print "Modify the cms3 connection since  ControlPath in ~/.ssh/cofig is set to something other than tmp or home dir"
            
ch_connect.close()
os.system("rm check_connection.txt")

os.system("ls " + cpath + " > check_snu_connection.txt")
snu_connect = open("check_snu_connection.txt",'r')
connected_cms3=False
for line in snu_connect:
    if "ssh-"+k_user+"@cms3" in line:
        connected_cms3=True
        
if not connected_cms3:
    print "Script needs a connection to cms3 machine. Follow instructions on twiki to make this connection."
    os.system("rm check_snu_connection.txt")
    sys.exit()
else:
    os.system("rm check_snu_connection.txt")    

mailfile=open("sendmail.sh","w")
mailfile.write('mail  -s "new sample '+ version + '"  jalmond@cern.ch < ' + datasetfilename)
mailfile.close()
print datasetpath + " passes checks."
updateinput(datasetpath,datasetfilename, version)
os.system("rm sendmail.sh")

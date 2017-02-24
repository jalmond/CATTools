import os,sys

from Setup import *

def CheckDatasetFile(samplename, SendEmail):
    
    print "Checking file: " + samplename
    datasetpath="/cms/scratch/SNU/datasets_" +version + "/"
    datasetfilename="dataset_" + samplename + ".txt"
    datasetpath=datasetpath+datasetfilename

    path=open(datasetpath,'r')

    xsec_filled=False
    validName=False
    validAliasName=False
    valid_cv=True
    for line in path:
        if "DataSetName" in line:
            validName=True
            splitline= line.split()
            if len(splitline) == 4:
                if"/" in splitline[3]:
                    validName=False
        if "name " in line:
            splitline= line.split()
            validAliasName=True
            if len(splitline) == 4:
                if"/" in splitline[3]:
                    validAliasName=False
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
        #sys.exit()

    if not validName:
        print "Please change DataSetName. This must be one string and contains no characters"
        sys.exit()
    if not validAliasName:
        print "Please change Name. This must be one string and contains no characters. This will be used in catanalyzer at SNU."
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
                print "Modify the cms21 connection since  ControlPath in ~/.ssh/cofig is set to something other than tmp or home dir"
            
    ch_connect.close()
    os.system("rm check_connection.txt")

    os.system("ls " + cpath + " > check_snu_connection.txt")
    snu_connect = open("check_snu_connection.txt",'r')
    connected_cms3=False
    for line in snu_connect:
        if "ssh-"+k_user+"@147.47.242.42" in line:
            connected_cms3=True
        
    if not connected_cms3:
        print "Script needs a connection to cms21 (147.47.242.42) machine. Follow instructions on twiki to make this connection."
        os.system("rm check_snu_connection.txt")
        sys.exit()
    else:
        os.system("rm check_snu_connection.txt")    


    os.system("scp -r " + datasetpath + " " + k_user+"@147.47.242.42://data1/LQAnalyzer_rootfiles_for_analysis/DataSetLists/datasets_"+version+"/")
    os.system("ssh " + username_snu  + "@147.47.242.42 chmod -R 777  /data1/LQAnalyzer_rootfiles_for_analysis/DataSetLists/datasets_" +version + "/"+datasetfilename)

    if SendEmail:
        mailfile=open("sendmail.sh","w")
        mailfile.write('mail  -s "new sample '+ version + '"  jalmond@cern.ch < ' + datasetfilename)
        mailfile.close()
        print datasetpath + " passes checks."
        updateinput(datasetpath,datasetfilename, version)
        os.system("rm sendmail.sh")
        

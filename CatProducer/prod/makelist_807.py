import os,sys


def GetListOfDataSets(catversion, sample_type):

    dlist=[]

    if sample_type == "mc":
        url = 'https://docs.google.com/spreadsheets/d/e/2PACX-1vTWct60gAWZoN7tmVAw_WJmQmDWdrpXluF1xz8cn4sL8NCll3Vb8ppN2254P10zmnZ_oqF1f8XSHuav/pub?gid=238920808&single=true&output=csv'
    if sample_type == "signal":
        url = 'https://docs.google.com/spreadsheets/d/e/2PACX-1vTWct60gAWZoN7tmVAw_WJmQmDWdrpXluF1xz8cn4sL8NCll3Vb8ppN2254P10zmnZ_oqF1f8XSHuav/pub?gid=602011091&single=true&output=csv'

    import csv

    from urllib import urlopen

    cr = csv.reader(urlopen(url).readlines())
    for row in cr:
        if row[0] == "END":
            return dlist
        if not row[0] == "User":
            if len(row) > 3:
                len_alias = len(row[1])
                len_dn = len(row[2])
                len_xsec = len(row[3])
                len_1 = 30 - len_alias
                len_2 = 180 - len_dn
                len_3 = 10 - len_xsec
                print "[Alias,dataset,xsec]: " + row[1]+ " "*len_1+ row[2] +" "*len_2 +row[3]

                if row[0] == "":
                    dlist.append([row[2],row[1],row[3],"jalmond"])
                else:
                    dlist.append([row[2],row[1],row[3],row[0]])



catversion=str(os.getenv("CATVERSION"))
datasetlist=GetListOfDataSets(catversion,"mc")
datasetlist_signal=GetListOfDataSets(catversion,"signal")


datasetname =  datasetlist + datasetlist_signal

catversion="v8-0-8"


os.system("bash /cvmfs/cms.cern.ch/crab3/crab.sh ")


version_production = "Catuple/"
version_production = ""

for m in range(0, len(datasetname)):

    print datasetname[m]
    dataset = datasetname[m][0]

    tmpdataset = dataset.replace("/"," ")
    tmpdataset=tmpdataset.split()
    
    pdataset = tmpdataset[0]
    ixsec = datasetname[m][2]
    name = datasetname[m][1]
    user = datasetname[m][3]
    tag=""

    version=""
    print "pdataset = "+pdataset
    print "name = " + name
    print "xsec = " + str(ixsec)

    os.system("ls -lth  /xrootd/store/user/"+user+"/"+pdataset + " > log0.txt" )

    readlog0 = open("log0.txt","r")
    for line in readlog0:
        if "root" in line or "jalmond" in line:
            sline = line.split()
            if len(sline) == 9:
                version=sline[8]
    readlog0.close()


    os.system("ls -lth /xrootd/store/user/"+user+"/"+version_production+pdataset + "/"+version+"/> log1.txt" )


    readlog1 = open("log1.txt","r")
    for line in readlog1:
        if "root" in line or "jalmond" in line:
            sline = line.split()
            if len(sline) == 9:
                tag=sline[8]
    readlog1.close()
    
        
    os.system("ls -lth /xrootd/store/user/"+user+"/"+version_production+pdataset + "/"+version+""+"/"+tag + "/ > log1b.txt")
    
    readlog1b = open("log1b.txt","r")
    nlines=0
    for line in readlog1b:
        sline = line.split()
        if len(sline) == 9:
            nlines=nlines+1
    readlog1b.close()
               

    nlines_wrote=0    
    while nlines > nlines_wrote:
        os.system("ls -lth /xrootd/store/user/"+user+"/"+version_production+pdataset + "/"+version+"/"+tag + "/000"+str(nlines_wrote)+"/ > log2"+str(nlines_wrote)+".txt")
        
        readlog2_list = []
        readlog2 = open("log2"+str(nlines_wrote)+".txt","r")
        for line in readlog2:
            readlog2_list.append(line)
        readlog2.close()

        if nlines_wrote==0:
            listfile = open("/cms/scratch/SNU/datasets_"+catversion+"/dataset_"+ name + ".txt","w")
            listfile.write("# DataSetName = " + dataset + "\n") 
            listfile.write("# xsec = "+str(ixsec)+"\n")
            listfile.write("# catversion = " + catversion + "\n")
            listfile.write("# name = " + name +  "\n")   
            listfile.write("# sample_user = " +user +  "\n")   
            for line in readlog2_list:
                if "root" in line or user in line:
                    sline =line.split()
                    if len(sline) == 9:
                        filename=sline[8]
                        listfile.write("root://cms-xrdr.sdfarm.kr:1094///xrd/store/user/"+user+"/"+version_production+pdataset + "/"+version+"/"+tag + "/000"+str(nlines_wrote)+"/"+filename+"\n")
            listfile.close()
        else:
            listfile = open("/cms/scratch/SNU/datasets_"+catversion+"/dataset_"+ name + ".txt","a")
            for line in readlog2_list:
                if "root" in line or user in line:
                    sline =line.split()
                    if len(sline) == 9:
                        filename=sline[8]
                        
                        listfile.write("root://cms-xrdr.sdfarm.kr:1094///xrd/store/user/"+user+"/"+version_production+pdataset + "/"+version+"/"+tag + "/000"+str(nlines_wrote)+"/"+filename+"\n") 
            listfile.close()

        nlines_wrote=nlines_wrote+1

        readlog2.close()

    os.system("rm log1.txt")
    os.system("rm log2*.txt")


print "["
for m in range(0, len(datasetname)):
    print '"'+datasetname[m][1] + '",'
print "]" 

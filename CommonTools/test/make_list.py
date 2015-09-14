import os, getpass, sys

from functions import *

samplelist="Samplelist.txt"
sampledir="/cms/scratch/CAT/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/v7-3-2_RunIISpring15DR74-Asympt50ns_MCRUN2_74_V9A-v2/150805_203735/0000/"

os.system("ls " + sampledir +" > " + samplelist)

fr = open(samplelist,'r')
count=0
for line in fr:
    count+=1
fr.close()

runscript="Sample_list.txt"


configfile=open(runscript,'w')

configfile.write(makeConfigFile(sampledir,samplelist, count))
configfile.close()


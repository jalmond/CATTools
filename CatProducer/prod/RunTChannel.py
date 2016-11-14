import os,sys

catversion="v8-0-2"
hn_mass=["100","200","500","1100"]
xsec=[1.501,0.08228,0.003444,0.00009347]
lep_channel=["emem","epep","mummum","mupmup"]
hn_channel = ["Tchannel"]
mv_list="mvfile.sh"
write_list = open(mv_list,"w")
for ch in hn_channel:
    ilepch=-1
    for lch in lep_channel:
        ilepch=ilepch+1
        for mass in hn_mass:
            os.system("./create-batch_jk  --jobName " + ch+"_"+lch+"_"+mass+"  --fileList  inputfiles/dataset_13TeV_HN_" + ch+"_"+lch+"_M-"+mass+".txt  --maxFiles 1  --cfg PAT2CAT_cfg.py  --queue batch6  --transferDest /xrootd/store/user/jalmond/Catuples//HN/"+catversion+"/" + ch+"_"+lch+"_"+mass+"/" )
            os.system("mkdir /xrootd/store/user/jalmond/Catuples/HN/"+catversion+"/" + ch +"_"+lch+"_" + mass)
            write_list.write("mv " + ch+"_"+lch+"_"+mass+"/*.root /xrootd/store/user/jalmond/Catuples/HN/"+catversion+"/" + ch +"_"+lch+"_" + mass + "\n")
write_list.close()

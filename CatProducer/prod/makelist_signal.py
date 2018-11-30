
import os

flavours = ["MuMu", "EE"]
zmasses = ["400", "600", "800", "1000", "1200", "1400", "1600", "1800", "2000", "2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000", "4200", "4400", "4600", "4800", "5000"]
nmasses  = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000", "1100", "1200", "1300", "1400", "1500", "1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400"]

for flavour in flavours:
 if flavour == "EE": user = "jhchoi"
 elif flavour == "MuMu": user = "helee"
 for zmass in zmasses:
  for nmass in nmasses:
   if (((float)(zmass)/2. - 1) < ((float)(nmass))):
     continue
   channel = "ZprimetoNN_"+flavour+"JJJJ_Zprime"+zmass+"_N"+nmass+"_WR5000_NLO"
   print "/"+channel+"/"+user+"-CMSSW_8_0_21_MiniAOD-28028af67189b3de7224b79195bd0e1d/USER"


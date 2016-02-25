This directory is for making Flat Cattuples on kisti machine:

Option 1 (FOR MAKING SAMPLES FOR SNU):
- Run full set of Data/MC or Data+MC jobs ( files are transferred to snu cluster when jobs are done). 
- ONLY flat ntupples are made, no sktrees are made

MC:
python RunNtupleMaker.py
Data:
python RunNtupleMaker_data_rereco.py (not needed after v761)
python RunNtupleMaker_data.py

note:
1)set these variables in the python files
- snu_lqpath
- username_snu

2)Set up connection to cms3,4 in screen (if not the process will exit and warn you to do this)



Option 2 (FOR MAKING SAMPLES FOR SNU):
- Run full set of Data/MC or Data+MC jobs ( files are transferred to snu cluster when jobs are done).	
- BOTH flat ntuples and sktrees are made
- Effective Lumi file is automatically produced for this production

To inlcude sktree production + skims for US/Ferdinando set FullRun=True in python files

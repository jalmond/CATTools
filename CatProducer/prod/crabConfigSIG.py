from WMCore.Configuration import Configuration
from CRABClient.UserUtilities import  getUsernameFromSiteDB

config = Configuration()

config.section_("General")
config.General.transferLogs    = False
config.General.transferOutputs = True

config.section_("JobType")
config.JobType.pluginName  = 'Analysis'
config.JobType.psetName    = 'PAT2CAT_cfg.py'

config.section_("Data")
config.Data.publication  = False
config.Data.splitting = 'Automatic'

#################################################################
# ALLOWS NON VALID DATASETS
config.Data.allowNonValidInputDataset = True

config.section_("Site")
# Where the output files will be transmitted to
config.Site.storageSite = 'T3_KR_KISTI'
#config.Data.inputDBS = 'phys03'
config.Data.outLFNDirBase = '/store/user/%s/' % (getUsernameFromSiteDB())

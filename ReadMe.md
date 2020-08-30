Please ensure that all the tensorflow model files ('.pb') are in the plugins folder of E2eDLrec folder.
Please run the following commands in AFS in the src directory of CMSSW_x_x_x (preferably CMSSW_10_6_8):
''' git clone https://github.com/Shra1-25/E2eDL.git
    scram b
    cmsRun E2eDL/E2eDLrec/python/E2eDLrec_cfg.py \
    inputFiles=file:[filename.root]'''
    

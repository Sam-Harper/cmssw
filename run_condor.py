import argparse
import numpy
import os


cwd = os.getcwd()


# Argument parser
parser = argparse.ArgumentParser(formatter_class = argparse.RawTextHelpFormatter)

# TreeMaker_SingleElectron_PT2to100_PhaseIITDRSpring19DR-PU200_106X_upgrade2023_realistic_v3-v1_GEN-SIM-DIGI-RAW
parser.add_argument(
    "--processName",
    help = "Name of the process to be run",
    type = str,
    required = True,
)

# sourceFiles/SingleElectron_PT2to100_PhaseIITDRSpring19DR-PU200_106X_upgrade2023_realistic_v3-v1_GEN-SIM-DIGI-RAW/SingleElectron_PT2to100_PhaseIITDRSpring19DR-PU200_106X_upgrade2023_realistic_v3-v1_GEN-SIM-DIGI-RAW.txt
parser.add_argument(
    "--inputFileList",
    help = "File containing the list of input files",
    type = str,
    required = True,
)

# EDAnalyzers/TreeMaker/python/ConfFile_cfg.py
parser.add_argument(
    "--cmsRunFile",
    help = "cmsRun file",
    type = str,
    required = True,
)

# /eos/cms/store/group/phys_egamma/sobhatta/HGCal_TreeMaker
parser.add_argument(
    "--outputDir",
    help = "cmsRun output directory",
    type = str,
    required = True,
)

parser.add_argument(
    "--suffix",
    help = "suffix",
    type = str,
    required = False,
    default = "",
)

parser.add_argument(
    "--nUnitPerJob",
    help = "Numbers of units to process per job",
    type = int,
    required = False,
    default = 5,
)

parser.add_argument(
    "--nInputFileMax",
    help = "Maximum number of units to process",
    type = int,
    required = False,
    default = -1,
)


# Parse arguments
args = parser.parse_args()


condorConfig = "condor_config.sub"
condorScript = "condor_script.sh"

condorConfig_name = condorConfig[0: condorConfig.rfind(".")]
condorConfig_ext = condorConfig[condorConfig.rfind("."):]

condorScript_name = condorScript[0: condorScript.rfind(".")]
condorScript_ext = condorScript[condorScript.rfind("."):]


if (__name__ == "__main__") :
    
    nJob_total = 0
    nUnit_total = 0
    
    
    processName = "%s%s" %(args.processName, args.suffix)
    
    
    outputDir = "%s/%s" %(args.outputDir, processName)
    command = "mkdir -p " + outputDir
    print "Command:", command
    os.system(command)
    print ""
    
    #condorDir = "%s/condorJobs" %(outputDir)
    condorDir = "condorJobs/%s" %(processName)
    command = "mkdir -p " + condorDir
    print "Command:", command
    os.system(command)
    print ""
    
    
    inputFiles = numpy.loadtxt(args.inputFileList, dtype = str)
    nInputFile = inputFiles.shape[0]
    
    if (args.nInputFileMax > 0 and nInputFile > args.nInputFileMax) :
        
        inputFiles = inputFiles[0: args.nInputFileMax]
        nInputFile = inputFiles.shape[0]
    
    
    nJob = int(numpy.ceil(float(nInputFile)/args.nUnitPerJob))
    
    print "****************************************************************************************************"
    print "****************************************************************************************************"
    print "Process:", args.processName
    print "Input file:", args.inputFileList
    print "cmsRun file:", args.cmsRunFile
    print "Output directory:", outputDir
    print "Condor directory:", condorDir
    print "# units:", nInputFile
    print "# jobs:", nJob
    print "# units per job:", args.nUnitPerJob
    print "****************************************************************************************************"
    print "****************************************************************************************************"
    print ""
    
    
    inputFileList = args.inputFileList
    inputFileList = inputFileList[inputFileList.rfind("/")+1:]
    inputFileList_name = inputFileList[0: inputFileList.rfind(".")]
    inputFileList_ext = inputFileList[inputFileList.rfind("."):]
    
    
    condorConfig_content = ""
    
    with open(condorConfig, "r") as f :
        
        condorConfig_content = f.read()
    
    
    condorScript_content = ""
    
    with open(condorScript, "r") as f :
        
        condorScript_content = f.read()
    
    
    nDigit = len(str(nJob))
    
    for iJob in range(0, nJob) :
        
        jobNumberStr = "%0*d" %(nDigit, iJob+1)
        
        if (iJob < nJob-1) :
            
            inputFiles_mod = inputFiles[iJob*args.nUnitPerJob: (iJob+1)*args.nUnitPerJob]
            
        else :
            
            inputFiles_mod = inputFiles[iJob*args.nUnitPerJob:]
        
        condorConfig_mod = condorConfig_name + "_" + jobNumberStr + condorConfig_ext
        condorConfig_mod = "%s/%s" %(condorDir, condorConfig_mod)
        
        condorScript_mod = condorScript_name + "_" + jobNumberStr + condorScript_ext
        condorScript_mod = "%s/%s" %(condorDir, condorScript_mod)
        
        inputFileList_mod = inputFileList_name + "_" + jobNumberStr + inputFileList_ext
        inputFileList_mod = "%s/%s" %(condorDir, inputFileList_mod)
        
        # Input file list
        print "Writing: %s" %(inputFileList_mod)
        
        with open(inputFileList_mod, "w") as f :
            
            f.write("\n".join(inputFiles_mod) + "\n")
        
        
        # Condor config
        condorConfig_content_mod = condorConfig_content
        condorConfig_content_mod = condorConfig_content_mod.replace("@exe@", condorScript_mod)
        condorConfig_content_mod = condorConfig_content_mod.replace("@log@", condorDir + "/" + "job_%s" %(jobNumberStr) + ".log")
        condorConfig_content_mod = condorConfig_content_mod.replace("@out@", condorDir + "/" + "job_%s" %(jobNumberStr) + ".out")
        condorConfig_content_mod = condorConfig_content_mod.replace("@err@", condorDir + "/" + "job_%s" %(jobNumberStr) + ".err")
        
        print "Writing: %s" %(condorConfig_mod)
        
        with open(condorConfig_mod, "w") as f :
            
            f.write(condorConfig_content_mod)
        
        
        # Condor script
        lineBreakStr = " \\\n"
        
        cmsRun_cmd = "cmsRun %s" %(args.cmsRunFile) + lineBreakStr
        cmsRun_cmd += "\t print" + lineBreakStr
        cmsRun_cmd += "\t sourceFile=%s" %(inputFileList_mod) + lineBreakStr
        cmsRun_cmd += "\t outputDir=%s" %(outputDir) + lineBreakStr
        cmsRun_cmd += "\t outFileNumber=%d" %(iJob+1) + lineBreakStr
        cmsRun_cmd += "\t onRaw=1" + lineBreakStr
        cmsRun_cmd += "\t storeRecHit=0" + lineBreakStr
        cmsRun_cmd += "\t storeSimHit=0" + lineBreakStr
        cmsRun_cmd += "\t debugFile=0" + lineBreakStr
        
        #cmsRun_cmd += "\t maxEvents=20" + lineBreakStr
        
        condorScript_content_mod = condorScript_content
        condorScript_content_mod = condorScript_content_mod.replace("@dir@", cwd)
        condorScript_content_mod = condorScript_content_mod.replace("@cmd@", cmsRun_cmd)
        
        print "Writing: %s" %(condorScript_mod)
        
        with open(condorScript_mod, "w") as f :
            
            f.write(condorScript_content_mod)
        
        command = "chmod +x %s" %(condorScript_mod)
        print "Command:", command
        os.system(command)
        
        
        # Submit job
        command = "condor_submit %s" %(condorConfig_mod)
        print "Command:", command
        
        commandReturn = 1
        
        # Repeat until job is submission is successful (returns 0)
        while (commandReturn) :
            
            commandReturn = os.system(command)
        
        
        print "\n"
        
    
    
    print "\n"
    print "****************************************************************************************************"
    print "****************************************************************************************************"
    print "Total # unit:", nInputFile
    print "Total # job:", nJob
    print "****************************************************************************************************"
    print "****************************************************************************************************"
    print "\n"

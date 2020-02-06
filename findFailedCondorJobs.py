import argparse
import glob
import os


# Argument parser
parser = argparse.ArgumentParser()

# List of directories
parser.add_argument(
    "-d",
    "--directories",
    help = "List of directories",
    nargs = "*",
    type = str,
    required = True
)

parser.add_argument(
    "-s",
    "--submit",
    help = "Sumbit the failed jobs",
    action = "store_true"
)

# Parse arguments
args = parser.parse_args()


l_directory = args.directories

print "You have entered:"
print "\n".join(l_directory)
print ""

if (args.submit) :
    
    print "Will SUBMIT the failed jobs."

else :
    
    print "Will NOT submit the failed jobs."

print ""
print "Enter YES to proceed, or anything else to cancel."
choice = raw_input("Input: ")

if (choice != "YES") :
    
    print "Exiting."
    exit()


extension = ".err"

# Case insentitive
#l_errorStr = ["error", "segmentation", "killed", "SCRAM fatal"]
l_errorStr = ["Fatal Exception"]
l_errorStrSkip = ["unknown branch"]

condorConfig_nameTemplate = "condor_config_%s.sub"
command_template = "condor_submit %s"


nJobTotal = 0


for iDir in range(0, len(l_directory)) :
    
    directory = l_directory[iDir]
    
    print "\n"
    print "**************************************************"
    print "Directory:", directory
    print "**************************************************"
    
    if (not os.path.isdir(directory)) :
        
        print "Directory does not exist."
        exit(1)
    
    l_fileName = glob.glob(directory + "/**")
    l_fileName = [f for f in l_fileName if extension in f]
    l_fileName.sort(key = lambda l: int(l[l.rfind("_")+1: l.rfind(extension)]))
    
    count = 0
    
    for iFile in range(0, len(l_fileName)) :
        
        fileName = l_fileName[iFile]
        
        fileContent = []
        
        with open(fileName) as f :
            
            fileContent = f.readlines()
        
        # Case insensitive search
        #if (not any([errorStr.lower() in line.lower() for line in fileContent])) :
        #if (not any([(errorStr.lower() in line.lower() and errorStrSkip.lower() not in line.lower()) for line in fileContent])) :
        #    
        #    continue
        
        hasError = False
        
        for errorStr in l_errorStr :
            
            for errorStrSkip in l_errorStrSkip :
                
                if (any([(errorStr.lower() in line.lower() and errorStrSkip.lower() not in line.lower()) for line in fileContent])) :
                    
                    hasError = True
                    
                    break
            
            if (hasError) :
                
                break
        
        if (not hasError) :
            
            continue
        
        count += 1
        nJobTotal += 1
        
        number = fileName[fileName.rfind("_")+1: fileName.rfind(extension)]
        
        condorConfig_name = condorConfig_nameTemplate %(number)
        condorConfig_name = "%s/%s" %(directory, condorConfig_name)
        command = command_template %(condorConfig_name)
        
        print "Count:", count
        print "Total count:", nJobTotal
        print "File:", fileName
        print "Condor config:", condorConfig_name
        
        if (args.submit) :
            
            print "Command:", command
            os.system(command)
        
        print ""

print "\n"
print "Total number of failed jobs:", nJobTotal
print "\n"

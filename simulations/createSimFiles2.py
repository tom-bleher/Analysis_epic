import os
import sys
import re
import multiprocessing
import logging

# Setup logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

#fileType = "idealElectrons"
fileType = "beamEffectsElectrons"

# directories genEvents and simEvents needs to exist
inPath = ""
outPath = ""
if len(sys.argv) > 1: 
  inPath = "/" + sys.argv[1]
if len(sys.argv) > 2: 
  outPath = "/" + sys.argv[2]

if not outPath:
  outPath = inPath

genPath = "genEvents{0}".format(inPath)
simPath = "simEvents{0}".format(outPath)
epicPath = "/home/dhevan/eic/epic/epic_ip6_extended.xml"

# Validate paths
if not os.path.exists(simPath):
    logging.error("Output directory doesn't exist. Create a directory called " + simPath)
    exit(1)

if len(os.listdir(simPath)) != 0:
    logging.error("{0} directory not empty. Clear the directory.".format(simPath))
    exit(1)

det_dir = os.environ.get('DETECTOR_PATH')
if not det_dir:
    logging.error("Environment variable 'DETECTOR_PATH' is not set.")
    exit(1)

compact_dir = os.path.join(det_dir, 'compact')
cmd = 'cp -r {0} {1}'.format(compact_dir, simPath)

# Copy over epic compact dir for parameter reference 
os.system('cp -r {0} {1}'.format(compact_dir, simPath))

def runSims(cmd):
    try:
        os.system(cmd)
    except Exception as e:
        logging.error(f"Error executing command: {cmd}\nException: {e}")

commands = []

# Create command strings
for file in sorted(os.listdir(genPath)):
    if fileType not in file:
        continue
    inFile = os.path.join(genPath, file)
    fileNum = re.search(r"\d+\.+\d\.", inFile).group()
    cmd = "ddsim --inputFiles {0} --outputFile {1}/output_{2}edm4hep.root --compactFile {3} -N 5000".format(inFile, simPath, fileNum, epicPath)
    logging.info(f"Command: {cmd}")
    commands.append(cmd)

# Determine the number of available CPU cores
num_cores = multiprocessing.cpu_count()
logging.info(f"Using {num_cores} CPU cores")

# Start Pool of processes
with multiprocessing.Pool(num_cores) as pool:
    try:
        # Run processes (synchronous, it is a blocking command)
        pool.map(runSims, commands)
    except Exception as e:
        logging.error(f"Error in multiprocessing pool: {e}")
    finally:
        # Ensure the pool is properly closed
        pool.close()
        pool.join()

logging.info("Simulation processing complete.")

import os
import sys
import re
import multiprocessing
import shutil
from datetime import datetime

# Get adresses 
# Prompt for pixel values 

# Create backup system with date notation

# Copy xml file 
# Make changed to pixel size 
# Store adresses temporarly locally 

# loop over adresses and create simulation files

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

# path containing the underlaying xml file with the pixel size definition
pixelPath = r""
genPath = r"genEvents{0}".format(inPath)
simPath = r"simEvents{0}".format(outPath)
SimBackUpPath = f"{simPath}\{datetime.now().strftime("%d%m%Y_%H%M%S")}" 
epicPath = r"/data/tomble/eic/epic/install/share/epic/epic_ip6_extended.xml"

if not os.path.exists(simPath):
    print("Out dir doesn't exist.  Create a dir called " + simPath)
    exit()

# If we have files from previous run, create a backup
if len(os.listdir(simPath)) != 0:
  for idx, file in enumerate(os.listdir(simPath)): 
    if os.path.isfile(source):
      shutil.move(simPath, SimBackUpPath)
  print("Created new back up simulation files in {0}".format(SimBackUpPath))

det_dir = os.environ['DETECTOR_PATH']
compact_dir = det_dir + '/compact'
cmd = 'cp -r {0} {1}'.format(compact_dir, simPath)

# cp over epic compact dir for parameter reference 
os.system('cp -r {0} {1}'.format(compact_dir, simPath) )

def runSims(x):
  os.system(x)

commands = []

# get desired pixel values 
pixel_input = list(input("Enter the desired pixel values seperated by commas. You may press enter to only run for default value "))

# transform to a list containing floats 
pixel_values = [float(value) for value in pixel_input if "," not in value]

default_pixel_value = 

if default_pixel_value not in pixel_values:
  pixel_values.append(default_pixel_value)

# loop over pixel values and create respective xml file copies
for pixel_value in pixel_values:
  shutil.copyfile(f"{pixelPath}/___.xml", f"{pixelPath}/___{pixel_value}.xml")

xml_files = [filename for filename in os.listdir(pixelPath) if filename.startswith("prefix")]

# create command strings
for file in sorted(os.listdir(genPath),):
#for it in range(1,50):
  if fileType not in file:
    continue
  inFile = genPath + "/" + file
  fileNum = re.search("\d+\.+\d\.", inFile).group()
  #fileNum = re.search("\d+\.", inFile).group()
  cmd = "ddsim --inputFiles {0} --outputFile {1}/output_{2}edm4hep.root --compactFile {3} -N 10000".format(inFile, simPath, fileNum, epicPath)
  print( cmd )
  commands.append( cmd )


# start Pool of processes
pool = multiprocessing.Pool(8) # 8 processes to start

# run processes (synchronous, it is a blocking command)
pool.map( runSims, commands )

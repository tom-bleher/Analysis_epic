import os
import sys
import re
import argparse
import multiprocessing
import subprocess

def runSims(command):
    try:
        subprocess.run(command, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Command failed with error: {e}")

def main():
    parser = argparse.ArgumentParser(description="Run simulations in parallel")
    parser.add_argument("inPath", help="Input directory path")
    parser.add_argument("outPath", nargs="?", default="", help="Output directory path")
    
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
    epicPath = "/data/tomble/eic/epic/install/share/epic/epic_ip6_extended.xml"
    
    if not os.path.exists(simPath):
        print("Out dir doesn't exist.  Create a dir called " + simPath)
        exit()
    
    if len(os.listdir(simPath)) != 0:
      print("{0} directory not empty.  Clear directory".format(simPath))
      exit()
    
    det_dir = os.environ['DETECTOR_PATH']
    compact_dir = det_dir + '/compact'
    cmd = 'cp -r {0} {1}'.format(compact_dir, simPath)
    
    # cp over epic compact dir for parameter reference 
    os.system('cp -r {0} {1}'.format(compact_dir, simPath) )
    
    # Copy epic compact dir for parameter reference 
    subprocess.run(cmd, shell=True, check=True)

    commands = []

    # Create command strings
    for file in sorted(os.listdir(genPath)):
        if fileType not in file:
            continue
        inFile = os.path.join(genPath, file)
        fileNum = re.search(r"\d+\.\d+\.", inFile).group()
        cmd = f"ddsim --inputFiles {inFile} --outputFile {simPath}/output_{fileNum}edm4hep.root --compactFile {epicPath} -N 5000"
        print(cmd)
        commands.append(cmd)

    # Start Pool of processes
    pool = multiprocessing.Pool(8)  # 8 processes to start

    # Run processes (synchronous, it is a blocking command)
    pool.map(runSims, commands)

if __name__ == "__main__":
    main()

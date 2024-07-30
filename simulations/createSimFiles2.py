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

    args = parser.parse_args()
    inPath = f"/{args.inPath}"
    outPath = f"/{args.outPath}" if args.outPath else inPath

    genPath = f"genEvents{inPath}"
    simPath = f"simEvents{outPath}"
    epicPath = "/home/dhevan/eic/epic/epic_ip6_extended.xml"

    if not os.path.exists(simPath):
        print(f"Out dir doesn't exist. Creating directory {simPath}")
        os.makedirs(simPath)

    if len(os.listdir(simPath)) != 0:
        print(f"{simPath} directory not empty. Clear the directory.")
        exit()

    det_dir = os.getenv('DETECTOR_PATH', '')
    if not det_dir:
        print("DETECTOR_PATH environment variable not set.")
        exit()

    compact_dir = os.path.join(det_dir, 'compact')
    cmd = f'cp -r {compact_dir} {simPath}'

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

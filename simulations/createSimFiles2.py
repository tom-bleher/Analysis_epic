import os
import sys
import re
import multiprocessing

def setup_paths(argv):
    inPath = "/" + argv[1] if len(argv) > 1 else ""
    outPath = "/" + argv[2] if len(argv) > 2 else inPath

    genPath = "genEvents{0}".format(inPath)
    simPath = "simEvents{0}".format(outPath)
    return genPath, simPath

def check_paths(simPath):
    if not os.path.exists(simPath):
        raise FileNotFoundError("Out dir doesn't exist. Create a dir called " + simPath)
    if len(os.listdir(simPath)) != 0:
        raise FileExistsError("{0} directory not empty. Clear directory".format(simPath))

def copy_compact_dir(simPath):
    det_dir = os.environ['DETECTOR_PATH']
    compact_dir = det_dir + '/compact'
    os.system('cp -r {0} {1}'.format(compact_dir, simPath))

def generate_commands(genPath, simPath, epicPath, fileType):
    commands = []
    for file in sorted(os.listdir(genPath)):
        if fileType not in file:
            continue
        inFile = os.path.join(genPath, file)
        fileNum = re.search(r"\d+\.+\d\.", inFile).group()
        cmd = "ddsim --inputFiles {0} --outputFile {1}/output_{2}edm4hep.root --compactFile {3} -N 5000".format(inFile, simPath, fileNum, epicPath)
        commands.append(cmd)
    return commands

def run_simulation(commands, pool_size=8):
    pool = multiprocessing.Pool(pool_size)
    pool.map(runSims, commands)

def runSims(cmd):
    os.system(cmd)

if __name__ == "__main__":
    fileType = "beamEffectsElectrons"
    epicPath = "/home/dhevan/eic/epic/epic_ip6_extended.xml"

    genPath, simPath = setup_paths(sys.argv)
    check_paths(simPath)
    copy_compact_dir(simPath)
    commands = generate_commands(genPath, simPath, epicPath, fileType)
    run_simulation(commands)

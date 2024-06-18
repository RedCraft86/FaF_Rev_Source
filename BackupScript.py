#--------------------------------------------------------------------#
#============================ PARAMETERS ============================#
#--------------------------------------------------------------------#

BASE_PATH = "D:/UnrealEngine/SymLinks/FaF_Rev/"
BACKUP_PATH = "F:/Backups/FaF_Rev_Content"

ARCHIVE_LEVEL = 0
UNIQUE_FILES = ["Marketplace"]

#--------------------------------------------------------------------#
#====================================================================#
#--------------------------------------------------------------------#

from datetime import datetime
from subprocess import Popen
import shutil
import time
import os

def Archive(Output, Inputs):
    cmd = ["nanazipc", "a", "-tzip", "-mx=" + str(ARCHIVE_LEVEL), Output] + Inputs
    p = Popen(cmd, shell=True, text=True)
    while p.poll() != 0:
        time.sleep(1)

assert os.path.isdir(BASE_PATH), "BASE_PATH does not exist."
assert os.path.isdir(BACKUP_PATH), "BACKUP_PATH does not exist."

CONTENT_PATH = os.path.join(BASE_PATH, "Content/").replace("\\", "/")
ARCHIVE_PATH = os.path.join(BASE_PATH, "Archives/").replace("\\", "/")

if ARCHIVE_LEVEL < 0:
    ARCHIVE_LEVEL = 0
# No higher than 2
elif ARCHIVE_LEVEL > 2:
    ARCHIVE_LEVEL = 2

LockFile = open(CONTENT_PATH + "Backup.Lock", 'w')
LockFile.write("Backing up content...")
LockFile.flush()

DateTime = datetime.now().strftime("%Y-%m-%d_%H.%M.%S")
Content = [f for f in os.scandir(CONTENT_PATH) if f.is_dir() and f.name != "Collections" and f.name != "Developer"]

ContentPaths = []
AlternateFiles = []
for file in Content:
    if file.name not in UNIQUE_FILES:
        ContentPaths.append(file.path.replace("\\", "/") + "/")
    else:
        AlternateFiles.append(file)

DirPairs = {"Content" + "_" + str(DateTime) + ".zip": ContentPaths}
for AltFile in AlternateFiles:
    DirPairs[AltFile.name + "_" + str(DateTime) + ".zip"] = [AltFile.path.replace("\\", "/") + "/"]

for output, inputs in DirPairs.items():
    Archive(ARCHIVE_PATH + output, inputs)

time.sleep(5)
LockFile.close()
os.remove(CONTENT_PATH + "Backup.Lock")
LockFile = open(ARCHIVE_PATH + "Backup.Lock", 'w')
LockFile.write("Copying content...")
LockFile.flush()

for file in DirPairs.keys():
    try:
        shutil.move(ARCHIVE_PATH + file, BACKUP_PATH + file)
    finally:
        continue

time.sleep(5)
LockFile.close()
os.remove(ARCHIVE_PATH + "Backup.Lock")

from datetime import datetime
from subprocess import Popen
import pathlib
import time
import os

OUT_PATH = "F:/Backups/FaF_Rev_Content/"
CONTENT_PATH = "D:/UnrealEngine/SymLinks/FaF_Rev/Content/"
DateTime = ""

def Archive(Output, Inputs):
    cmd = "nanazipc a -tzip -mx=0 " + OUT_PATH + Output + "_" + DateTime + ".zip"
    for Input in Inputs:
        cmd += " " + Input

    cmd = cmd.replace("\\", "/").replace("//", "/")
    p = Popen(cmd.split(), shell=True, text=True)
    while p.poll() != 0:
        time.sleep(1)

    return

ThisDir = str(pathlib.Path(__file__).parent.resolve()).replace("\\", "/") + "/"
LockFile = open(ThisDir + "Backup.Lock", 'w')
LockFile.write("Backing up content...")
LockFile.flush()

# noinspection PyRedeclaration
DateTime = datetime.now().strftime("%Y-%m-%d_%H.%M.%S")
ContentFiles = [f.name for f in os.scandir(ThisDir + "Content/") if f.is_dir()]

NonPacks = []
Marketplace = ""
for Content in ContentFiles:
    if "Collections" in Content or "Developer" in Content:
        continue
    Path = CONTENT_PATH + Content + "/"
    if os.path.isdir(Path):
        if "Marketplace" in Content:
            Marketplace = Path
        else:
            NonPacks.append(Path)

Archive("NonPacks", NonPacks)
Archive("Marketplace", [Marketplace])

time.sleep(1)
LockFile.close()
os.remove(ThisDir + "Backup.Lock")

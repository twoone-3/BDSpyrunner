import os
import sys

def getFileLength(filePath):
    try:
        with open(filePath, "rb") as file:
            return len(file.read())
    except:
        return None

def downloadCVDUMP():
    if getFileLength("cvdump.exe") != 979968:
        for count in range(3):
            print()
            os.system("del /f /q cvdump.exe")
            print("Downloading cvdump.exe")
            if os.system("curl -o cvdump.exe https://raw.githubusercontent.com/microsoft/microsoft-pdb/master/cvdump/cvdump.exe") == 0:
                if getFileLength("cvdump.exe") == 979968:
                    print("Downloaded cvdump.exe")
                    break
            else:
                print(f"Failed to download cvdump.exe, retrying...({count+1}/3)")
        if getFileLength("cvdump.exe") != 979968:
            print("Failed to download cvdump.exe, please try again or download it manually and put it in the same folder as this script")
            print("Url: https://raw.githubusercontent.com/microsoft/microsoft-pdb/master/cvdump/cvdump.exe")
            sys.exit(1)

def readPDBSymbols(pdbFilePath) -> list:
    with open(pdbFilePath, "r", encoding="utf-8") as infoFile:
        pdbSymbols = []
        lines = infoFile.readlines()
        for line in lines:
            if "?" in line and "Z" in line:
            #if "0x" in line:
                pdbSymbols.append(line[43:-1])
                #print(line[43:-1])
        print(f"Loaded {len(pdbSymbols)} symbols from pdb file")
        return pdbSymbols

def readPluginSymbols(sourceCodePath) -> list:
    fileList = getFileList(sourceCodePath)
    pluginSymbols = []
    for filePath in fileList:
        try:
            with open(filePath, "r", encoding="utf-8") as sourceFile:
                lines = sourceFile.readlines()
                for line in lines:
                    if r'"?' in line and r'Z"' in line:
                        strBegin = line.find(r'"?') + 1
                        strEnd = line.find(r'Z"') + 1
                        #print(line[strBegin:strEnd])
                        pluginSymbols.append(line[strBegin:strEnd])
        except:
            with open(filePath, "r") as sourceFile:
                lines = sourceFile.readlines()
                for line in lines:
                    if r'"?' in line and r'Z"' in line:
                        strBegin = line.find(r'"?') + 1
                        strEnd = line.find(r'Z"') + 1
                        #print(line[strBegin:strEnd])
                        pluginSymbols.append(line[strBegin:strEnd])
        if verboseMode:
            print(f"Processed file: {filePath}")
    print(f"Loaded {len(pluginSymbols)} symbols from source code files")
    return pluginSymbols

def getFileList(sourceCodePath) -> list:
    fileList = []
    for root, dirs, files in os.walk(sourceCodePath):
        for file in files:
            if ".h" in file or ".cpp" in file:
                fileList.append(os.path.join(root, file))
    return fileList

def searchStrInFile(filePath, str):
    try:
        with open(filePath, "r", encoding="utf-8") as sourceFile:
            lines = sourceFile.readlines()
            for line in lines:
                if "//" in line:  # Does not match the symbol in the comment
                    #pass
                    continue
                if str in line:
                    return True
    except:
        return False

def compare():
    pdbSymbols = readPDBSymbols("pdbInfo.txt")
    pluginSymbols = readPluginSymbols(sourceCodePath)
    count = 0
    print()
    for symbol in pluginSymbols:
        if symbol in pdbSymbols:
            if verboseMode:
                print(f"Found symbol: \n{symbol}\n")
            pass
        else:
            # The search algorithm needs to be improved, it runs too slow in large projects
            for filePath in getFileList(sourceCodePath):
                if searchStrInFile(filePath, symbol):
                    count += 1
                    print(f"Missing symbol in file {filePath}: \n{symbol}\n")
            #print(f"Missing symbol: {symbol}")
            pass
    print(f"Missing {count} symbols")

if __name__ == "__main__":
    try:
        pdbFilePath = sys.argv[1]
        sourceCodePath = sys.argv[2]
        verboseMode = sys.argv[3]
        overWrite = sys.argv[4]
        if verboseMode != "y":
            verboseMode = False
        if overWrite != "y":
            overWrite = False
    except:
        pdbFilePath = input(r"pdb file path (such as E:\bds\bedrock_server.pdb): ")
        sourceCodePath = input("plugin source code path: ")
        verboseMode = input("type y to enable verbose mode: ")
        overWrite = input("type y to overwrite the cache: ")
        if verboseMode != "y":
            verboseMode = False
        if overWrite != "y":
            overWrite = False

    #os.system("del /f /q pdbInfo.txt")
    print("\nSymbol Checker For BDS Projects\n")
    downloadCVDUMP()
    if overWrite:
        print("Generating pdbInfo.txt...")
        os.system(f".\cvdump.exe -headers -p {pdbFilePath} > pdbInfo.txt")
    compare()
    #os.system("del /f /q pdbInfo.txt")
import os
import time
import chardet
import argparse
import pefile
import requests
import colorama
from colorama import Fore


def is_executable(filename) -> bool:
    try:
        pe = pefile.PE(filename)

        # 检查是否有可执行代码段
        if not any(s.SizeOfRawData > 0 for s in pe.sections):
            return False

        # 检查是否有导入表
        if not hasattr(pe, "DIRECTORY_ENTRY_IMPORT"):
            return False

        # 检查是否有导出表
        # if not hasattr(pe, "DIRECTORY_ENTRY_EXPORT"):
        #    return False

        # 检查是否有资源表
        if not hasattr(pe, "DIRECTORY_ENTRY_RESOURCE"):
            return False

        # 文件完整
        return True
    except pefile.PEFormatError:
        # 文件不完整
        return False
    except OSError:
        # File not found
        return False


colorama.init()  # 初始化 colorama 库


def printc(text, color=""):
    """打印带有颜色的文本"""
    print(color + text + Fore.RESET)


def downloadCVDUMP():
    url = "https://raw.fgit.ml/microsoft/microsoft-pdb/master/cvdump/cvdump.exe"
    if not is_executable("cvdump.exe"):
        for retries in range(3):
            print()
            printc("Downloading cvdump.exe", Fore.CYAN)
            response = requests.get(url)
            if response.status_code == 200:
                printc("Connected!", Fore.GREEN)
                # 写入文件
                with open("cvdump.exe", "wb") as file:
                    file.write(response.content)
            if is_executable("cvdump.exe"):
                printc("Downloaded cvdump.exe", Fore.GREEN)
                print()
                break
            else:
                printc(
                    f"Failed to download cvdump.exe, retrying...({retries+1}/3)", Fore.RED)
        if not is_executable("cvdump.exe"):
            printc("Failed to download cvdump.exe, please try again or download it manually and put it in the same folder as this script", Fore.RED)
            printc("Url: " + url, Fore.CYAN)
            os._exit(1)


def readPDBSymbols(pdbFilePath) -> list:
    with open(pdbFilePath, "r", encoding=get_file_encoding(pdbFilePath)) as infoFile:
        pdbSymbols = []
        lines = (line for line in infoFile)
        for lineNumber, line in enumerate(lines):
            if "?" in line and "Z" in line:
                # if "0x" in line:
                pdbSymbols.append(line[43:-1])
                # print(line[43:-1])
        printc(f"Loaded {len(pdbSymbols)} symbols from pdb file", Fore.GREEN)
        return pdbSymbols


def readPluginSymbols(source_code_path) -> list:
    fileList = getFileList(source_code_path)
    pluginSymbols = []
    for filePath in fileList:
        with open(filePath, "r", encoding=get_file_encoding(filePath)) as sourceFile:
            lines = (line for line in sourceFile)
            for lineNumber, line in enumerate(lines):
                if r'"?' in line and r'Z"' in line:
                    strBegin = line.find(r'"?') + 1
                    strEnd = line.find(r'Z"') + 1
                    # print(line[strBegin:strEnd])
                    pluginSymbols.append(line[strBegin:strEnd])
        if verbose_mode:
            print(f"Processed file: {filePath}")
    printc(
        f"Loaded {len(pluginSymbols)} symbols from source code files", Fore.GREEN)
    return pluginSymbols


def getFileList(source_code_path) -> list:
    fileList = []
    for root, dirs, files in os.walk(source_code_path):
        for file in files:
            if ".h" in file or ".cpp" in file:
                fileList.append(os.path.join(root, file))
    return fileList


def searchStrInFile(filePath, str) -> int:
    with open(filePath, "r", encoding=get_file_encoding(filePath)) as sourceFile:
        lines = (line for line in sourceFile)
        for lineNumber, line in enumerate(lines):
            if "//" in line:  # Does not match the symbol in the comment
                # pass
                continue
            if str in line:
                return lineNumber + 1
    return 0


def get_file_encoding(filePath) -> str:
    with open(filePath, "rb") as f:
        data = f.read()
        result = chardet.detect(data)
        return result["encoding"]


def compare():
    pdbSymbols = readPDBSymbols("pdbInfo.txt")
    pluginSymbols = readPluginSymbols(source_code_path)
    count = 0
    print()
    for symbol in pluginSymbols:
        if symbol in pdbSymbols:
            if verbose_mode:
                printc(f"Found symbol: \n{symbol}\n", Fore.GREEN)
        else:
            # The search algorithm needs to be improved, it runs too slow in large projects
            for filePath in getFileList(source_code_path):
                searchResult = searchStrInFile(filePath, symbol)
                if searchResult != 0:
                    count += 1
                    printc(
                        f"Missing symbol at file {filePath}:{searchResult}", Fore.LIGHTRED_EX)
                    printc(symbol, Fore.YELLOW)
                    for index in range(3, len(symbol), 3):
                        similarSymbols = list(
                            filter(lambda x: symbol[:index] in x, pdbSymbols))
                        if len(similarSymbols) <= 3:
                            printc("Similar symbols in symbol file:", Fore.GREEN)
                            printc("\n".join(similarSymbols),
                                   Fore.LIGHTBLUE_EX)
                            break
    color = Fore.LIGHTRED_EX
    if count == 0:
        color = Fore.LIGHTGREEN_EX
    printc(f"Missing {count} symbols", color)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("pdb_file_path", help="the path to the pdb file")
    parser.add_argument("source_code_path", help="the path to the source code")
    parser.add_argument("--verbose", "-v", action="store_true",
                        help="enable verbose mode")
    parser.add_argument("--overwrite", "-o",
                        action="store_true", help="overwrite the cache")

    args = parser.parse_args()

    pdb_file_path = args.pdb_file_path
    source_code_path = args.source_code_path
    verbose_mode = args.verbose
    overwrite = args.overwrite

    #os.system("del /f /q pdbInfo.txt")
    printc("\nSymbol Checker For BDS Projects\n", Fore.LIGHTCYAN_EX)
    downloadCVDUMP()
    if overwrite:
        print("Generating pdbInfo.txt...")
        os.system(f".\cvdump.exe -headers -p {pdb_file_path} > pdbInfo.txt")
    compare()

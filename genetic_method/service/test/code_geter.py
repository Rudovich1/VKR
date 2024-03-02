from pathlib import Path
import os
import codecs

ROW_FILE_PATH = Path("genetic_method/service/test/row_res.txt")
CPP_FILE_PATH = Path("genetic_method/service/test/res.cpp")

def make_cpp():
    row_data = open(ROW_FILE_PATH, "rb")
    row_bytes = row_data.read()
    row_data.close()
    data = codecs.decode(row_bytes, "unicode_escape")
    data = data[1:-2]
    cpp_file = open(CPP_FILE_PATH, "w")
    cpp_file.write(data)
    cpp_file.close()
    

if __name__ == "__main__":
    make_cpp()
    
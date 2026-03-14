import os
import sys
import string
import re

def fileType(file):
    if "wdr_night_attr" in file:
        return 0
    elif "wdr_attr" in file:
            return 1
    elif "night_attr" in file:
        return 2
    else:
        return 3

inFile = sys.argv[1]
outFile = "sensorParam_" + sys.argv[2]
destFolder = sys.argv[3]
#print(inFile)
#print(outFile)
# print(fileType(inFile))

sensor = re.findall(r"(.+?)_night_attr", inFile)

if len(sensor) == 0:
    sensor = re.findall(r"(.+?)_attr", inFile)

sensor=str(sensor)
sensor=sensor[2:-2].upper()

if 'WDR' in sensor:
    sensor = sensor[0:-4]
# print(sensor)

# arrayName = re.findall("mipi_"r"(.+?)_attr", inFile)
# print(arrayName)
# arrayName = str(arrayName)
# arrayName = arrayName[2:-2]
# print(arrayName)

fileIn = open(inFile, "rb")
fileOut = open(destFolder + "/" + outFile, "w")
fileSize = os.path.getsize(inFile)

string = "#include <rtthread.h>  //include rtthread.h to use setting in rtconfig.h\n"
string += "#include \"multi_sensor.h\"\n\n"
string += "#ifdef RT_USING_" + sensor + "\n\n"
string += "#define _NAME(n, s) n##_##s\n"
string += "#ifdef FH_USING_MULTI_SENSOR\n"
string += "#define NAME(n) _NAME(n, "+ sensor + ")\n"
string += "#else\n"
string += "#define NAME(n) n\n"
string += "#endif\n\n"
if fileType(inFile) == 0:
    string += "SENSOR_SECTION char NAME(isp_param_buff_wdr_night)[] = {\n    "
elif fileType(inFile) == 1:
    string += "SENSOR_SECTION char NAME(isp_param_buff_wdr)[] = {\n    "
elif fileType(inFile) == 2:
    string += "SENSOR_SECTION char NAME(isp_param_buff_night)[] = {\n    "
else:
    string += "SENSOR_SECTION char NAME(isp_param_buff)[] = {\n    "

for i in range(0,fileSize):
    num = fileIn.read(1)
    num = hex(ord(num))
    num = "0x" + num[2:].rjust(2, '0')
    string += str(num)
    if ((i + 1) % 4) == 0 and i != 0:
        if i + 1 < fileSize:
            string += ","
            string += "  // 0x" + hex(i - 3)[2:].rjust(4,'0')
            string += "\n    "
        else:
            string += ","
            string += "  // 0x" + hex(i - 3)[2:].rjust(4,'0')
    else:
        string += ", "
string += "\n};\n"
string += "#endif\n"

fileOut.write(string)
fileIn.close()
fileOut.close()




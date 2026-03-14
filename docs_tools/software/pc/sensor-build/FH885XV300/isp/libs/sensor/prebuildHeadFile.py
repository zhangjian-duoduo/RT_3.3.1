import os
import sys

def produceFileHead(sensor):
    name = sensor.upper()
    string = "#include <rtthread.h>  //include rtthread.h to use setting in rtconfig.h\n"
    string += "#include \"multi_sensor.h\"\n\n"
    string += "#ifdef RT_USING_" + name + "\n\n"
    string += "#define _NAME(n, s) n##_##s\n"
    string += "#ifdef FH_USING_MULTI_SENSOR\n"
    string += "#define NAME(n) _NAME(n, "+ name + ")\n"
    string += "#else\n"
    string += "#define NAME(n) n\n"
    string += "#endif\n\n"
    return string

def produceDayParaFile(sensor, destFolder):
    string = produceFileHead(sensor)
    string += "SENSOR_SECTION char NAME(isp_param_buff)[];\n"
    string += "#endif\n"
    fileName = "sensorParam_" + sensor + "_attr.c"
    file = open(destFolder + "/" + fileName, "w")
    file.write(string)
    file.close()

def produceNightParaFile(sensor, destFolder):
    string = produceFileHead(sensor)
    string += "SENSOR_SECTION char NAME(isp_param_buff_night)[];\n"
    string += "#endif\n"
    fileName = "sensorParam_" + sensor + "_night_attr.c"
    file = open(destFolder + "/" + fileName, "w")
    file.write(string)
    file.close()

def produceDayWdrParaFile(sensor, destFolder):
    string = produceFileHead(sensor)
    string += "SENSOR_SECTION char NAME(isp_param_buff_wdr)[];\n"
    string += "#endif\n"
    fileName = "sensorParam_" + sensor + "_wdr_attr.c"
    file = open(destFolder + "/" + fileName, "w")
    file.write(string)
    file.close()

def produceNightWdrParaFile(sensor, destFolder):
    string = produceFileHead(sensor)
    string += "SENSOR_SECTION char NAME(isp_param_buff_wdr_night)[];\n"
    string += "#endif\n"
    fileName = "sensorParam_" + sensor + "_wdr_night_attr.c"
    file = open(destFolder + "/" + fileName, "w")
    file.write(string)
    file.close()

sensor = sys.argv[1]
destFolder = sys.argv[2]

produceDayParaFile(sensor, destFolder)

produceNightParaFile(sensor, destFolder)

produceDayWdrParaFile(sensor, destFolder)

produceNightWdrParaFile(sensor, destFolder)




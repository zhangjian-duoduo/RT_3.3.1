#!/usr/bin/env python3
import os
import sys

def app_h_check():
    config_h = open('config.h', 'r')
    multi_check = 0
    multi_sensor = 0
    sensor_num = 0

    for line in config_h:
        if '/* Choose Sensor */' in line:
            multi_check = 1
        if 'FH_APP_USING_FORMAT' in line:
            multi_check = 0

        if line[0] == '#':
            if 'FH_USING_MULTI_SENSOR' in line:
                multi_sensor = 1
            if multi_check:
                if 'RT' in line:
                    sensor_num += 1
                    if sensor_num > 1 and multi_sensor == 0:
                        print("Error: Multi_sensor needs to be chosen")
                        sys.exit(2)

if __name__ == '__main__':
    app_h_check()

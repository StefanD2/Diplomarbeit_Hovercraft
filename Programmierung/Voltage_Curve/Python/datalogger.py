import serial
import time
import os
import csv
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from datetime import datetime

ser = serial.Serial()
ser.port = 'COM14'
ser.baudrate=115200
ser.open()
time.sleep(2)
ser.timeout=5

print("connected to: " + ser.portstr)
volts=[]
percent=[]
rpm=[]
time=[]
try:
    while True:
        inp = ser.readline()
        now = datetime.now()
        inp = inp.rstrip().decode()
        if inp=='':
            continue
        inp=inp.split('-')
        print(inp)
        volts.append(inp[0])
        percent.append(inp[1])
        rpm.append(inp[2])
        time.append(now)
        print(now,": ",inp)
except KeyboardInterrupt:
    print('interrupted!')


voltages=np.array(volts).astype(np.str)
percents=np.array(percent).astype(np.str)
drehzahl=np.array(rpm).astype(np.str)
times=np.array(time).astype(np.str)


combined_data=np.vstack((times,voltages,percents,drehzahl)).T
testframe=pd.DataFrame(combined_data,columns=['Time', 'Volts','Percent','rpm'])

testframe.to_csv('voltages_log03.csv',index=False,header=True)

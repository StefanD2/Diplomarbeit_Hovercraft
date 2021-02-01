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
ser.port = 'COM4'
ser.baudrate=115200
ser.open()
time.sleep(2)
ser.timeout=5

print("connected to: " + ser.portstr)
data=[]
time=[]
try:
    while True:
        inp = ser.readline()
        now = datetime.now()
        inp = inp.rstrip().decode()
        data.append(inp)
        time.append(now)
        print(now,": ",inp)
except KeyboardInterrupt:
    print('interrupted!')

print(data)

voltages=np.array(data).astype(np.str)
times=np.array(time).astype(np.str)


combined_data=np.vstack((times,voltages)).T
testframe=pd.DataFrame(combined_data,columns=['Time', 'Volts'])

testframe.to_csv('voltages_log02.csv',index=False,header=True)

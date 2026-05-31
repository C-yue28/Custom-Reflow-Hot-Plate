import math
import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import time

ser = serial.Serial('COM8', 115200, timeout=1)

time.sleep(2)

x_data = []
target_data = []
actual_data = []
error_data = []

fig, ax = plt.subplots()
(target_data_line,) = ax.plot([], [], "r-", marker='None', label="Target")
(actual_data_line,) = ax.plot([], [], "b-", marker='None', label="Actual")
(error_data_line,) = ax.plot([], [], "g-", marker='None', label="Error")

def init():
    ax.set_xlim(0, 360)
    ax.set_ylim(-20, 280)
    ax.set_title("Target & Actual Temperature & Error vs. Time")
    ax.set_xlabel("Time (s)")
    ax.set_ylabel("Temperature (C)")
    ax.legend(loc="upper left")
    return (target_data_line, actual_data_line, error_data_line)

begin = True
START_TIME = -1

def update(x_frame):
    global target_data_line, actual_data_line, error_data_line, begin, START_TIME

    dataline = None

    if ser.in_waiting > 0:
        dataline = ser.readline().decode('utf-8').rstrip()

    if dataline is None:
        return

    print(dataline)

    unpacked_data = dataline.split(",")
    if len(unpacked_data) <= 2: 
        return

    try:
        (time, target, actual, error) = (float(piece.split(":")[1]) for piece in unpacked_data)
    except Exception as e:
        return
    
    if begin:
        START_TIME = time
        begin = False

    if START_TIME == -1:
        return

    x_data.append(round((time-START_TIME)/1000.0))
    target_data.append(target)
    actual_data.append(actual)
    error_data.append(error)

    target_data_line.set_data(x_data, target_data)
    actual_data_line.set_data(x_data, actual_data)
    error_data_line.set_data(x_data, error_data)

    return target_data_line, actual_data_line, error_data_line

ani = FuncAnimation(
    fig, update, init_func=init, interval=1, cache_frame_data=False
)

plt.show()


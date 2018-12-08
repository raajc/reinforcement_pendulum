#Uses Serial Library
import serial
import time
from pytictoc import TicToc
import PyDuino as PD

t=TicToc()

ser = serial.Serial('COM20', 115200)  # Initialize serial port

print("connected to: " + ser.portstr)  # Confirm connection

# def read16bit(ser):#Function to read input from serial
#     ch = int.from_bytes(ser.read(2), byteorder='big')  # Combine both bytes received over serial
#     return ch
#
# def writePWM(ser, toSend, dir): #Function To Write to Arduino
#     if dir == "R":
#         str1 = str(toSend).encode()#Actual message being sent
#         str2 = "R".encode() #Arduino Endline Character
#         strSend = str1+str2 #Combine
#         ser.write(strSend) #Send
#
#     if dir == "L":
#         str1 = str(toSend).encode()  # Actual message being sent
#         str2 = "L".encode()  # Arduino Endline Character
#         strSend = str1 + str2  # Combine
#         ser.write(strSend)  # Send
#     #time.sleep(.16)
#
# def getPEncoderPos(ser): #Function To Write to Arduino
#     ser.write("E".encode()) #Send
#     data = read16bit(ser)-32768
#     #time.sleep(.16) #If you get rid of this make sure you are not reading from arduino more than this
#     return data
#
# def getMEncoderPos(ser): #Function To Write to Arduino
#     ser.write("M".encode()) #Send
#     data = read16bit(ser)-32768
#     #time.sleep(.3) #If you get rid of this make sure you are not reading from arduino more than this
#     return data
#
# def getBothEncoderPos(ser): #Function To Write to Arduino
#     ser.write("E".encode()) #Send
#     pdata = read16bit(ser)-32768
#     ser.write("M".encode()) #Send
#     mdata = read16bit(ser)-32768
#     #time.sleep(.3) #If you get rid of this make sure you are not reading from arduino more than this
#     return pdata, mdata


Timer = 0
period = 150
vel = 0
pos = 0
prevPos = 0

kp = 5
kd = 0
offset = 0
PWM = 0

t.tic()
t.tocvalue()
while True: #Loop to send 5000 to arduino and read it when arduino sends it bac

    if(t.tocvalue()*1000 - Timer >period):
        Timer = t.tocvalue()*1000
        print("Timer")
        print(Timer)
        pos = PD.getPEncoderPos(ser)
        print(pos)
        vel = pos-prevPos
        print(vel)
        PWM = kp*pos+offset+kd*vel
        print(PWM)
        prevPos = pos
        if (PWM>0):
            PD.writePWM(ser,PWM,"L")
        if (PWM<0):
            PD.writePWM((ser,-1*PWM,"R")
    prevPos = pos
ser.close() #Close Serial

#To Do: Make these functions
    #ReadEncoder
    #WritePWM
    #WriteDirection
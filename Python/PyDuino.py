#Uses Serial Library
import serial
import time
import keyboard

ser = serial.Serial('COM20', 115200)  # Initialize serial port

print("connected to: " + ser.portstr)  # Confirm connection

def read16bit():#Function to read input from serial
    ch = int.from_bytes(ser.read(2), byteorder='big')  # Combine both bytes received over serial
    return ch

def writePWM(toSend, dir): #Function To Write to Arduino
    if dir == "R":
        str1 = str(toSend).encode()#Actual message being sent
        str2 = "R".encode() #Arduino Endline Character
        strSend = str1+str2 #Combine

        ser.write(strSend) #Send
    if dir == "L":
        str1 = str(toSend).encode()  # Actual message being sent
        str2 = "L".encode()  # Arduino Endline Character
        strSend = str1 + str2  # Combine

        ser.write(strSend)  # Send

def getPEncoder(): #Function To Write to Arduino
    ser.write("E".encode()) #Send
    data = read16bit()-32768
    time.sleep(.16) #If you get rid of this make sure you are not reading from arduino more than this
    return data
def getMEncoder(): #Function To Write to Arduino
    ser.write("M".encode()) #Send
    data = read16bit()-32768
    time.sleep(.16) #If you get rid of this make sure you are not reading from arduino more than this
    return data

def getBothEncoder(): #Function To Write to Arduino
    ser.write("E".encode()) #Send
    pdata = read16bit()-32768
    ser.write("M".encode()) #Send
    mdata = read16bit()-32768
    time.sleep(.16) #If you get rid of this make sure you are not reading from arduino more than this
    return pdata, mdata


while True: #Loop to send 5000 to arduino and read it when arduino sends it back
    pData, mData = getBothEncoder()

    print(pData)
    print(mData)

    writePWM(50,"L")
    time.sleep(1)
    writePWM(50, "R")
    time.sleep(1)






ser.close() #Close Serial

#To Do: Make these functions
    #ReadEncoder
    #WritePWM
    #WriteDirection
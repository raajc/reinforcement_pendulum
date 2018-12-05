#Uses Serial Library
import serial

ser = serial.Serial('COM19', 115200)  # Initialize serial port

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

def getEcoder(): #Function To Write to Arduino
    ser.write("E".encode()) #Send
    data = read16bit()
    return data

while True: #Loop to send 5000 to arduino and read it when arduino sends it back
    writePWM(500, "R")
    data = read16bit()
    if data:
        print(data)

ser.close() #Close Serial

#To Do: Make these functions
    #ReadEncoder
    #WritePWM
    #WriteDirection
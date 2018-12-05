#Uses Serial Library
import serial

ser = serial.Serial('COM19', 115200)  # Initialize serial port

print("connected to: " + ser.portstr)  # Confirm connection

def read16bit():#Function to read input from serial
    ch = int.from_bytes(ser.read(2), byteorder='big')  # Combine both bytes received over serial
    return ch

def writeToSerial(toSend): #Function To Write to Arduino
    str1 = str(toSend).encode()#Actual message being sent
    str2 = "#".encode() #Arduino Endline Character
    strSend = str1+str2 #Combine

    ser.write(strSend) #Send


while True: #Loop to send 5000 to arduino and read it when arduino sends it back
    writeToSerial(5000)
    data = read16bit()
    if data:
        print(data)

ser.close() #Close Serial

#To Do: Make these functions
    #ReadEncoder
    #WritePWM
    #WriteDirection
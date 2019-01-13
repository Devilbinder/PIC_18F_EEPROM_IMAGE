import serial
import cv2

try:
	ser = serial.Serial("COM9", 9600, timeout=1)
	
except:
	print ("ERROR: Failed to open serial port!")
	exit()

imag = cv2.imread('../image/binder_tronics_16x16.jpg',0)


print(imag)
for i in range(0,len(imag)):
	for j in range(0,len(imag[i])):
		
		if(imag[i][j] < 128):
			imag[i][j] = 0
		else:
			imag[i][j] = 255
			
		ser.write(bytes([int(imag[i][j])]))
		while(1):
			if(ser.read(1)==b'\r'):
				break


try:
    ser.close()
except:
    print ("ERROR: No port open")	



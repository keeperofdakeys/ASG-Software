#!/usr/bin/env python

# HACKY ASG RANDOM MOVEMENT CODE LAWL
# Copyleft 2013 Darkside
#
# ASG Command Set:
# < - move left
# > - move right
# Multiple of the movement commands will cause the ASG to move further
# s - stop
# f - fire
# p - report position (raw ADC value)

import serial,random,time

left_pos = 400
right_pos = 900
centre_pos = 680

fuzz_factor = 40

serial_baud = 9600
serial_device = "/dev/tty.usbmodemfd1221"

s = serial.Serial(serial_device,serial_baud,timeout=1)

while(True):
    setpoint = random.randrange(left_pos,right_pos)
    print "Setpoint: ",str(setpoint)
    s.write("p")
    current_pos = int(s.readline())
    print "Current pos: ",str(current_pos)
    
    if(current_pos < setpoint):
        s.write(">>>>>>>>>>>>>>>>>>>>>")
    else:
        s.write("<<<<<<<<<<<<<<<<<<<<<")
    
    while( not( (current_pos <= (setpoint+fuzz_factor)) and (current_pos >= (setpoint-fuzz_factor)) ) ):
        # If we aren't near the setpoint, try and move to it

        time.sleep(0.1)
        
        s.write("p")
        current_pos = int(s.readline())
        print "Setpoint: ",setpoint, " Current pos: ",str(current_pos)
       
    s.write("s")
    time.sleep(2)
    
    
        
    


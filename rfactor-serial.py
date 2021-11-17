"""
rfactor2python - serial port example
Author: Joao C. <me@joaoubaldo.com>
Edited: D. Nicklin <danicklin.co.uk>


On this example, gear and RPM telemetry data is transmitted as 1 byte over 
a serial connection:
- 4 bits for gear (G)
- 4 bits for RPM (R)
- bit order: GGGGRRRR

Example:
    Data: 3rd gear and 2 RPM LEDs
    Byte: 00110010    

This example uses PySerial (http://pyserial.sourceforge.net) module.
"""

#  Configuration
# Look inside RF2PyPlugin.__init__
#  /

import serial
import struct


class RF2PyPlugin(object):
    def __init__(self):
        self.PORT = "COM4"
        self.BPS = 9600
        self.RPM_LED_COUNT = 7  # number of LEDs to display RPMs

        self.ser = None

    # game startup
    def Startup(self):
        pass


    # game shutdown
    def Shutdown(self):
        pass


    # entering realtime (where the vehicle can be driven)
    def EnterRealtime(self):
        self.ser = serial.Serial(self.PORT,self.BPS)


    # exiting realtime
    def ExitRealtime(self):
        self.ser.close()


    # session started
    def StartSession(self):
        pass


    # session ended
    def EndSession(self):
        pass


    # update plugin with scoring info (approximately once per second)
    #   'info' is a dictionary with scoring data
    def UpdateScoring(self, info):
        pass

    #Packs a python 4 byte unsigned integer to an arduino unsigned long
    #def packIntegerAsULong(self,value):
        #return struct.pack('I', value)    #should check bounds


    # update plugin with telemetry info
    #   'info' is a dictionary with telemetry data
    #b = g << 4 & 0xFF | int(r/(mr/self.RPM_LED_COUNT))
    #self.ser.write(chr(int(b)))
    #self.ser.write(packIntegerAsULong(mr))
    def UpdateTelemetry(self, info):
        g = info["mGear"]
        r = info["mEngineRPM"]
        mr = info["mEngineMaxRPM"]
        wt = info["mEngineWaterTemp"]
        ot = info["mEngineOilTemp"]
        
        val = struct.pack("I", r)
        g = g & 0xFF
    
        self.ser.write(chr(int(mr/1000)))
        self.ser.write(chr(int(g)))
        self.ser.write(chr(int(ot)))
        self.ser.write(chr(int(wt)))
        self.ser.write(val)
        self.ser.write('\n')


    # See if the plugin wants to take over a hardware control.  If the plugin takes over the
    # control, this method returns true and sets the value of the float pointed to by the
    # second arg.  Otherwise, it returns false and leaves the float unmodified.
    #
    # Important: fRetVal is a list with only one value.
    # In order to modify this value you should do something like:
    # fRetVal[0] = newValue
    def CheckHWControl(self, controlName, fRetVal):
        return False

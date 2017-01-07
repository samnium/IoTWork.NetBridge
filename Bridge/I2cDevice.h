////////////////////////////////////////////////////////////////////////////
//
//  This file is part of NetBridgeLib
//
//  The MIT License (MIT)
//  Copyright (c) 2017 Gianluca Masone
//
//  Permission is hereby granted, free of charge, to any person obtaining
//  a copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//   Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//
////////////////////////////////////////////////////////////////////////////

#ifndef _NETBRIDGE_I2C_DEVICE_H
#define	_NETBRIDGE_I2C_DEVICE_H

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "Debug.h"
#include "Common.h"

class I2cDevice
{
public:
   I2cDevice ();
   virtual ~ I2cDevice ();

   unsigned char m_I2CBus;	// I2C bus of the imu (eg 1 for Raspberry Pi usually)

   void SetDeviceId(int Id);
   bool Open ();
   void Close ();
   bool Read (unsigned char slaveAddr, unsigned char regAddr, unsigned char length, unsigned char * data, const char * errorMsg);	// normal read with register select
   bool Read (unsigned char slaveAddr, unsigned char length, unsigned char * data, const char * errorMsg);	// read without register select
   bool Write (unsigned char slaveAddr, unsigned char regAddr,
               unsigned char length, unsigned char const * data,
               const char * errorMsg);
   bool Write (unsigned char slaveAddr, unsigned char regAddr,
               unsigned char const data, const char * errorMsg);

   void delayMs (int milliSeconds);

protected:
   void I2CClose ();
   bool I2CSelectSlave (unsigned char slaveAddr, const char * errorMsg);
   bool ifWrite (unsigned char * data, unsigned char length);

private:
   int m_I2C;
   unsigned char m_currentSlave;
};

#endif // _NETBRIDGE_I2C_DEVICE_H

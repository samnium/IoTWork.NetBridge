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

#include <stdio.h>
#include <unistd.h>

#include "netbridge.h"
#include "I2cDevice.h"


I2cDevice::I2cDevice ()
{
   m_I2CBus = 255;
   m_currentSlave = 255;
   m_I2C = -1;
}

I2cDevice::~I2cDevice ()
{
   Close ();
}

void I2cDevice::SetDeviceId(int Id)
{
   m_I2CBus = Id;
}

bool I2cDevice::Open ()
{
   char
   buf[32];

   DEBUG_PRINT("m_I2C :: %d\n",m_I2C);

   if (m_I2C >= 0)
      return true;

   DEBUG_PRINT("m_I2CBus :: %d\n",m_I2CBus);

   if (m_I2CBus == 255)
   {
      HAL_ERROR ("No I2C bus has been set\n");
      return false;
   }

   sprintf (buf, "/dev/i2c-%d", m_I2CBus);
   m_I2C = open (buf, O_RDWR);

   if (m_I2C < 0)
   {
      HAL_ERROR1 ("Failed to open I2C bus %d\n", m_I2CBus);
      m_I2C = -1;
      return false;
   }

   DEBUG_PRINT("Done :: %d\n",m_I2C);

   return true;
}

void
I2cDevice::Close ()
{
   I2CClose ();
}

void
I2cDevice::I2CClose ()
{
   if (m_I2C >= 0)
   {
      close (m_I2C);
      m_I2C = -1;
      m_currentSlave = 255;
   }
}


bool
I2cDevice::Write (unsigned char slaveAddr, unsigned char regAddr,
                  unsigned char const data, const char * errorMsg)
{
   return Write (slaveAddr, regAddr, 1, &data, errorMsg);
}

bool
I2cDevice::Write (unsigned char slaveAddr, unsigned char regAddr,
                  unsigned char length, unsigned char const * data,
                  const char * errorMsg)
{
   DEBUG_PRINT("INPUT slaveAddr     %u\n",slaveAddr);
   DEBUG_PRINT("INPUT regAddr       %u\n",regAddr);
   DEBUG_PRINT("INPUT length        %u\n",length);
   DEBUG_PRINT("INPUT data          %p\n",data);
   if (length > 0 && data != NULL)
        DEBUG_PRINT("INPUT data0         %u\n",data[0]);

   int result;
   unsigned char txBuff[MAX_WRITE_LEN + 1];
   char * ifType;

   DEBUG_PRINT("I2CSelectSlave\n");

   if (!I2CSelectSlave (slaveAddr, errorMsg))
      return false;

   ifType = (char *) "I2C";

   if (length == 0)
   {
      result = ifWrite (&regAddr, 1);

      if (result < 0)
      {
         if (strlen (errorMsg) > 0)
            HAL_ERROR2 ("%s write of regAddr failed - %s\n", ifType,
                        errorMsg);

         return false;
      }
      else if (result != 1)
      {
         if (strlen (errorMsg) > 0)
            HAL_ERROR2 ("%s write of regAddr failed (nothing written) - %s\n",
                        ifType, errorMsg);

         return false;
      }
   }
   else
   {
      txBuff[0] = regAddr;
      memcpy (txBuff + 1, data, length);

      result = ifWrite (txBuff, length + 1);

      DEBUG_PRINT("result %d\n",result);

      if (result < 0)
      {
         if (strlen (errorMsg) > 0)
            HAL_ERROR3 ("%s data write of %d bytes failed - %s\n", ifType,
                        length, errorMsg);

         return false;
      }
      else if (result < (int) length)
      {
         if (strlen (errorMsg) > 0)
            HAL_ERROR4
            ("%s data write of %d bytes failed, only %d written - %s\n",
             ifType, length, result, errorMsg);

         return false;
      }
   }

   return true;
}

bool I2cDevice::ifWrite (unsigned char * data, unsigned char length)
{
   return write (m_I2C, data, length);
}


bool
I2cDevice::Read (unsigned char slaveAddr, unsigned char regAddr,
                 unsigned char length, unsigned char * data,
                 const char * errorMsg)
{
   int tries, result, total;

   if (!Write (slaveAddr, regAddr, 0, NULL, errorMsg))
      return false;

   total = 0;
   tries = 0;

   while ((total < length) && (tries < 5))
   {
      result = read (m_I2C, data + total, length - total);

      if (result < 0)
      {
         if (strlen (errorMsg) > 0)
            HAL_ERROR3 ("I2C read error from %d, %d - %s\n", slaveAddr,
                        regAddr, errorMsg);

         return false;
      }

      total += result;

      if (total == length)
         break;

      delayMs (10);
      tries++;
   }

   if (total < length)
   {
      if (strlen (errorMsg) > 0)
         HAL_ERROR3 ("I2C read from %d, %d failed - %s\n", slaveAddr, regAddr,
                     errorMsg);

      return false;
   }

   return true;
}

bool
I2cDevice::Read (unsigned char slaveAddr, unsigned char length,
                 unsigned char * data, const char * errorMsg)
{
   int tries, result, total;

   if (!I2CSelectSlave (slaveAddr, errorMsg))
      return false;

   total = 0;
   tries = 0;

   while ((total < length) && (tries < 5))
   {
      result = read (m_I2C, data + total, length - total);

      if (result < 0)
      {
         if (strlen (errorMsg) > 0)
            HAL_ERROR2 ("I2C read error from %d - %s\n", slaveAddr, errorMsg);

         return false;
      }

      total += result;

      if (total == length)
         break;

      delayMs (10);
      tries++;
   }

   if (total < length)
   {
      if (strlen (errorMsg) > 0)
         HAL_ERROR2 ("I2C read from %d failed - %s\n", slaveAddr, errorMsg);

      return false;
   }

   return true;
}


bool I2cDevice::I2CSelectSlave (unsigned char slaveAddr, const char * errorMsg)
{
   if (m_currentSlave == slaveAddr)
   {
      DEBUG_PRINT("Just Selected %d\n", m_currentSlave);
      return true;
   }

   if (!Open ())
   {
      HAL_ERROR1 ("Failed to open I2C port - %s\n", errorMsg);
      return false;
   }

   if (ioctl (m_I2C, I2C_SLAVE, slaveAddr) < 0)
   {
      HAL_ERROR2 ("I2C slave select %d failed - %s\n", slaveAddr, errorMsg);
      return false;
   }

   m_currentSlave = slaveAddr;

   DEBUG_PRINT("Selected %d\n", m_currentSlave);

   return true;
}

void
I2cDevice::delayMs (int milliSeconds)
{
   usleep (1000 * milliSeconds);
}




// ---------------------------------------------------------------
//
// C for marshalling
//
// ---------------------------------------------------------------


extern "C"  I2cDevice * __i2c_Create()
{
   return new I2cDevice();
}


extern "C"  void __i2c_Dispose(I2cDevice * device)
{
   if (device != NULL)
   {
      delete device;
      device = NULL;
   }
}

extern "C" void __i2c_SetDeviceId(I2cDevice * device, int Id)
{
   if (device != NULL)
   {
      device->SetDeviceId(Id);
   }
}


extern "C" bool __i2c_Open (I2cDevice * device)
{
   if (device != NULL)
   {
      return device->Open();
   }

   return false;
}


extern "C" void __i2c_Close (I2cDevice * device)
{
   if (device != NULL)
   {
      device->Close();
   }
}


extern "C" bool __i2c_Read_AtRegister (I2cDevice * device, unsigned char slaveAddr,
                                       unsigned char regAddr, unsigned char length,
                                       unsigned char * data, const char * errorMsg)
{
   if (device != NULL)
   {
      return device->Read(slaveAddr, regAddr, length, data, errorMsg);
   }

   return false;
}


extern "C" bool __i2c_Read (I2cDevice * device, unsigned char slaveAddr,
                            unsigned char length,
                            unsigned char * data, const char * errorMsg)
{
   if (device != NULL)
   {
      return device->Read(slaveAddr, length, data, errorMsg);
   }

   return false;
}


extern "C" bool __i2c_Write (I2cDevice * device,
                             unsigned char slaveAddr, unsigned char regAddr,
                             unsigned char length, unsigned char const * data,
                             const char * errorMsg)
{
   if (device != NULL)
   {
      return device->Write(slaveAddr, regAddr, length, data, errorMsg);
   }

   return false;
}


extern "C" bool __i2c_Write_Byte (I2cDevice * device, unsigned char slaveAddr,
                                  unsigned char regAddr,
                                  unsigned char const data, const char * errorMsg)
{
   if (device != NULL)
   {
      return device->Write(slaveAddr, regAddr, data, errorMsg);
   }

   return false;
}

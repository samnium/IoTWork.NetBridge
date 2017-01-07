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

#include <unistd.h>

#include <linux/spi/spidev.h>

#include "SpiDevice.h"


SpiDevice::SpiDevice ()
{
   m_currentSlave = 255;
   m_SPI = -1;
   m_SPISpeed = 500000;
}

SpiDevice::~SpiDevice ()
{
   Close ();
}

bool SpiDevice::Open ()
{
   char
   buf[32];
   unsigned char
   SPIMode = SPI_MODE_0;
   unsigned char
   SPIBits = 8;
   uint32_t
   SPISpeed = m_SPISpeed;

   if (m_SPIBus == 255)
   {
      HAL_ERROR ("No SPI bus has been set\n");
      return false;
   }

   sprintf (buf, "/dev/spidev%d.%d", m_SPIBus, m_SPISelect);
   m_SPI = open (buf, O_RDWR);

   if (m_SPI < 0)
   {
      HAL_ERROR2 ("Failed to open SPI bus %d, select %d\n", m_SPIBus,
                  m_SPISelect);
      m_SPI = -1;
      return false;
   }

   if (ioctl (m_SPI, SPI_IOC_WR_MODE, &SPIMode) < 0)
   {
      HAL_ERROR1 ("Failed to set WR SPI_MODE0 on bus %d", m_SPIBus);
      close (m_SPIBus);
      return false;
   }

   if (ioctl (m_SPI, SPI_IOC_RD_MODE, &SPIMode) < 0)
   {
      HAL_ERROR1 ("Failed to set RD SPI_MODE0 on bus %d", m_SPIBus);
      close (m_SPIBus);
      return false;
   }

   if (ioctl (m_SPI, SPI_IOC_WR_BITS_PER_WORD, &SPIBits) < 0)
   {
      HAL_ERROR1 ("Failed to set WR 8 bit mode on bus %d", m_SPIBus);
      close (m_SPIBus);
      return false;
   }

   if (ioctl (m_SPI, SPI_IOC_RD_BITS_PER_WORD, &SPIBits) < 0)
   {
      HAL_ERROR1 ("Failed to set RD 8 bit mode on bus %d", m_SPIBus);
      close (m_SPIBus);
      return false;
   }

   if (ioctl (m_SPI, SPI_IOC_WR_MAX_SPEED_HZ, &SPISpeed) < 0)
   {
      HAL_ERROR2 ("Failed to set WR %dHz on bus %d", SPISpeed, m_SPIBus);
      close (m_SPIBus);
      return false;
   }

   if (ioctl (m_SPI, SPI_IOC_RD_MAX_SPEED_HZ, &SPISpeed) < 0)
   {
      HAL_ERROR2 ("Failed to set RD %dHz on bus %d", SPISpeed, m_SPIBus);
      close (m_SPIBus);
      return false;
   }

   return true;
}

void
SpiDevice::Close ()
{
   SPIClose ();
}


void
SpiDevice::SPIClose ()
{
   if (m_SPI >= 0)
   {
      close (m_SPI);
      m_SPI = -1;
   }
}

bool
SpiDevice::Write (unsigned char slaveAddr, unsigned char regAddr,
                  unsigned char const data, const char * errorMsg)
{
   return Write (slaveAddr, regAddr, 1, &data, errorMsg);
}

bool
SpiDevice::Write (unsigned char slaveAddr, unsigned char regAddr,
                  unsigned char length, unsigned char const * data,
                  const char * errorMsg)
{
   int result;
   unsigned char txBuff[MAX_WRITE_LEN + 1];
   char * ifType;

   ifType = (char *) "SPI";

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

bool SpiDevice::ifWrite (unsigned char * data, unsigned char length)
{
   struct spi_ioc_transfer
      wrIOC;

   memset (&wrIOC, 0, sizeof (wrIOC));
   wrIOC.tx_buf = (unsigned long) data;
   wrIOC.rx_buf = 0;
   wrIOC.len = length;
   return ioctl (m_SPI, SPI_IOC_MESSAGE (1), &wrIOC);
}


bool
SpiDevice::Read (unsigned char slaveAddr, unsigned char regAddr,
                 unsigned char length, unsigned char * data,
                 const char * errorMsg)
{
   unsigned char rxBuff[MAX_READ_LEN + 1];
   struct spi_ioc_transfer rdIOC;

   rxBuff[0] = regAddr | 0x80;
   memcpy (rxBuff + 1, data, length);
   memset (&rdIOC, 0, sizeof (rdIOC));
   rdIOC.tx_buf = (unsigned long) rxBuff;
   rdIOC.rx_buf = (unsigned long) rxBuff;
   rdIOC.len = length + 1;

   if (ioctl (m_SPI, SPI_IOC_MESSAGE (1), &rdIOC) < 0)
   {
      if (strlen (errorMsg) > 0)
         HAL_ERROR2 ("SPI read error from %d - %s\n", regAddr, errorMsg);

      return false;
   }

   memcpy (data, rxBuff + 1, length);

   return true;
}

bool
SpiDevice::Read (unsigned char slaveAddr, unsigned char length,
                 unsigned char * data, const char * errorMsg)
{
   unsigned char rxBuff[MAX_READ_LEN + 1];
   struct spi_ioc_transfer rdIOC;

   memset (&rdIOC, 0, sizeof (rdIOC));
   rdIOC.tx_buf = 0;
   rdIOC.rx_buf = (unsigned long) rxBuff;
   rdIOC.len = length;

   if (ioctl (m_SPI, SPI_IOC_MESSAGE (1), &rdIOC) < 0)
   {
      if (strlen (errorMsg) > 0)
         HAL_ERROR1 ("SPI read error from - %s\n", errorMsg);

      return false;
   }

   memcpy (data, rxBuff, length);

   return true;
}



void
SpiDevice::delayMs (int milliSeconds)
{
   usleep (1000 * milliSeconds);
}


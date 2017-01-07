////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTIMULibCS
//
//  Copyright (c) 2015, richards-tech, LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of
//  this software and associated documentation files (the "Software"), to deal in
//  the Software without restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
//  Software, and to permit persons to whom the Software is furnished to do so,
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using NetBridge.Linux;

namespace PiSHat.Sensors.Devices.HTS221
{
/// <summary>
/// The HTS221 humidity-sensor.
/// </summary>
public class HTS221HumiditySensor : HumiditySensor
{
    private readonly byte _i2CAddress;

    private I2cDevice _i2CDevice;

    private Func<Int16, double> _temperatureConversionFunc;
    private Func<Int16, double> _humidityConversionFunc;

    private bool _humidityValid = false;
    private double _humidity = 0;
    private bool _temperatureValid = false;
    private double _temperature = 0;

    public HTS221HumiditySensor(byte i2CAddress)
    {
        _i2CAddress = i2CAddress;
        Name = "HTS221HumiditySensor";
    }

    public override void Dispose()
    {
        base.Dispose();
        _i2CDevice.Dispose();
    }

    protected override bool InitDeviceAsync()
    {
        ConnectToI2CDevices();

        I2CSupport.Write(_i2CDevice, HTS221Defines.CTRL1, 0x87, "Failed to set HTS221 CTRL_REG_1");

        I2CSupport.Write(_i2CDevice, HTS221Defines.AV_CONF, 0x1b, "Failed to set HTS221 AV_CONF");

        _temperatureConversionFunc = GetTemperatureConversionFunc();

        _humidityConversionFunc = GetHumidityConversionFunc();

        return true;
    }

    private Func<Int16, double> GetTemperatureConversionFunc()
    {
        byte tempRawMsb = I2CSupport.Read8Bits(_i2CDevice, HTS221Defines.T1_T0 + 0x80, "Failed to read HTS221 T1_T0");

        byte temp0Lsb = I2CSupport.Read8Bits(_i2CDevice, HTS221Defines.T0_C_8 + 0x80, "Failed to read HTS221 T0_C_8");

        UInt16 T0_C_8 = (UInt16)((((UInt16)tempRawMsb & 0x03) << 8) | (UInt16)temp0Lsb);
        double T0 = T0_C_8 / 8.0;

        byte temp1Lsb = I2CSupport.Read8Bits(_i2CDevice, HTS221Defines.T1_C_8 + 0x80, "Failed to read HTS221 T1_C_8");

        UInt16 T1_C_8 = (UInt16)(((UInt16)(tempRawMsb & 0x0C) << 6) | (UInt16)temp1Lsb);
        double T1 = T1_C_8 / 8.0;

        Int16 T0_OUT = (Int16)I2CSupport.Read16Bits(_i2CDevice, HTS221Defines.T0_OUT + 0x80, ByteOrder.LittleEndian, "Failed to read HTS221 T0_OUT");

        Int16 T1_OUT = (Int16)I2CSupport.Read16Bits(_i2CDevice, HTS221Defines.T1_OUT + 0x80, ByteOrder.LittleEndian, "Failed to read HTS221 T1_OUT");

        // Temperature calibration slope
        double m = (T1 - T0) / (T1_OUT - T0_OUT);

        // Temperature calibration y intercept
        double b = T0 - (m * T0_OUT);

        return rawTemperature => rawTemperature * m + b;
    }

    private Func<Int16, double> GetHumidityConversionFunc()
    {
        byte H0_H_2 = I2CSupport.Read8Bits(_i2CDevice, HTS221Defines.H0_H_2 + 0x80, "Failed to read HTS221 H0_H_2");
        double H0 = H0_H_2 / 2.0;

        byte H1_H_2 = I2CSupport.Read8Bits(_i2CDevice, HTS221Defines.H1_H_2 + 0x80, "Failed to read HTS221 H1_H_2");
        double H1 = H1_H_2 / 2.0;

        Int16 H0_T0_OUT = (Int16)I2CSupport.Read16Bits(_i2CDevice, HTS221Defines.H0_T0_OUT + 0x80, ByteOrder.LittleEndian, "Failed to read HTS221 H0_T_OUT");

        Int16 H1_T0_OUT = (Int16)I2CSupport.Read16Bits(_i2CDevice, HTS221Defines.H1_T0_OUT + 0x80, ByteOrder.LittleEndian, "Failed to read HTS221 H1_T_OUT");

        // Humidity calibration slope
        double m = (H1 - H0) / (H1_T0_OUT - H0_T0_OUT);

        // Humidity calibration y intercept
        double b = H0 - (m * H0_T0_OUT);

        return rawHumidity => rawHumidity * m + b;
    }

    private void ConnectToI2CDevices()
    {
        try
        {
            byte[] addresses = new byte[1];
            addresses[0] = _i2CAddress;

            Console.WriteLine("Address {0}", addresses[0]);

            _i2CDevice = new I2cDevice(1, addresses);
            _i2CDevice.Connect();
        }
        catch (Exception exception)
        {
            throw new SensorException("Failed to connect to HTS221", exception);
        }
    }

    /// <summary>
    /// Tries to update the readings.
    /// Returns true if new readings are available, otherwise false.
    /// An exception is thrown if something goes wrong.
    /// </summary>
    public override bool Update()
    {
        bool newReadings = false;

        var readings = new SensorReadings
        {
            Timestamp = DateTime.Now
        };

        byte status = I2CSupport.Read8Bits(_i2CDevice, HTS221Defines.STATUS, "Failed to read HTS221 status");

        if ((status & 0x02) == 0x02)
        {
            Int16 rawHumidity = (Int16)I2CSupport.Read16Bits(_i2CDevice, HTS221Defines.HUMIDITY_OUT_L + 0x80, ByteOrder.LittleEndian, "Failed to read HTS221 humidity");
            _humidity = _humidityConversionFunc(rawHumidity);
            _humidityValid = true;
            newReadings = true;

            Console.WriteLine("{0} : HUMI {1}", Name, _humidity);
        }

        if ((status & 0x01) == 0x01)
        {
            Int16 rawTemperature = (Int16)I2CSupport.Read16Bits(_i2CDevice, HTS221Defines.TEMP_OUT_L + 0x80, ByteOrder.LittleEndian, "Failed to read HTS221 temperature");
            _temperature = _temperatureConversionFunc(rawTemperature);
            _temperatureValid = true;
            newReadings = true;

            Console.WriteLine("{0} : TEMP {1}", Name, _temperature);
        }

        if (newReadings)
        {
            readings.Humidity = _humidity;
            readings.HumidityValid = _humidityValid;
            readings.Temperature = _temperature;
            readings.TemperatureValid = _temperatureValid;
            AssignNewReadings(readings);
            return true;
        }

        return false;
    }
}
}

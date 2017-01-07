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
using System.Threading;
using System.Threading.Tasks;
using PiSHat.Sensors;
using PiSHat.Sensors.Devices.HTS221;
using PiSHat.Sensors.Devices.LPS25H;
using PiSHat.Sensors.Devices.LSM9DS1;

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

using PiSHat.Sensors;

namespace NetBridge.Linux
{

    public class SensorThread : IDisposable
    {
        private readonly object _syncObj = new object();
        private readonly ManualResetEvent _loopStoppedEvent = new ManualResetEvent(false);
        private readonly LSM9DS1ImuSensor _imuSensor;
        private readonly HTS221HumiditySensor _humiditySensor;
        private readonly LPS25HPressureSensor _pressureSensor;
        private int _imuSampleCount;
        private int _pressureSampleCount;
        private int _humiditySampleCount;
        private int _imuSampleRate;
        private int _pressureSampleRate;
        private int _humiditySampleRate;
        private string _imuErrorMessage;
        private string _pressureErrorMessage;
        private string _humidityErrorMessage;
        private bool _stopLoop;

        public SensorThread()
        {
            _imuSensor = null;
            _humiditySensor = null;
            _pressureSensor = null;

            //var lsm9Ds1Config = new LSM9DS1Config();

            //_imuSensor = new LSM9DS1ImuSensor(
            //    LSM9DS1Defines.ADDRESS0,
            //    LSM9DS1Defines.MAG_ADDRESS0,
            //    lsm9Ds1Config,
            //    new SensorFusionRTQF());

            _humiditySensor = new HTS221HumiditySensor(HTS221Defines.ADDRESS);

            _pressureSensor = new LPS25HPressureSensor(LPS25HDefines.ADDRESS0);

            Start();
        }

        public void Dispose()
        {
            _stopLoop = true;
            _loopStoppedEvent.WaitOne();

            _imuSensor.Dispose();
            _humiditySensor.Dispose();
            _pressureSensor.Dispose();
        }

        private void Start()
        {
            InitDevices();
            MainLoop();
        }

        private void InitDevices()
        {
            _imuErrorMessage = InitSensorAsync(_imuSensor);
            _pressureErrorMessage = InitSensorAsync(_pressureSensor);
            _humidityErrorMessage = InitSensorAsync(_humiditySensor);
        }

        private static string InitSensorAsync(Sensor sensor)
        {
            try
            {
                if (sensor != null)
                {
                    Console.WriteLine("InitSensorAsync on {0}", sensor.Name);
                    sensor.InitAsync();
                }

                return string.Empty;
            }
            catch (Exception exception)
            {
                return exception.Message;
            }
        }

        private void MainLoop()
        {
            DateTime startTime = DateTime.Now;
            DateTime oneSecondAfterStartTime = startTime.AddSeconds(1);

            while (!_stopLoop)
            {
                Thread.Sleep(1000);

                SensorReadings readings;
                string errorMessage = String.Empty;

                if (_imuSensor != null && TryReadSensor(_imuSensor, out readings, out errorMessage))
                {
                    lock (_syncObj)
                    {
                        _imuSampleCount++;
                    }
                }

                _imuErrorMessage = errorMessage;

                if (_pressureSensor != null && TryReadSensor(_pressureSensor, out readings, out errorMessage))
                {
                    lock (_syncObj)
                    {
                        _pressureSampleCount++;
                    }
                }

                _pressureErrorMessage = errorMessage;

                if (_humiditySensor != null && TryReadSensor(_humiditySensor, out readings, out errorMessage))
                {
                    lock (_syncObj)
                    {
                        _humiditySampleCount++;
                    }
                }

                _humidityErrorMessage = errorMessage;

                if (DateTime.Now >= oneSecondAfterStartTime)
                {
                    lock (_syncObj)
                    {
                        _imuSampleRate = _imuSampleCount;
                        _pressureSampleRate = _pressureSampleCount;
                        _humiditySampleRate = _humiditySampleCount;

                        _imuSampleCount = 0;
                        _pressureSampleCount = 0;
                        _humiditySampleCount = 0;

                        startTime = DateTime.Now;
                        oneSecondAfterStartTime = startTime.AddSeconds(1);
                    }
                }
            }

            _loopStoppedEvent.Set();
        }

        private static bool TryReadSensor(
            Sensor sensor,
            out SensorReadings readings,
            out string errorMessage)
        {
            errorMessage = string.Empty;


            if (sensor.Initiated)
            {
                //Console.WriteLine("TryReadSensor on {0}", sensor.Name);

                try
                {
                    if (sensor.Update())
                    {
                        readings = sensor.Readings;
                        return true;
                    }
                }
                catch (Exception exception)
                {
                    errorMessage = exception.Message;
                }
            }

            readings = new SensorReadings();
            return false;
        }


        public ImuSensorData GetImuSensorData()
        {
            lock (_syncObj)
            {
                return new ImuSensorData
                {
                    Initiated = _imuSensor.Initiated,
                    ErrorMessage = _imuErrorMessage ?? string.Empty,
                    Readings = _imuSensor.Readings,
                    SampleRate = _imuSampleRate,
                    GyroBiasValid = _imuSensor.GyroBiasValid,
                    MagCalValid = _imuSensor.MagCalValid
                };
            }
        }

        public SensorData GetPressureSensorData()
        {
            lock (_syncObj)
            {
                return new SensorData
                {
                    Initiated = _pressureSensor.Initiated,
                    ErrorMessage = _pressureErrorMessage ?? string.Empty,
                    Readings = _pressureSensor.Readings,
                    SampleRate = _pressureSampleRate,
                };
            }
        }

        public SensorData GetHumiditySensorData()
        {
            lock (_syncObj)
            {
                return new SensorData
                {
                    Initiated = _humiditySensor.Initiated,
                    ErrorMessage = _humidityErrorMessage ?? string.Empty,
                    Readings = _humiditySensor.Readings,
                    SampleRate = _humiditySampleRate,
                };
            }
        }
    }
}

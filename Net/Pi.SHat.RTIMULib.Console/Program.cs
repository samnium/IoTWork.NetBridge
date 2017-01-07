using System;
using System.Runtime.InteropServices;

namespace NetBridge.Linux
{
    public class Program
    {
        public static void Main(string[] args)
        {

            try
            {
                SensorThread sensorThread = new SensorThread();

                while (true)
                {
                    System.Console.WriteLine("Menu");
                    System.Console.WriteLine("");
                    System.Console.WriteLine("1. Read Temperature");
                    System.Console.WriteLine("2. Read Humidity");
                    System.Console.WriteLine("3. Read Pressure");
                    System.Console.WriteLine("4. Dump errors");
                    System.Console.WriteLine("");
                    System.Console.WriteLine("(0 exit) >");

                    var choice = System.Console.ReadLine();
                    var choicedValue = Int32.Parse(choice);

                    if (choicedValue == 0)
                        break;

                    switch (choicedValue)
                    {
                        case 1:
                            {
                                var sd = sensorThread.GetHumiditySensorData();
                                if (sd.Initiated)
                                {
                                    var readings = sd.Readings;
                                    if (readings.TemperatureValid)
                                    {
                                        System.Console.WriteLine("Temperature is {0}", readings.Temperature);
                                    }
                                    else
                                    {
                                        System.Console.WriteLine("Temperature is not valid");
                                    }
                                }
                                else
                                {
                                    System.Console.WriteLine("Sensor not Initiated");
                                }
                            }
                            break;
                        case 2:
                            {
                                var sd = sensorThread.GetHumiditySensorData();
                                if (sd.Initiated)
                                {
                                    var readings = sd.Readings;
                                    if (readings.HumidityValid)
                                    {
                                        System.Console.WriteLine("Humidity is {0}", readings.Humidity);
                                    }
                                    else
                                    {
                                        System.Console.WriteLine("Humidity is not valid");
                                    }
                                }
                                else
                                {
                                    System.Console.WriteLine("Sensor not Initiated");
                                }
                            }
                            break;
                        case 3:
                            {
                                var sd = sensorThread.GetPressureSensorData();
                                if (sd.Initiated)
                                {
                                    var readings = sd.Readings;
                                    if (readings.PressureValid)
                                    {
                                        System.Console.WriteLine("Pressure is {0}", readings.Pressure);
                                    }
                                    else
                                    {
                                        System.Console.WriteLine("Pressure is not valid");
                                    }
                                }
                                else
                                {
                                    System.Console.WriteLine("Sensor not Initiated");
                                }
                            }
                            break;
                        case 4:
                            {
                                var sd = sensorThread.GetHumiditySensorData();
                                if (sd.Initiated)
                                {
                                    System.Console.WriteLine("Humidity Errors: {0}", sd.ErrorMessage);
                                }
                                else
                                {
                                    System.Console.WriteLine("Humidity Sensor not Initiated");
                                }

                                sd = sensorThread.GetPressureSensorData();
                                if (sd.Initiated)
                                {
                                    System.Console.WriteLine("Pressure Errors: {0}", sd.ErrorMessage);
                                }
                                else
                                {
                                    System.Console.WriteLine("Pressure Sensor not Initiated");
                                }
                            }
                            break;
                    }
                }
            }
            catch(Exception ex)
            {
                System.Console.WriteLine(ex.Message);
            }
        }
    }
}

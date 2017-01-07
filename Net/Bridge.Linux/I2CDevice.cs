using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace NetBridge.Linux
{
    // https://www.codeproject.com/articles/18032/how-to-marshal-a-c-class

    public class I2cDevice : IDisposable
    {
        #region PInvokes

        [DllImport("libnetbridge.so")]
        private static extern IntPtr __i2c_Create();

        [DllImport("libnetbridge.so")]
        private static extern void __i2c_Dispose(IntPtr device);

        [DllImport("libnetbridge.so")]
        private static extern void __i2c_SetDeviceId(IntPtr device, int Id);

        [DllImport("libnetbridge.so")]
        private static extern bool __i2c_Open(IntPtr device);

        [DllImport("libnetbridge.so")]
        private static extern void __i2c_Close(IntPtr device);

        [DllImport("libnetbridge.so")]
        private static extern bool __i2c_Read_AtRegister(IntPtr device,
                byte slaveAddr,
                byte regAddr,
                byte length,
                byte[] data,
                string errorMsg);

        [DllImport("libnetbridge.so")]
        private static extern bool __i2c_Read(IntPtr device,
                                               byte slaveAddr,
                                               byte length,
                                               byte data,
                                               string errorMsg);

        [DllImport("libnetbridge.so")]
        private static extern bool __i2c_Write(IntPtr device,
                                                byte slaveAddr,
                                                byte regAddr,
                                                byte length,
                                                byte[] data,
                                                string errorMsg);

        [DllImport("libnetbridge.so")]
        private static extern bool __i2c_Write_Byte(IntPtr device,
                byte slaveAddr,
                byte regAddr,
                byte[] data,
                string errorMsg);

        #endregion

        private IntPtr _i2cDevice;
        private int _id;
        private bool _opened;
        private byte[] _addresses;

        public int Id
        {
            get
            {
                return _id;
            }
        }

        public I2cDevice(int Id, byte[] Addresses)
        {
            try
            {
                Console.WriteLine("I2cDevice Addresses {0}:{1}", Id, Addresses );
                
                _addresses = Addresses;

                if (_addresses == null || _addresses.Length == 0)
                    throw new ArgumentException("Addresses are not supplied or are voids");

                _opened = false;
                _i2cDevice = __i2c_Create();
                _id = Id;

                Console.WriteLine("I2cDevice Done!");
            }
            catch (System.Exception ex)
            {
                System.Console.WriteLine("Exception {0}", ex.Message);
            }
        }

        public void Dispose()
        {
            Dispose(true);
        }

        protected virtual void Dispose(bool Disposing)
        {
            if (_i2cDevice != IntPtr.Zero)
            {
                // Call the DLL Export to dispose this class
                __i2c_Dispose(_i2cDevice);
                _i2cDevice = IntPtr.Zero;
            }

            if (Disposing)
            {
                // No need to call the finalizer since we've now cleaned
                // up the unmanaged memory
                GC.SuppressFinalize(this);
            }
        }

        public void Connect()
        {
            try
            {
                Console.WriteLine("Connect __i2c_SetDeviceId");
                __i2c_SetDeviceId(_i2cDevice, _id);

                Console.WriteLine("Connect __i2c_Open");
                _opened = __i2c_Open(_i2cDevice);

                Console.WriteLine("Connect Done");
            }
            catch(Exception ex)
            {
                _opened = false;
            }
        }

        public static System.String GetDeviceSelector()
        {
            return String.Empty;
        }

        public static System.String GetDeviceSelector(System.String friendlyName)
        {
            return String.Empty;
        }

        public void Write(System.Byte[] buffer)
        {
            try
            {
                if (_i2cDevice != IntPtr.Zero)
                {

                    byte reg = buffer[0];
                    byte[] data = new byte[buffer.Length - 1];
                    Array.Copy(buffer, 1, data, 0, buffer.Length - 1);

                    Console.WriteLine("I2CDevice Write on addr:{0} reg:{1} length:{2} data0:{3}", _addresses[0], reg, data.Length, data[0]);

                    __i2c_Write(_i2cDevice, _addresses[0], reg, (byte)data.Length, data, "writing");
                }
            }
            catch (Exception ex)
            {

            }
        }

        public void WriteRead(System.Byte[] writeBuffer, System.Byte[] readBuffer)
        {
            try
            {
                if (_i2cDevice != IntPtr.Zero)
                {
                    byte reg = writeBuffer[0];

                    __i2c_Read_AtRegister(_i2cDevice, _addresses[0], reg, (byte)readBuffer.Length, readBuffer, "writing");
                }
            }
            catch (Exception ex)
            {

            }
        }
    }

}
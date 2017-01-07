using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace Bridge.Linux
{
    public class FrameBufferOfRaspberryPiSenseHat : IFrameBuffer
    {
        #region PInvokes

        [DllImport("libnetbridge.so")]
        private static extern IntPtr __FrameBuffer_PiSenseHat_Create();

        [DllImport("libnetbridge.so")]
        private static extern void __FrameBuffer_PiSenseHat_Dispose(IntPtr FrameBuffer);

        [DllImport("libnetbridge.so")]
        private static extern void __FrameBuffer_PiSenseHat_Open(IntPtr FrameBuffer);

        [DllImport("libnetbridge.so")]
        private static extern void __FrameBuffer_PiSenseHat_Close(IntPtr FrameBuffer);

        [DllImport("libnetbridge.so")]
        private static extern void __FrameBuffer_PiSenseHat_Draw(IntPtr FrameBuffer, Char c);

        #endregion

        private static readonly Lazy<FrameBufferOfRaspberryPiSenseHat> lazy =
            new Lazy<FrameBufferOfRaspberryPiSenseHat>(() => new FrameBufferOfRaspberryPiSenseHat());

        public static FrameBufferOfRaspberryPiSenseHat Instance { get { return lazy.Value; } }

        private IntPtr _FrameBuffer;
        private object _locker;

        private FrameBufferOfRaspberryPiSenseHat()
        {
            try
            {
                _locker = new object();

                Console.WriteLine("FrameBufferOfRaspberryPiSenseHat creating ...");

                _FrameBuffer = __FrameBuffer_PiSenseHat_Create();
                
                Console.WriteLine("FrameBufferOfRaspberryPiSenseHat Done!");
            }
            catch (System.Exception ex)
            {
                System.Console.WriteLine("Exception {0}", ex.Message);
            }
        }


        public void Close()
        {
            try
            {
                lock (_locker)
                {
                    __FrameBuffer_PiSenseHat_Close(_FrameBuffer);
                }
            }
            catch (System.Exception ex)
            {
                System.Console.WriteLine("Exception {0}", ex.Message);
            }
        }

        public void Draw(char c)
        {
            try
            {
                lock (_locker)
                {
                    __FrameBuffer_PiSenseHat_Draw(_FrameBuffer, c);
                }
            }
            catch (System.Exception ex)
            {
                System.Console.WriteLine("Exception {0}", ex.Message);
            }
        }

        public void Open()
        {
            try
            {
                lock (_locker)
                {
                    __FrameBuffer_PiSenseHat_Open(_FrameBuffer);
                }
            }
            catch (System.Exception ex)
            {
                System.Console.WriteLine("Exception {0}", ex.Message);
            }
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Bridge.Linux
{
    public interface IFrameBuffer
    {
        void Open();

        void Close();

        void Draw(Char c); 
    }
}

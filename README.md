IoTWork.NetBridge is software component to use for "binding" Linux capabiities inside your .Net Application.
This version is most focused on I2C of the Raspberry Pi 3 but you can add more facilities.

IoTWork.NetBridge is composed by two mains modules:

* **IoTWork.Bridge**: it's a C++ library that implement the real Linux features.It'is built as libnetbridge.so and installed under /usr/local/folder.
* **IoTWork.Net**: it's the bridging dll that let you to access to IoTWork.Bridge from a .Net Applicaion

For example in the case of the I2C metioned above you have this situation:

* IoTWork.Bridge: implement the C++ class I2CDevice that access to the device /dec/i2c0 and let you to operate on it using Read and Write and Ioctl functions
* IoTWork.Net: implement the C# class I2cDevice that act as wrapper of the corresponding C++

The bridging feature is implement using the [PInvoke Pattern](http://www.mono-project.com/docs/advanced/pinvoke/).

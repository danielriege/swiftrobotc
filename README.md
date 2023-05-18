## Why swiftrobotc?
Please take a look at [swiftrobot](https://github.com/danielriege/swiftrobot) first. swiftrobotc is an implementation of that in C++. It enables the possibility to communicate with non Apple devices, either over Wi-Fi or USB.

It does not cover all features of [swiftrobot](https://github.com/danielriege/swiftrobot), but the most important ones for seamless integration like:
 - Bonjour service discovery, so clients connect to each other without any manual configuration needed.
 - Publish/Subscribe messaging system based on shared memory or TCP sockets.
 - USB communication for iOS devices

## USB with iOS?
The current version of swiftrobotc uses [usbmux](https://www.theiphonewiki.com/wiki/Usbmux), which comes preinstalled on every Mac. It is used by iTunes etc. to communicate with your devices. After a proper connection request, all traffic to this UNIX socket is tunneled through USB to an iOS device. This ability is exploited by swiftrobotc.

Using an open source implementation of [usbmuxd](https://github.com/libimobiledevice/usbmuxd) enables swiftrobotc to work on Linux systems and therefore on most embedded devices.
If not using a Mac, please check out their [installation guide](https://github.com/libimobiledevice/usbmuxd#installation--getting-started) to use swiftrobotc on Linux. 

Future versions will be using libusb to elliminate the usbmuxd dependency and enable microcontrollers to communicate with iOS devices via USB as well. 
## Installation
If you want to use USB capability, make sure [usbmuxd](https://github.com/libimobiledevice/usbmuxd) is running on your system if not using a Mac. Otherwise usbmuxd is not needed.

Further instructions come as soon as the first version is ready. 

## License
This package is licensed under the MIT license. See the LICENSE file for more information.

#!/bin/bash

#adb shell
#logcat-enable
#reboot

#also side not i had to enter *#*#2846579#*#*  Go to "ProjectMenu" -> "Background Setting" -> "Log Setting"
#Open "Log switch" and set it to ON.   on an android 2.3

#adb -d logcat -c;adb logcat Irrlicht:V DEBUG:V *:S


adb logcat


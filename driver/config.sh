#!/bin/sh

groupadd plugdev

usermod -aG plugdev $USER

echo 'SUBSYSTEMS=="usb", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", GROUP="plugdev", MODE="0666"
' > /etc/udev/rules.d/40-dfuse.rules

udevadm control --reload-rules && udevadm trigger

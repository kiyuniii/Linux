#!/bin/bash
sudo insmod gpioirq_module.ko
dmesg | tail -3
sudo mknod /dev/gpioled c 200 0
sudo chmod 666 /dev/

# orcon-usb-dongle
Use ESP32 or Arduino board having 2 UARTs. Please be aware of IO voltage levels between CC1101 (module) and Arduino motherboard.

![usb dongle](https://tweakers.net/i/g_-GBxCCM6Ay2VLwS0zH_Zez9-4=/full-fit-in/4920x3264/filters:max_bytes(3145728):no_upscale():strip_icc():fill(white):strip_exif()/f/image/FWoJ8TzERY2RuCxRh4murepU.jpg?f=user_large?raw=true)

The repository contains two folders:
- standalone
- Firmata

# Standalone version
The "standalone" is the most basic version to communicate with the Orcon unit, after powering the USB dongle, it will stay in pairing mode for 5sec, when you press a button on the RF15 remote control, the addresses will be copied. If no button pressed within 5sec, it will use previous stored address in ram. The addresses will be stored in ram so that it is not forgotten during power off.

# Firmata version
The "Firmata" version is able to communicate with home automation systems like "Home Assistant". You have to configure it like this:
```
- serial_port: /dev/serial/by-id/usb-Arduino_LLC_ORCON_RF_Dongle-if00
  serial_baud_rate: 57600
  lights:
    - name: fan speed
      pin_mode: PWM
      pin: 3
      minimum: 0
      maximum: 99
  sensors:
    - name: fan speed
      pin_mode: ANALOG
      pin: A0
      differential: 1
```

Also the Firmata version will support pairing for the frst 2,5sec. Firmata for Home Assistant only allow the Arduino "Lights" to receive a analog value. Normally lights in Home Assistant are being controlled by sending percentage based on a range from 0 till 255, therefore setting the min/max to 0-99, will result in percentage equals decimal number, what is more convenient in automations.

You can add custom card to the Lovelace front panel like this:

![custom card](https://tweakers.net/i/92_YFrTlCgnYt5MYhOnoPeuxj60=/full-fit-in/4000x4000/filters:no_upscale():fill(white):strip_exif()/f/image/rBX8e7hh46UIrR00872p1JKJ.png?f=user_large)

The schematic can be found here: [Click!](https://github.com/Marcelh1/orcon-usb-dongle/blob/main/Schematic%20USB%20Dongle.pdf)


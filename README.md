# Firmware for the RUNS platform

[GUI client repository](https://github.com/Jah-On/RUNS_CLIENT).

## Abstract

Remote User Navigated System, or RUNS for short, serves as an educational project teaching the foundations of embedded systems development, IoT platform design, and API integration in user applications. 

This repository houses the firmware for the platform. This firmware is specifically made for the Ti MSP‑EXP432P401R development kit. __There are no plans to port this project to other devices__.

## Bluetooth Interface

The scope of this project is largely determined by the Bluetooth API. __There are no plans to expand the API beyond the API listed below__ but forking is always welcome and encouraged.

### RUNS Service Specifications

These services will consist of characteristics intended to handle specific components of the "robot". These services include temperature relay, front bumper trigger relay, and movement.

#### Temperature - `20240233-1360-1234-0001-000000000000`

Service for reading temperature values from the robot.

##### MCU - `20240233-1360-1234-0001-000000000001` - READ ONLY

Characteristic for reading MCU temperature.

Signed eight bit value as nearest degree centigrade.

##### Ambient - `20240233-1360-1234-0001-000000000010` - READ ONLY

Characteristic for reading ambient temperature.

Signed eight bit value as nearest degree centigrade.

#### Object Proximity - `20240233-1360-1234-0002-000000000000`

Service for reading object proximity from the robot. Currently only bump switches but could be expanded for IR line tracking or ultrasonic range approximation.

##### Bump switches - `20240233-1360-1234-0002-000000000001` - READ ONLY

Characteristic for reading the six frontal bump switches.

Eight bit encoded value. Bits seven and six are always zero; bit zero is right most bump switch in back to front perspective.

#### Movement - `20240233-1360-1234-0003-000000000000`

Service for controlling robot movement. Currently only motor movements but could be expanded for servo movement.

##### Velocity - `20240233-1360-1234-0003-000000000001` - WRITE ONLY

Characteristic for controlling the relative velocity.

Scale is 100 to -100 as an 8 bit value where 100 is 100% forward velocity and -100 is 100% backward velocity.

##### Rotation - `20240233-1360-1234-0003-000000000010` - WRITE ONLY

Characteristic for turning the robot.

Represented as an eight bit value where 0x20 is right, 0x10 is left, and 0x00 no turn.

## Progress

|       Task             |           Description           |
|------------------------|---------------------------------|
|    API Specification   |          Done.                  |
|    BT Implementation   |          Done.                  |
|      Motor Control     |          Done.                  |
|    Bumper Interrupts   |          Done.                  |
|   Temperature Sensing  |          Done.                  |

## Project reflection

Overall, this project was not too bad. I had the help of previous labs to look back on in case I got stuck. However, I elected not copying the files from previous labs besides the class given ones. The rational was that I could optimize the functionality specific to this project instead of trying to build on top of previous code. Additionally, this allowed me to use structures which is not something we weren't mandated to do for the labs (I could have used them for lab but they would provide little value since scope of the labs were relatively small). 

The biggest challenge I faced with the project was reading from the MCU's built in temperature sensor. Initially, I thought it was as simple as setting the ACT14TCMAP flag in the ADC14CTL1 register... it wasn't. There is another register, REF_A, which we did (and will) not cover in class which is responsible for enabling the temperature sensor and setting the reference voltage for it. Great, once I have that set, it'll surely solve ALL my problems... it didn't. I used TI's calibration values and conversion formula from the SLAU356i data sheet (you're welcome). -65 degrees Celsius... I mean, it is Rochester but it's not THAT cold? Maybe it was the sampling, inverse, unicorns...? I tried just about everything. By this point, I thought *surely this is TI's fault, surely*. I did what any sane person would do and calculated the inverse function of the degree Celsius to ADC function and got... -69 ~~(nice)~~ degrees Celsius. Well, at least I know I'm the dumb ass and not TI. Turns out, I needed to set VRSEL to 0x01 to use REF_A's voltage. Once set to that, I got values that made sense. *Sighhhhhhhhh*.

Now for client app things that took a while to work, yay. Overall, the dearimgui + SimpleBLE combination worked pretty well. The robot class responsible for Bluetooth communication runs in it's own thread and the GUI simply requests the most recent data or requests speed/rotation changes. The robot has an Bluetooth execution queue which allows the whole system to run smoothly. The thing that did not go smoothly was writing to the movement characteristics. I knew this worked on the firmware side since it worked fine via nRF Connect. Turns out that one can't use the `std:::to_string` method because that converts the integer value to the ASCII character representing that integer. I made a null terminated char buffer and made a string each time using that and this works perfectly. I was also using the standard SimpleBLE API which caused the app to crash whenever the robot disconnected. I switched it to the Safe API and now the app behaves correctly without crashing. The only thing "missing" from the client app is that the triangles are not buttons but that can be added if someone opens an issue for it. 

Despite these mentally and emotionally taxing issues, this project was a TON OF FUN. This is the first full stack IoT project that I've done and I love having the power to integrate and orchestrate each individual component into one coherent system. I can also confidently say "I'm done" with this project which is rare outside of relatively basic projects. 

## Special thanks to...

- Daniel Valvano and Jonathan Valvano for writing a lot (if not all) of the helper code that my course used to aid our labs!
- Texas Instruments for writing good and complete documentation! 
- The lecturers, lab professors, and TAs who helped with issues and provided words of encouragement/perseverance!

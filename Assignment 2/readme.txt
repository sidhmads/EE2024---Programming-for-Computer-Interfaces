#Assignement 2
=====================
##This project contains the code for 

###Devices Used:

-ACCELEROMETER:  MMA7455L x-axis reading in 'g's (where 1g = 9.8 m/s2)
-LIGHT_SENSOR: ISL29003 with LIGHT_RANGE_4000
-TEMPERATURE_SENSOR: MAX6576
-LED_ARRAY: PCA_9532
-SEGMENT_DISPLAY: 7-segment LED display
-GRAPHICS_DISPLAY: 96x64 White OLED
-BLINK_BLUE: Blue Light for RGB LED, alternating between ON and OFF every 333 milliseconds
-BLINK_RED: Red Light for RGB LED, alternating between ON and OFF every 333 milliseconds
-MODE_TOGGLE: SW3, read using interrupts.
-CLEAR_WARNING: SW4.
-NUSCloud: UART terminal program (Tera Term) on a personal computer

###Features

####Modes of Operation
```
When powered on, system is in STATIONARY Mode.
Pressing MODE_TOGGLE causes a count-down sequence to be initiated, and then the system enters the LAUNCH mode at the end of count-down, provided some conditions are met.
When in LAUNCH mode, if MODE_TOGGLE is pressed twice within 1 second, the system switches to RETURN mode, simulating a scenario of the space shuttle separating from the rocket and returning to the earth for landing, like an aircraft.
Pressing MODE_TOGGLE once in RETURN mode causes the system to return to STATIONARY Mode within 1 second, simulating a scenario of the space shuttle being mounted on the rocket for further launches after landing.
```
####Rocket Orientation Detection
```
Rocket orientation detection is essential to detect the rocket orientation / course and trigger corrective actions.
We use the ACCELEROMETER to detect rocket orientation. When ACCELEROMETER X or Y reading is greater than ACC_THRESHOLD, the system goes into a ACC_WARNING state. You can ignore the ACCELEROMETER Z reading (we are assuming that the desired LAUNCH profile is straight up vertically). The system does the following in ACC_WARNING state.
Show a message “Veer off course” on the GRAPHICS_DISPLAY.
Set the BLUE LED blinking as given in BLINK_BLUE.
The system remains in a state of ACC_WARNING even if the ACCELEROMETER X and Y readings fall below the ACC_THRESHOLD until CLEAR_WARNING is pressed or until the system changes to RETURN mode, whichever happens earlier.
Orientation Detection is done only in the LAUNCH mode. 
```
####Fuel Tank Monitoring
```
Rocket fuel is stored in a tank, it burns when combined with the oxidizer in combustion chamber, so that the rocket is propelled to the space. 
We need good design as well as safety/precautionary measures to prevent the fuel tank from overheating, which could lead to fires.
We use the TEMPERATURE_SENSOR to monitor the temperature of fuel tank in the rocket.
If temperature increases beyond TEMP_HIGH_THRESHOLD, the system goes into a TEMP_WARNING state. The system does the following in TEMP_WARNING state.
Show message “Temp. too high” warning on GRAPHICS_DISPLAY.
Set the RED LED blinking as given in BLINK_RED.
Count down sequence in progress is aborted.
The system remains in a state of TEMP_WARNING even if the TEMPERATURE_SENSOR reading falls below the TEMP_HIGH_THRESHOLD until CLEAR_WARNING is pressed or until the system changes to RETURN mode, whichever happens earlier.
Fuel tank monitoring is done in both STATIONARY and LAUNCH modes.
```
####Obstacle Avoidance
```
Even though the LIGHT_SENSOR measures ambient light intensity in reality, in this assignment, it is assumed to simulate a radar echo measurements, which can be used to measure distance to obstacles.
When an obstacle is too near, i.e., when the light intensity increases* beyond OBSTACLE_NEAR_THRESHOLD, the system goes into a state of OBST_WARNING. The system does the following in OBST_WARNING state.
Display “Obstacle near” message on GRAPHICS_DISPLAY.
Read the light intensity reading from LIGHT_SENSOR and turn on the 16 LEDs in LED_ARRAY proportionately.
For instance, when the reading is very high, turn ON all the LEDs. Turn OFF more LEDs as the reading decreases.
The number of LEDs turned on in the LED array gives us an indication of how close the system is to nearby objects.
Obstacle Avoidance is done only in the RETURN mode.
The system remains in a state of OBST_WARNING until the obstacle is farther than OBSTACLE_NEAR_THRESHOLD (we assume that the pilot will man the space shuttle away from the obstacle) or until the system changes to STATIONARY mode, whichever happens earlier.
```
####Telemetry
```
Various messages and data are sent to a server (NUSCloud) connected via the XBee modules, for trajectory monitoring. The following information is sent.
Messages regarding entering STATIONARY, LAUNCH and RETURN modes.
Emergency messages whenever they occur are also sent within a reasonable time frame (within 1 second). Some examples are below.
	-"Veer off course.\r\n" - during orientation monitoring.
	-"Temp. too high. \r\n" warning - when fuel temperature goes beyond the safe range.
Accelerometer and temperature sensor readings are to be sent once every 10 seconds in LAUNCH mode. For EE2024 students, some additional telemetry functionality as described in the modes below is also expected.
```

###The project makes use of code from the following library projects:
- CMSISv1p30_LPC17xx : for CMSIS 1.30 files relevant to LPC17xx
- MCU_Lib        	 : for LPC17xx peripheral driver files
- EaBaseBoard_Lib    : for Embedded Artists LPCXpresso Base Board peripheral drivers

These library projects must exist in the same workspace in order
for the project to successfully build.


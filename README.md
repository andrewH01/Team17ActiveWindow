# Team 17 - Active Window Primary Control Module (PCM)
by Maxwell Rapier, Jonathan Townsend, Damian Gunadasa & Andrew Hartnett


**Background:**
Smart home technology greatly improves home efficiency. Smart window technology stands to further increase efficiency in terms of power consumption by incorporating sensors that track real time environmental data and by networking sensor data between windows. This allows users to set automated preferences relating to temperature, pressure, humidity and sunlight to create a suitable, personal environment within the home bypassing traditional HVAC solutions

**Goal:**
Design a user interface that controls a system of motors based on environmental data from sensors to regulate indoor environments robustly between windows.

The Active Window - Window Primary Control Module will meet or exceed the following system specifications:
	
	1. Supports communication with network via WallBus (CAN Bus)
	2. Supports communication with inter-window motors via WindowBus (CAN Bus)
	3. Allow the user to set the height at which the window, thermal cover, and blinds are opened/drawn
	4. Allow the user to create a time schedule to open/close parts of the window
	5. Allow the user to stop smart capabilities of the window, returning it to a simple mechanical device
	6. Measure temperature, humidity, air pressure, and light inside and outside the room
	7. Display the height at which the window, thermal cover, and blinds are opened/drawn
	8. Ability to enter Sleep mode when not being used, can enter Active mode upon user touching screen
	9. Volume (LxWxH): 1,536 cubic cm. (12 x 16 x 8 cm) (93.7 cubic in. (4.72 x 6.30 x 3.15 in))


**Hardware Diagram** (as of 11/1/2020)
![Active Window PCM Hardware Diagram](/media/hardware_diagram_11_1.PNG)


**Software Diagram** (as of 11/1/2020)
![Active Window PCM Software Diagram](/media/software_diagram_11_1.PNG)


*This project is a work in progress. We are working to complete this as part of our final year studies at the University of Massachusetts Amherst in the College of Engineering. Updates will be made throughout the 2020-21 school year.*

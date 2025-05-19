# **Gesture-Based Frequency Controller Utilizing Direct Digital Synthesis and Computer Vision**

## **Overview**

Controlling electronic devices through gestures is an exciting way to make human-machine interaction more natural and intuitive. Instead of using buttons or knobs, we can use hand movements to control things like sound frequency, brightness, or other signals.

This project combines hand gesture recognition with signal generation to create a gesture-based frequency controller. By using a webcam to track hand rotation and finger positions, we translate natural hand movements into frequency values. These values are then used by an Arduino to generate sine waves using Direct Digital Synthesis (DDS). This approach offers a fun and innovative way to control signal frequencies without any physical contact.

Hand movements are captured using a webcam and interpreted using **MediaPipe** and **OpenCV** in Python. The detected hand rotation angle is converted into a frequency (100–1000 Hz) and transmitted to an **Arduino Uno**, which uses **Direct Digital Synthesis (DDS)** to output a sine wave using PWM. The PWM signal is then passed through an **RC low-pass filter** to approximate an analog sine wave, which can be visualized on a **Digital Storage Oscilloscope (DSO)**.

## **Theory / Background**

### Direct Digital Synthesis (DDS)  
DDS is a technique to create precise and stable waveforms like sine waves using digital electronics. It uses a phase accumulator to step through a pre-calculated sine wave table, outputting values that control the pulse width modulation (PWM) on the Arduino. This method allows for flexible and accurate frequency generation.

### Hand Gesture Recognition  
We use MediaPipe and OpenCV in Python to detect hand landmarks from a live webcam feed. MediaPipe provides precise 3D coordinates of hand joints, enabling us to calculate wrist rotation angles and detect whether fingers are open or closed.

### PWM and RC Filtering  
Arduino outputs the sine wave digitally as a PWM signal. The RC low-pass filter smooths the PWM signal into an analog-like sine wave, which can then be observed on an oscilloscope.




## **Components Used**

### Software
- Python 3.11.2
- OpenCV
- MediaPipe
- Tkinter (GUI)
- PySerial
- Arduino IDE

### Hardware
- Arduino Uno
- Webcam (PC)
- Resistor + Capacitor (RC filter)
- Digital Storage Oscilloscope (DSO)

![App Screenshot](https://github.com/itsharshschoice/Gesture-Based-Frequency-Controller-Utilizing-Direct-Digital-Synthesis-and-Computer-Vision/blob/main/Images/hardware.png?raw=true)


## **Directory Structure**

Gesture-Based-Frequency-Control:

| Code | Functionality |
|-|-|
| arduino_dds.ino | Arduino code for DDS-based sine generation |
| gesture_control.ipynb | Python code for gesture detection and frequency control |



## **How It Works**

1. **Hand Gesture Detection (Python):**
   - Captures live video feed.
   - Detects hand landmarks using MediaPipe.
   - Calculates wrist rotation angle and finger positions.

2. **Frequency Calculation:**
   - Maps wrist rotation angle (−90° to +90°) to a frequency (100–1000 Hz).
   - Frequency is "locked" if all fingers are closed.

3. **Serial Communication:**
   - Frequency and finger states are sent to Arduino over USB Serial.

4. **DDS on Arduino:**
   - Uses a 32-bit phase accumulator and a 256-sample sine lookup table.
   - Generates PWM duty cycle values that represent a sine wave.
   - PWM is output on Pin 9 using 8-bit Fast PWM.

5. **Analog Signal Generation:**
   - PWM signal is filtered using an RC low-pass filter.
   - Output is observed on a DSO.


## **Output and Results**

- **Live GUI Display**: Finger states and current frequency shown in real-time.

![App Screenshot](https://github.com/itsharshschoice/Gesture-Based-Frequency-Controller-Utilizing-Direct-Digital-Synthesis-and-Computer-Vision/blob/main/Images/output1.png?raw=true)

![App Screenshot](https://github.com/itsharshschoice/Gesture-Based-Frequency-Controller-Utilizing-Direct-Digital-Synthesis-and-Computer-Vision/blob/main/Images/output2.png?raw=true)


- **PWM Sine Output and DSO Verification**: Smooth analog-like sine wave generated from PWM. Frequency and waveform integrity confirmed.

![App Screenshot](https://github.com/itsharshschoice/Gesture-Based-Frequency-Controller-Utilizing-Direct-Digital-Synthesis-and-Computer-Vision/blob/main/Images/output3.png?raw=true)


## **Features**

- Real-time video-based hand tracking
- Frequency dynamically controlled by hand rotation
- Finger detection to pause frequency changes
- Sine waveform generation via DDS and PWM
- Serial communication between PC and Arduino
- Waveform output verified on oscilloscope


## **Challenges and Solutions**

- **Frequency Stability:** Rapid hand movements caused sudden jumps in frequency. We addressed this by locking frequency changes when all fingers are closed, allowing users to pause adjustments.

- **Signal Noise:** The initial PWM output was noisy. Adding an RC low-pass filter effectively smoothed the signal for a clean sine wave output.



# FM Transmitter with Arduino & Si4713
#### Author: Bocaletto Luca

A professional Arduino UNO project using the SparkFun Si4713 breakout to broadcast line-level audio over FM (87.5 – 108 MHz), complete with RDS support.

## Bill of Materials
- Arduino UNO (5 V)  
- Si4713 FM transmitter breakout (SparkFun or equivalent)  
- LM386 audio amplifier (for mic or line input)  
- 3.5 mm audio jack (IN+ and IN–)  
- 17 cm wire antenna  
- LED + 220 Ω resistor (status indicator)  
- Push-button (mute/unmute)  
- Jumper wires & breadboard  
- USB cable  

## Wiring Diagram

        Arduino UNO         Si4713 Breakout        LM386 Amp         Audio Jack         Antenna
        ─────────────────────────────────────────────────────────────────────────────────────
        5V    ─► VIN       SDA ─► A4            IN+ ─► AUDIO_IN   Tip  ─► Left Channel   ─► wire
        GND   ─► GND       SCL ─► A5            IN– ─► AUDIO_IN   Ring ─► Right Channel  ─► GND
        ANT   ─► ANT       OUT ─► LM386_OUT  Sleeve (ground)
        Button ─► D2 (INPUT_PULLUP, press to GND)
        LED    ─► D6 (via 220 Ω resistor to GND)

## Usage

1. Install the **SparkFunSi4713** library via Arduino Library Manager.  
2. Open **Serial Monitor** at `115200` baud.  
3. Send commands:

        F <MHz>  — set frequency (e.g. `F 99.7`)
        V <0–63> — set volume level
        P <PS>   — set 8-char RDS Program Service name
        T <RT>   — set RDS Radio Text (max 64 chars)
        S        — show current settings

4. Feed audio into the LM386 amplifier.  
5. Tune your FM receiver to the chosen frequency.

> **Note:** Always check local regulations before transmitting on FM band.  

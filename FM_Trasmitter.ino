#include <Wire.h>
#include <SparkFunSi4713.h>

#define LED_PIN     6    // status LED (GPOUT)
#define BUTTON_PIN  2    // mute/unmute button

Si4713_FM si4713;
float    freqMHz = 99.5;                   // default frequency
uint8_t  volume  = 32;                     // default volume (0–63)
char     rdsPS[9]  = "ARDUINO";            // 8-char Program Service
char     rdsRT[65] = "Welcome to Arduino FM!"; // up to 64-char Radio Text

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    Wire.begin();

    Serial.println();
    Serial.println("FMTransmitter v1.0");
    Serial.println("Initializing Si4713...");

    if (si4713.begin() != SI4713_SUCCESS) {
        Serial.println("Si4713 init failed!");
        while (1);
    }

    // Configure FM transmitter
    si4713.setProperty(SI4713_PROP_TX_POWER,     0x0510); // ~10 dBm
    si4713.setProperty(SI4713_PROP_TX_DEVIATION, 0x0028);
    setFrequency(freqMHz);
    setVolume(volume);
    setRDS(rdsPS, rdsRT);

    Serial.println("Ready. Enter commands:");
}

void loop() {
    if (Serial.available()) {
        String line = Serial.readStringUntil('\n');
        line.trim();
        if (line.length() > 0) {
            char cmd = toupper(line.charAt(0));
            String arg = line.substring(1);
            arg.trim();

            switch (cmd) {
                case 'F': freqMHz = arg.toFloat(); setFrequency(freqMHz); break;
                case 'V': volume  = constrain(arg.toInt(), 0, 63); setVolume(volume); break;
                case 'P': arg.toCharArray(rdsPS, 9); break;
                case 'T': arg.toCharArray(rdsRT, 65); break;
                case 'S': printStatus(); break;
            }
            setRDS(rdsPS, rdsRT);
        }
    }

    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 500) {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        lastBlink = millis();
    }
}

void setFrequency(float mhz) {
    uint16_t freqKHz = uint16_t(mhz * 1000);
    si4713.txTuneFrequency(freqKHz);
    Serial.print("Frequency: ");
    Serial.print(mhz, 1);
    Serial.println(" MHz");
}

void setVolume(uint8_t vol) {
    si4713.txTunePowerAndVolume(si4713.getCurrentPower(), vol);
    Serial.print("Volume: ");
    Serial.println(vol);
}

void setRDS(const char* ps, const char* rt) {
    si4713.setRDS(ps, rt);
    Serial.print("RDS PS=");
    Serial.print(ps);
    Serial.print(" RT=");
    Serial.println(rt);
}

void printStatus() {
    Serial.println("=== Status ===");
    Serial.print("Freq: ");
    Serial.print(freqMHz, 1);
    Serial.print(" MHz  Vol: ");
    Serial.print(volume);
    Serial.print("  PS: ");
    Serial.print(rdsPS);
    Serial.print("  RT: ");
    Serial.println(rdsRT);
}

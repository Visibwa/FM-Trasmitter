#include <Wire.h>
#include <SparkFunSi4713.h>

#define LED_PIN      6      // GPOUT status LED
#define BUTTON_PIN   2      // Mute/unmute (INPUT_PULLUP)
#define SERIAL_BAUD  115200

Si4713_FM tx;
float      freqMHz    = 99.5;
uint8_t    volume     = 32;
uint8_t    lastVolume = 32;
bool       muted      = false;
char       rdsPS[9]   = "ARDUINO";
char       rdsRT[65]  = "Welcome to Arduino FM!";

// simple serial command buffer
char       cmdBuf[64];
uint8_t    cmdPos     = 0;

// button debounce
unsigned long btnLast    = 0;
const unsigned long debounce = 200;

// LED blink
unsigned long blinkLast     = 0;
const unsigned long blinkInt = 500;

void setup() {
    Serial.begin(SERIAL_BAUD);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    Wire.begin();

    Serial.println();
    Serial.println("FMTransmitter Pro v1.1");
    Serial.println("Initializing Si4713...");

    if (tx.begin() != SI4713_SUCCESS) {
        Serial.println("ERROR: Si4713 init failed!");
        while (1);
    }

    tx.setProperty(SI4713_PROP_TX_POWER,     0x0510); // ~10 dBm
    tx.setProperty(SI4713_PROP_TX_DEVIATION, 0x0028);
    applyFrequency();
    applyVolume();
    applyRDS();
    printHelp();
}

void loop() {
    handleButton();
    handleSerial();
    blinkLED();
}

// non-blocking button check
void handleButton() {
    if (digitalRead(BUTTON_PIN) == LOW && millis() - btnLast > debounce) {
        btnLast = millis();
        muted = !muted;
        if (muted) {
            lastVolume = volume;
            volume = 0;
        } else {
            volume = lastVolume;
        }
        applyVolume();
        Serial.print(muted ? "Muted\n" : "Unmuted\n");
    }
}

// read & parse serial commands
void handleSerial() {
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\r') continue;
        if (c == '\n') {
            cmdBuf[cmdPos] = '\0';
            processCommand(cmdBuf);
            cmdPos = 0;
        } else if (cmdPos < sizeof(cmdBuf) - 1) {
            cmdBuf[cmdPos++] = c;
        }
    }
}

void processCommand(char *s) {
    char cmd = toupper(s[0]);
    char *arg = s + 1;
    while (*arg == ' ') arg++;
    switch (cmd) {
        case 'F': freqMHz = atof(arg); applyFrequency(); break;
        case 'V': {
            int v = atoi(arg);
            volume = constrain(v, 0, 63);
            if (!muted) lastVolume = volume;
            applyVolume(); 
        } break;
        case 'P': strncpy(rdsPS, arg, 8); rdsPS[8] = '\0';        break;
        case 'T': strncpy(rdsRT, arg, 64); rdsRT[64] = '\0';      break;
        case 'S': printStatus();                                   break;
        case 'H': printHelp();                                     break;
        default:  Serial.println("Unknown command, send 'H' for help"); break;
    }
    applyRDS();
}

// simple LED blink when transmitting
void blinkLED() {
    if (millis() - blinkLast > blinkInt) {
        blinkLast = millis();
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
}

void applyFrequency() {
    uint16_t kHz = uint16_t(freqMHz * 1000);
    tx.txTuneFrequency(kHz);
    Serial.print("Set Freq: "); Serial.print(freqMHz, 1); Serial.println(" MHz");
}

void applyVolume() {
    tx.txTunePowerAndVolume(tx.getCurrentPower(), volume);
    Serial.print("Set Volume: "); Serial.println(volume);
}

void applyRDS() {
    tx.setRDS(rdsPS, rdsRT);
    Serial.print("Set RDS PS="); Serial.print(rdsPS);
    Serial.print(" RT="); Serial.println(rdsRT);
}

void printStatus() {
    Serial.println("=== Status ===");
    Serial.print("Freq: "); Serial.print(freqMHz, 1); Serial.println(" MHz");
    Serial.print("Vol: "); Serial.println(volume);
    Serial.print("PS: ");  Serial.println(rdsPS);
    Serial.print("RT: ");  Serial.println(rdsRT);
    Serial.print("Mute: ");Serial.println(muted ? "Yes" : "No");
}

void printHelp() {
    Serial.println("Commands:");
    Serial.println(" F <MHz>   Set frequency (e.g. F 99.7)");
    Serial.println(" V <0-63>  Set volume level");
    Serial.println(" P <PS>    Set RDS Program Service (8 char)");
    Serial.println(" T <RT>    Set RDS Radio Text (up to 64 char)");
    Serial.println(" S         Show status");
    Serial.println(" H         Show this help");
}

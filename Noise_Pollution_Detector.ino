/*
  =====================================================
   NOISE POLLUTION DETECTOR
   ATL Project - Class 8
   =====================================================

  WHAT THIS PROJECT DOES:
  ------------------------
  A sound sensor (KY-038 module) listens to the noise
  around it. The Arduino reads the sound in TWO different
  ways from the SAME sensor:

  1) ANALOG VALUE (from pin A0)
     - This gives a NUMBER from 0 to 1023.
     - It tells us HOW LOUD the sound is (the amplitude).
     - Louder sound = bigger number.
     - This is like a "volume meter".

  2) DIGITAL VALUE (from pin D0)
     - This gives only TWO possible values: HIGH (1) or LOW (0).
     - The module compares the sound to a threshold that
       YOU set using the small blue screw (potentiometer)
       on the KY-038 board.
     - If sound crosses that threshold -> HIGH, else LOW.
     - This is like a simple "yes/no, is it loud?" switch.

  WHY LEARN BOTH?
  ----------------
  - ANALOG values let us measure and show a NUMBER (great for
    graphs, levels, OLED display, Serial Monitor readings).
  - DIGITAL values are simple and fast, good for triggering
    an action like turning on a buzzer or LED instantly.
  Real world sensors often give you both options, and a good
  student should know when to use which one.

  OUTPUTS OF THIS PROJECT:
  --------------------------
  - RGB LED shows noise level as a colour:
      GREEN  = Normal noise level
      YELLOW = Moderate noise level
      RED    = High noise pollution
  - Buzzer beeps when noise is HIGH (pollution alert)
  - OLED display shows live analog value, digital value,
    and the noise status in text
  - Serial Monitor prints the same information, so students
    can see the raw numbers while the code is running

  HARDWARE USED:
  ---------------
  - Arduino Uno
  - KY-038 Sound Sensor Module
  - Common Anode RGB LED (3 x 220 ohm resistors)
  - Buzzer
  - 0.96" I2C OLED Display (SSD1306, 128x64)

  PIN CONNECTIONS:
  ------------------
  KY-038 VCC   -> Arduino 5V
  KY-038 GND   -> Arduino GND
  KY-038 A0    -> Arduino A0
  KY-038 D0    -> Arduino Pin 2

  OLED VCC     -> Arduino 5V
  OLED GND     -> Arduino GND
  OLED SDA     -> Arduino A4
  OLED SCL     -> Arduino A5

  RGB LED common pin -> Arduino 5V (Common ANODE)
  RGB LED Red leg     -> 220 ohm resistor -> Arduino Pin 9
  RGB LED Green leg   -> 220 ohm resistor -> Arduino Pin 10
  RGB LED Blue leg    -> 220 ohm resistor -> Arduino Pin 11

  Buzzer (+) -> Arduino Pin 8
  Buzzer (-) -> Arduino GND

  LIBRARIES NEEDED (install from Library Manager):
  ----------------------------------------------------
  1. "Adafruit GFX Library"
  2. "Adafruit SSD1306"

  =====================================================
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- OLED SETTINGS ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1        // Uno shares reset pin, so -1 is fine
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- PIN DEFINITIONS ----------
const int SOUND_ANALOG_PIN  = A0;   // Gives a NUMBER (0-1023)
const int SOUND_DIGITAL_PIN = 2;    // Gives HIGH or LOW only

const int RED_PIN    = 9;
const int GREEN_PIN   = 10;
const int BLUE_PIN   = 11;

const int BUZZER_PIN = 8;
const int STATUS_LED = 13;          // Arduino's built-in LED

// ---------- THRESHOLD VALUES (You can adjust after testing) ----------
// These decide where "Normal", "Moderate" and "High" begin.
// Test in your classroom and change these numbers if needed.
const int NORMAL_LIMIT   = 20;     // Below this = Normal
const int MODERATE_LIMIT = 50;     // Below this = Moderate, above = High

void setup() {
  // Start Serial Monitor for printing readings
  Serial.begin(9600);
  Serial.println(F("=== Noise Pollution Detector Starting ==="));

  // Set pin modes
  pinMode(SOUND_DIGITAL_PIN, INPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

  digitalWrite(STATUS_LED, HIGH);   // Built-in LED ON = system is powered and running

  // Start the OLED display (I2C address is usually 0x3C)
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED not found! Check wiring."));
    // We do not stop the program - project can still run using Serial Monitor
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Noise Pollution"));
  display.println(F("Detector Ready..."));
  display.display();
  delay(1500);
}

void loop() {
  // ---------- STEP 1: READ THE ANALOG VALUE ----------
  // analogRead() returns a number between 0 and 1023
  // This tells us the STRENGTH (loudness) of the sound
  int analogValue = analogRead(SOUND_ANALOG_PIN);

  // ---------- STEP 2: READ THE DIGITAL VALUE ----------
  // digitalRead() returns only HIGH (1) or LOW (0)
  // This tells us if sound crossed the threshold set on
  // the KY-038 module's blue potentiometer
  int digitalValue = digitalRead(SOUND_DIGITAL_PIN);

  // ---------- STEP 3: DECIDE NOISE LEVEL USING ANALOG VALUE ----------
  // We use the analog number to classify noise into 3 categories.
  String noiseStatus;

  if (analogValue < NORMAL_LIMIT) {
    noiseStatus = "NORMAL";
    setColor(false, true, false);   // GREEN
    noTone(BUZZER_PIN);
  }
  else if (analogValue < MODERATE_LIMIT) {
    noiseStatus = "MODERATE";
    setColor(true, true, false);    // YELLOW (Red + Green together)
    noTone(BUZZER_PIN);
  }
  else {
    noiseStatus = "HIGH! POLLUTION";
    setColor(true, false, false);   // RED
    tone(BUZZER_PIN, 1000);         // Buzzer beeps at 1000 Hz
  }

  // ---------- STEP 4: PRINT BOTH VALUES ON SERIAL MONITOR ----------
  // This helps students SEE the difference between analog and digital
  Serial.print("Analog Value (Loudness Number): ");
  Serial.print(analogValue);

  Serial.print("   |   Digital Value (Threshold HIGH/LOW): ");
  Serial.print(digitalValue == HIGH ? "HIGH" : "LOW");

  Serial.print("   |   Noise Level: ");
  Serial.println(noiseStatus);

  // ---------- STEP 5: SHOW EVERYTHING ON OLED DISPLAY ----------
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println(F("Noise Pollution Meter"));
  display.println(F("--------------------"));

  display.print(F("Analog : "));
  display.println(analogValue);

  display.print(F("Digital: "));
  display.println(digitalValue == HIGH ? "HIGH" : "LOW");

  display.println();
  display.setTextSize(2);
  display.println(noiseStatus);

  display.display();

  // Small delay so readings are easier to read on screen/monitor
  delay(300);
}

// ---------- FUNCTION TO CONTROL RGB LED ----------
// IMPORTANT: This is a COMMON ANODE RGB LED.
// That means: LOW = colour ON, HIGH = colour OFF (it is "inverted").
// This function makes it simple - just tell it true/false for each colour
// and it takes care of the inverted logic automatically.
void setColor(bool red, bool green, bool blue) {
  digitalWrite(RED_PIN,   red   ? LOW : HIGH);
  digitalWrite(GREEN_PIN, green ? LOW : HIGH);
  digitalWrite(BLUE_PIN,  blue  ? LOW : HIGH);
}

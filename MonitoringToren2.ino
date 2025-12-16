#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <U8x8lib.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C

// Create an instance of the U8x8 library
U8X8_SSD1306_128X64_NONAME_HW_I2C display(SCREEN_ADDRESS, /* reset=*/ U8X8_PIN_NONE);

// Set pin for sensor and actuators
const int buzzer = 3;         // Buzzer pin
const int ledPin_R = 4;       // Red LED pin
const int ledPin_G = 5;       // Green LED pin
const int decreaseButton = 6; // Button to decrease toren height
const int increaseButton = 7; // Button to increase toren height
const int trigPin = 8;        // Ultrasonic sensor trigger pin
const int echoPin = 9;        // Ultrasonic sensor echo pin
const int chipSelect = 10;    // Chip select pin for SD card
File dataFile;                // File object for SD card operations

// Set variables
long duration;          // Time duration for ultrasonic pulse
int water_distance;     // Distance measured by ultrasonic sensor
int percentage;         // Water percentage in the tank
int i;                  // Loop counter for data collection
int toren_height = 100; // Height of the tank in cm

// Structure to store water data
struct data {
  int percentage;      // Water percentage
  int water_height;    // Water height in cm
} raw[30], total, avg; // Arrays for raw data, total, and average

void setup() {
  // Initialize pins for input/output
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
  pinMode(buzzer, OUTPUT);
  pinMode(ledPin_G, OUTPUT);
  pinMode(ledPin_R, OUTPUT);
  pinMode(increaseButton, INPUT_PULLUP);
  pinMode(decreaseButton, INPUT_PULLUP);

  Serial.begin(9600); // Start serial communication at 9600 baud
  SPI.begin();        // Initiate SPI bus

  // Initialize SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    for (;;);
  }
  Serial.println("SD card initialized.");

  // Initialize the display
  display.begin();
  display.setPowerSave(0); // Turn on the display
  splashScreen();          // Display splash screen
  delay(2000);             // Pause for 2 seconds
}

void splashScreen() {
  // Display splash screen text
  display.clearDisplay();
  display.setFont(u8x8_font_chroma48medium8_r);
  display.drawString(4, 1, "KELOMPOK 10");
  display.drawString(4, 3, "SISTEM");
  display.drawString(2, 4, "MONITORING");
  display.drawString(2, 5, "PENAMPUNGAN AIR");
}

void updateDisplay(int waterHeight, int percentage) {
  // Update the OLED display with current readings
  display.clearDisplay();
  char buffer[16];

  snprintf(buffer, sizeof(buffer), "Tank: %dcm", toren_height);
  display.drawString(0, 0, buffer);
  snprintf(buffer, sizeof(buffer), "Height: %dcm", waterHeight);
  display.drawString(0, 1, buffer);
  snprintf(buffer, sizeof(buffer), "Percent: %d%%", percentage);
  display.drawString(0, 2, buffer);

  // Display status based on water percentage
  if (percentage < 20) {
    display.drawString(0, 6, "Status: LOW");
  } else if (percentage > 85) {
    display.drawString(0, 6, "Status: HIGH");
  } else {
    display.drawString(0, 6, "Status: OKAY");
  }
}

// Function to modify toren_height
void modifyTorenHeight(int* heightPtr, int change) {
  *heightPtr += change; // Modify the value pointed to by heightPtr
  Serial.println(*heightPtr); // Print the new height to the serial monitor
}

// Calculate distance from ultrasonic sensor pulse duration
int calc_distance(int t) {
  return (t / 2) / 29.1; // Convert time to distance in cm
}

// Calculate water height based on tank height and water distance
int calc_height(int h, int d) {
  return h - d;
}

// Calculate percentage of water in the tank
int calc_percent(int w, int h) {
  int p = (w * 100) / h; // Calculate percentage
  return constrain(p, 0, 100); // Ensure percentage is between 0 and 100
}

void component_low() {
  // Activate components for low water level
  digitalWrite(ledPin_R, HIGH); // Turn on red LED
  digitalWrite(ledPin_G, LOW);  // Turn off green LED
  tone(buzzer, 1000);           // Activate buzzer
}

void component_high() {
  // Activate components for high water level
  digitalWrite(ledPin_R, HIGH); // Turn on red LED
  digitalWrite(ledPin_G, LOW);  // Turn off green LED
  digitalWrite(buzzer, HIGH);           // Activate buzzer
}

void component_okay() {
  // Activate components for normal water level
  digitalWrite(ledPin_R, LOW);  // Turn off red LED
  digitalWrite(ledPin_G, HIGH); // Turn on green LED
  noTone(buzzer);               // Deactivate buzzer
}

void logToSDCard(int waterHeight, int percentage) {
  // Log data to SD card
  dataFile = SD.open("log.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print("Height: ");
    dataFile.print(waterHeight);
    dataFile.print(" cm, Percentage: ");
    dataFile.print(percentage);
    dataFile.println("%");
    dataFile.close(); // Close the file
    Serial.println("Data logged to SD card.");
  } else {
    Serial.println("Error opening log file!");
  }
}

void loop() {
  // Collect 30 data samples
  for (i = 0; i < 30; i++) {
    if (digitalRead(increaseButton) == LOW) {
      modifyTorenHeight(&toren_height, 2); // Increase toren height by 2 cm
      delay(200); // Debounce delay
    }

    if (digitalRead(decreaseButton) == LOW) {
      modifyTorenHeight(&toren_height, -2); // Decrease toren height by 2 cm
      delay(200); // Debounce delay
    }

    // Clears the trigpin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);

    // Calculate the distance and percentage
    water_distance = calc_distance(duration); // Calculate distance
    raw[i].water_height = calc_height(toren_height, water_distance); // Calculate water height
    raw[i].percentage = calc_percent(raw[i].water_height, toren_height); // Calculate percentage

    //serial print at all times
    Serial.print("Distance: ");
    Serial.print(water_distance);
    Serial.println(" cm");
    Serial.print("Percentage: ");
    Serial.print(raw[i].percentage);
    Serial.println(" %");
    Serial.println(raw[i].water_height);

    updateDisplay(raw[i].water_height, raw[i].percentage); // Update the display

    // Trigger appropriate components based on percentage
    if (raw[i].percentage < 20) {
      component_low();
    } else if (raw[i].percentage > 85) {
      component_high();
    } else {
      component_okay();
    }

    delay(500); // Delay between samples
  }

  // Reset totals for averaging
  total.percentage = 0;
  total.water_height = 0;

  // Sum up the collected data
  for (i = 0; i < 30; i++) {
    total.percentage += raw[i].percentage;
    total.water_height += raw[i].water_height;
  }

  // Calculate averages
  avg.percentage = total.percentage / 30;
  avg.water_height = total.water_height / 30;

  logToSDCard(avg.water_height, avg.percentage); // Log data to SD card
}

#include "HX711.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DOUT  3
#define CLK  2
HX711 scale;// Declare the scale object without parameters

// Define pins for the ultrasonic sensor
const int trigPin = 9;
const int echoPin = 8;

// Define pin for the LED
const int ledPin = 13;

// Define distance threshold (in cm)
const int distanceThreshold = 10;

float weight; 
float calibration_factor = 419640; // Adjust this value as needed
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// LED pin definitions
#define LED1 4 // Pin for LED 1
#define LED2 5 // Pin for LED 2
#define LED3 6 // Pin for LED 3
#define LED4 7 // Pin for LED 4

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");
  
  scale.begin(DOUT, CLK); // Initialize the scale with DOUT and CLK
  scale.set_scale();
  scale.tare(); // Reset the scale to 0
  long zero_factor = scale.read_average(); // Get a baseline reading
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);
  
  // Initialize the display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  // Initialize LEDs
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(trigPin, OUTPUT);
  
 
  pinMode(echoPin, INPUT);

}

void loop() {
  scale.set_scale(calibration_factor);// Adjust to this calibration factor
  weight = scale.get_units(5); 

  if(weight < 0) {
    weight = 0.00;
  }

  Serial.print("Kilogram: ");
  Serial.print(weight, 2); // Print weight with 2 decimal places
  Serial.print(" Kg");
  Serial.print(" calibration_factor: ");
  Serial.println(calibration_factor);

  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(0, 0); // column row
  display.print("Weight:");
  display.setTextSize(3);
  display.setCursor(0, 35);
  display.print(weight, 2); // Print weight with 2 decimal places
  display.setTextSize(3);
  display.setCursor(75, 35);
  display.print("Kg"); 
  display.display();
  
  // Control LEDs based on weight
  if (weight < 0.01) {
    blinkLED(LED1);
  } else if (weight < 0.09) {
    blinkLED(LED2);
  } else if (weight < 0.45) {
    blinkLED(LED3);
  } else {
    blinkLED(LED4);
  }
  
  delay(100);
  long duration, distance;
  
  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Set the trigPin HIGH for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance (in cm)
  distance = duration * 0.034 / 2; // Speed of sound is 0.034 cm/µs
  
  // Print the distance to the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Check if the distance is less than the threshold
  if (distance < distanceThreshold) {
    digitalWrite(ledPin, HIGH); // Turn the LED on if the distance is less than the threshold
  } else {
    digitalWrite(ledPin, LOW); // Turn the LED off if the distance is greater than the threshold
  }

  // Wait for a short period before the next measurement
  delay(500);

}

void blinkLED(int ledPin) {
  digitalWrite(ledPin, HIGH); // Turn the LED on
  delay(250);                 // Wait for 250 milliseconds
  digitalWrite(ledPin, LOW);  // Turn the LED off
  delay(250);                 // Wait for 250 milliseconds
}
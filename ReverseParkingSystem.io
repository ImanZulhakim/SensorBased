// Author: Iman Zulhakim
// Reverse Parking System
// 5/9/2024

const int trigPin = 12; //D6
const int echoPin = 14; //D5

#define GREEN_LED D0
#define YELLOW_LED D1
#define RED_LED D2
#define BUZZER D3

// Define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034

long duration;
float distanceCm;

void setup() {
  Serial.begin(115200);         // Start serial communication
  pinMode(trigPin, OUTPUT);     // Set the trigPin as an Output
  pinMode(echoPin, INPUT);      // Set the echoPin as an Input
  pinMode(GREEN_LED, OUTPUT);   // Set green LED pin as output
  pinMode(YELLOW_LED, OUTPUT);  // Set yellow LED pin as output
  pinMode(RED_LED, OUTPUT);     // Set red LED pin as output
  pinMode(BUZZER, OUTPUT);      // Set buzzer pin as output
}

void loop() {
  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Set the trigPin on HIGH state for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in centimeters
  distanceCm = duration * SOUND_VELOCITY / 2;

  // Print the distance on the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);

  // Check distance and act accordingly
  if (distanceCm > 20) { // Light Green LED when above 20 cm
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);  // Turn off the buzzer
  } else if (distanceCm > 10) { // Light Yellow LED when between 10 to 20 cm
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    tone(BUZZER, 1000);  // Turn on the buzzer at 1000 Hz
    delay(200);          // Buzzer on for 200 milliseconds
    noTone(BUZZER);      // Turn off the buzzer
    delay(200);          // Wait for 200 milliseconds
  } else if (distanceCm > 5) { // Light Red LED when between 5 to 10 cm
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER, 1000);  // Turn on the buzzer at 1000 Hz
    delay(200);          // Buzzer on for 200 milliseconds
    noTone(BUZZER);      // Turn off the buzzer
    delay(50);           // Wait for 50 milliseconds

  } else { // keep lighting the Red LED when below 5 cm but beep longer
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER, 3000);  // Turn on the buzzer at 3000 Hz
    delay(5000);           // Buzzer on for 5000 milliseconds
    noTone(BUZZER);      // Turn off the buzzer

    
  }
  delay(1000);  // Wait 1 seconds before taking the next measurement

}

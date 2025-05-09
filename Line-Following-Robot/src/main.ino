#include <Arduino.h>
#include <avr/interrupt.h>
#include <EEPROM.h>

// Pin definitions for motor control
#define RDIR 11     // Right motor direction pin
#define LDIR 9      // Left motor direction pin
#define LPWM 5      // Left motor speed (PWM)
#define RPWM 6      // Right motor speed (PWM)

// Pin definitions for user interface
#define BUTTON1 7   // Button 1 for starting with speed 60
#define BUTTON2 8   // Button 2 for starting with speed 70
#define BUTTON3 12  // Button 3 for starting with speed 80
#define BUZZER 4    // Buzzer for auditory feedback

// Pin definitions for RGB LED
#define RED 3       // Red LED pin
#define GREEN 2     // Green LED pin
#define BLUE 13     // Blue LED pin

// Global variables
int addr = 0;                       // EEPROM address for calibration data
volatile int lastPos;               // Last position for PID derivative term
volatile unsigned char isCalib = 0; // Calibration mode flag
volatile int servoPwm;              // PWM value for motor control
volatile unsigned char sensor;      // Sensor state (8-bit)
unsigned int sensorValue[8];        // Raw sensor readings
unsigned int sensorPID[8];          // Normalized sensor values for PID
unsigned int black_value[8];        // Black calibration values
unsigned int white_value[8];        // White calibration values
unsigned int compare_value[8];      // Threshold values for sensor detection
int speed_run_forward;              // Base forward speed
int cnt = 0;                        // Counter for soft start
unsigned char pattern, start;       // State machine variables
int line = 0;                       // Current line position
int RememberLine = 0;               // Last detected line direction
float kp;                           // Proportional gain for PID
int kd;                             // Derivative gain for PID

// Initialize hardware and parameters
void setup() {
  // Configure motor control pins
  pinMode(LDIR, OUTPUT);
  pinMode(RDIR, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(RPWM, OUTPUT);

  // Configure input buttons with internal pull-up resistors
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);

  // Configure RGB LED pins
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  // Initialize motor directions and LED states
  digitalWrite(RDIR, LOW);
  digitalWrite(LDIR, LOW);
  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);
  digitalWrite(BLUE, HIGH);

  // Set initial parameters
  speed_run_forward = 50; // Default forward speed (PWM: 10-255)
  speed_run(0, 0);        // Stop motors initially
  pattern = 10;           // Initial state for soft start
  start = 0;              // Wait for button press to begin
  readEeprom();           // Load calibration data from EEPROM
  Serial.begin(9600);     // Initialize serial communication

  // Play startup sound sequence
  for (int i = 2000; i < 3500; i += 500) {
    tone(BUZZER, i, 100);
    delay(100);
  }

  // Initialize timer and buzzer
  timer_init();
  isCalib = 1; // Enable calibration mode
  pinMode(BUZZER, OUTPUT);
}

// Configure Timer2 for periodic sensor reading
void timer_init() {
  ASSR = (0 << EXCLK) | (0 << AS2); // Use internal clock
  TCCR2A = (0 << COM2A1) | (0 << COM2A0) | (0 << COM2B1) | (0 << COM2B0) | (0 << WGM21) | (0 << WGM20); // Normal mode
  TCCR2B = (0 << WGM22) | (1 << CS22) | (1 << CS21) | (1 << CS20); // Prescaler 1024
  TCNT2 = 0xB2; // Initial counter value
  TIMSK2 = (1 << TOIE2); // Enable overflow interrupt
}

// Timer2 overflow interrupt service routine
ISR(TIMER2_OVF_vect) {
  TCNT2 = 0xB2; // Reset counter
  read_sensor(); // Read sensor data
  cnt++; // Increment soft start counter
}

// Main control loop
void loop() {
  // Wait for button press to start
  while (start == 0) {
    updateLine(); // Update calibration values
    if (digitalRead(BUTTON1) == 0) { // Start with speed 60
      start = 1;
      isCalib = 0;
      cnt = 0;
      speed_run_forward = 60;
    }
    if (digitalRead(BUTTON2) == 0) { // Start with speed 70
      start = 1;
      isCalib = 0;
      cnt = 0;
      speed_run_forward = 70;
    }
    if (digitalRead(BUTTON3) == 0) { // Start with speed 80
      start = 1;
      isCalib = 0;
      cnt = 0;
      speed_run_forward = 80;
    }
  }

  // State machine for robot behavior
  switch (pattern) {
    case 10: // Soft start phase
      if (cnt >= 100) {
        pattern = 11; // Transition to line following
        cnt = 0;
      }
      runforwardline(90); // Run at initial speed
      break;

    case 11: // Line following mode
      if (sensorMask(0x01) == 0x01) { // Detect leftmost sensor
        RememberLine = 1;
        cnt = 0;
      } else if (sensorMask(0x80) == 0x80) { // Detect rightmost sensor
        RememberLine = -1;
        cnt = 0;
      }
      if (cnt > 200) RememberLine = 0; // Reset memory after delay
      if (sensor == 0x00) { // No line detected
        if (RememberLine != 0) {
          pattern = 12; // Handle line loss
        } else {
          speed_run(0, 0); // Stop motors
        }
      } else {
        runforwardline(speed_run_forward); // Follow line
      }
      break;

    case 12: // Line recovery mode
      if (RememberLine == 1) { // Last seen on left
        speed_run(60, -40); // Turn right
        pattern = 21;
      } else if (RememberLine == -1) { // Last seen on right
        speed_run(-40, 60); // Turn left
        pattern = 31;
      } else {
        pattern = 11; // Return to line following
      }
      break;

    case 21: // Recovering from left deviation
      speed_run(60, -40); // Continue turning right
      RGB(1); // Indicate with green LED
      if (sensorMask(0xff) != 0) { // Line detected
        speed_run(60, 60 / 2); // Adjust turn
        pattern = 22;
      }
      break;

    case 22: // Re-aligning after left recovery
      speed_run(60, 60 / 2); // Smooth turn
      RGB(1); // Green LED
      if (sensorMask(0xfc) != 0) { // Line centered
        pattern = 11; // Resume line following
      }
      break;

    case 31: // Recovering from right deviation
      speed_run(-40, 60); // Continue turning left
      RGB(2); // Blue LED
      if (sensorMask(0xff) != 0) { // Line detected
        speed_run(60 / 2, 60); // Adjust turn
        pattern = 32;
      }
      break;

    case 32: // Re-aligning after right recovery
      speed_run(60 / 2, 60); // Smooth turn
      RGB(2); // Blue LED
      if (sensorMask(0x3f) != 0) { // Line centered
        pattern = 11; // Resume line following
      }
      break;

    case 100: // Stopped state
      speed_run(0, 0); // Stop motors
      RGB(millis() / 100 % 3); // Cycle LED colors
      break;

    default: // Fallback to line following
      pattern = 11;
      break;
  }
}

// Control robot to follow line using PID
void runforwardline(int tocdo) {
  switch (sensor) {
    case 0b00000000: // No line detected
      handleAndSpeed(servoPwm, tocdo); // Maintain last PID control
      break;

    case 0b00011000: // Centered on line
    case 0b00001000:
    case 0b00010000:
    case 0b00111000:
    case 0b00011100:
      kd = 5; // Set derivative gain
      RGB(0); // Red LED
      line = 0; // Line is centered
      handleAndSpeed(servoPwm, tocdo + 20); // Increase speed
      break;

    case 0b00111100: // Wide line detection
    case 0b00111110:
    case 0b01111110:
      kd = 5;
      RGB(4); // Custom LED state
      line = 0;
      handleAndSpeed(servoPwm, tocdo + 20);
      break;

    case 0b00001100: // Slight right deviation
    case 0b00000100:
    case 0b00001110:
    case 0b00011110:
      kd = 5;
      line = 1; // Line slightly to right
      handleAndSpeed(servoPwm, tocdo);
      break;

    case 0b00000110: // Moderate right deviation
    case 0b00000010:
    case 0b00000111:
      kd = 5;
      line = 2;
      handleAndSpeed(servoPwm, tocdo);
      break;

    case 0b00000011: // Sharp right deviation
    case 0b00000001:
    case 0b00001111:
    case 0b00011111:
    case 0b00111111:
      kd = 5;
      line = 3;
      handleAndSpeed(servoPwm, tocdo);
      break;

    case 0b00110000: // Slight left deviation
    case 0b00100000:
    case 0b01110000:
    case 0b01111000:
      kd = 5;
      line = -1; // Line slightly to left
      handleAndSpeed(servoPwm, tocdo);
      break;

    case 0b01100000: // Moderate left deviation
    case 0b01000000:
    case 0b11100000:
      kd = 5;
      line = -2;
      handleAndSpeed(servoPwm, tocdo);
      break;

    case 0b11000000: // Sharp left deviation
    case 0b10000000:
    case 0b11110000:
    case 0b11111000:
    case 0b11111100:
      kd = 5;
      line = -3;
      handleAndSpeed(servoPwm, tocdo);
      break;

    default: // Unhandled sensor state
      handleAndSpeed(servoPwm, tocdo);
      break;
  }
}

// Update calibration values during setup
void updateLine() {
  if (sensor == 0xff) beep(100); // Beep if all sensors detect line
  for (int i = 0; i < 8; i++) {
    Serial.print(sensorValue[i]);
    Serial.print("  ");
    if (black_value[i] == 0) black_value[i] = 1100; // Initialize black value
    if (sensorValue[i] < black_value[i]) black_value[i] = sensorValue[i]; // Update min
    if (sensorValue[i] > white_value[i]) white_value[i] = sensorValue[i]; // Update max
    compare_value[i] = (black_value[i] + white_value[i]) / 2; // Set threshold
  }
  Serial.println();
  RGB(millis() / 100 % 3); // Cycle LED colors during calibration
}

// Set RGB LED color
void RGB(int color) {
  switch (color) {
    case 0: // Red
      digitalWrite(RED, LOW);
      digitalWrite(GREEN, HIGH);
      digitalWrite(BLUE, HIGH);
      break;
    case 1: // Green
      digitalWrite(RED, HIGH);
      digitalWrite(GREEN, LOW);
      digitalWrite(BLUE, HIGH);
      break;
    case 2: // Blue
      digitalWrite(RED, HIGH);
      digitalWrite(GREEN, HIGH);
      digitalWrite(BLUE, LOW);
      break;
    default: // Off
      digitalWrite(RED, HIGH);
      digitalWrite(GREEN, HIGH);
      digitalWrite(BLUE, HIGH);
      break;
  }
}

// Read calibration data from EEPROM
void readEeprom() {
  for (int i = 0; i < 8; i++) {
    compare_value[i] = EEPROM.read(i) * 4; // Scale stored values
  }
}

// Apply PID control to adjust motor speeds
void handleAndSpeed(int angle, int speed1) {
  int speedLeft, speedRight;
  if ((speed1 + angle) > 255) speed1 = 255 - angle; // Limit max speed
  if ((speed1 - angle) > 255) speed1 = 255 + angle;
  speedLeft = speed1 + angle;
  speedRight = speed1 - angle;
  speed_run(speedLeft, speedRight); // Set motor speeds
}

// Set motor speeds with direction control
void speed_run(int speedDC_left, int speedDC_right) {
  // Left motor
  if (speedDC_left < 0) {
    analogWrite(LPWM, 255 + speedDC_left);
    digitalWrite(LDIR, HIGH); // Reverse
  } else {
    analogWrite(LPWM, speedDC_left);
    digitalWrite(LDIR, LOW); // Forward
  }
  // Right motor
  if (speedDC_right < 0) {
    analogWrite(RPWM, 255 + speedDC_right);
    digitalWrite(RDIR, HIGH); // Reverse
  } else {
    analogWrite(RPWM, speedDC_right);
    digitalWrite(RDIR, LOW); // Forward
  }
}

// Read and process sensor data
void read_sensor() {
  unsigned char temp = 0;
  unsigned int sum = 0;
  unsigned long avg = 0;
  int i, iP, iD, iRet;

  // Read analog sensors (inverted due to hardware)
  sensorValue[0] = 1023 - analogRead(A0);
  sensorValue[1] = 1023 - analogRead(A1);
  sensorValue[2] = 1023 - analogRead(A2);
  sensorValue[3] = 1023 - analogRead(A3);
  sensorValue[4] = 1023 - analogRead(A4);
  sensorValue[5] = 1023 - analogRead(A5);
  sensorValue[6] = 1023 - analogRead(A6);
  sensorValue[7] = 1023 - analogRead(A7);

  // Process sensor data
  for (int j = 0; j < 8; j++) {
    if (isCalib == 0) { // Limit values during run mode
      if (sensorValue[j] < black_value[j]) sensorValue[j] = black_value[j];
      if (sensorValue[j] > white_value[j]) sensorValue[j] = white_value[j];
      sensorPID[j] = map(sensorValue[j], black_value[j], white_value[j], 0, 1000); // Normalize
    }
    temp = temp << 1;
    if (sensorValue[j] > compare_value[j]) {
      temp |= 0x01; // Set bit for line detection
    } else {
      temp &= 0xfe;
    }
    sensor = temp; // Update sensor state
  }

  // Calculate PID terms
  for (int j = 0; j < 8; j++) {
    avg += (long)(sensorPID[j]) * ((j) * 1000);
    sum += sensorPID[j];
  }
  i = (int)((avg / sum) - 3500); // Center error
  kp = 0.6; // Proportional gain
  iP = kp * i; // Proportional term
  iD = kd * (lastPos - i); // Derivative term
  iRet = (iP - iD); // Combined PID output
  if (iRet < -4000) iRet = 0; // Limit output
  servoPwm = iRet / 30; // Scale for motor control
  lastPos = i; // Store current position
}

// Generate beep for feedback
void beep(int timer) {
  digitalWrite(BUZZER, HIGH);
  delay(timer);
  digitalWrite(BUZZER, LOW);
}

// Check sensor state against mask
unsigned char sensorMask(unsigned char mask) {
  return (sensor & mask);
}

// Detect 90-degree turns (unused)
int check90() {
  if (sensorMask(0x0f)) return 1; // Right turn
  if (sensorMask(0xf0)) return -1; // Left turn
  return 0;
}
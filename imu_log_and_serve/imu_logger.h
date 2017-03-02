/* IMU data logger (imu_logger.h)
 *  
 * Connects to SD card and IMU and allows for logging IMU sensor readings
 * at specified intervals to that SD card.
 * 
 * Notes:
 *    Some parameters must be defined before this header file is imported,
 *    these parameters can be found in `imu_log_and_serve.ino`
 * 
 * Todo:
 *    Add Magnetometer readings to logs.
 *  
 * Based on:
 *    Datalogger.ino by Tom Igoe, from the `SD(esp8266)` example codes.
 *    Sensor example codes by Adafruit.
 * 
 * Authors:
 *    Peter Moran (Fall 2016)
 */
 
//////////// Initialize code ////////////
#include <Wire.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_BMP085_U.h>

// Sensors
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(20);
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

// SD card
String filename;

//////////// Initialize Hardware ////////////
void setup_sensor() {
  //////////// Set up sensors ////////////
  if(!accel.begin()) {
    /* There was a problem detecting the ADXL345 ... check your connections */
    DBG_OUTPUT_PORT.println("Ooops, no LSM303 detected ... Check your wiring!");
  }
  if(!gyro.begin()) {
    /* There was a problem detecting the L3GD20 ... check your connections */
    DBG_OUTPUT_PORT.println("Ooops, no L3GD20 detected ... Check your wiring!");
  }
  if(!bmp.begin()) {
    /* There was a problem detecting the BMP085 ... check your connections */
    DBG_OUTPUT_PORT.println("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
  }

  //////////// Set up SD card ////////////
  Serial.print("Initializing SD card ...");

  // See if the SD card is present and can be initialized:
  if (!SD.begin(CHIP_SELECT_PIN)) {
    Serial.println("Card failed, or not present");
    // Can't run. Stop program.
    return;
  }
  Serial.print("\nSD card found. Setting up log file ...");

  // Create a file to write to. The filename will be the "file prefix" followed by the lowest available integer.
  int i = 1;
  filename = FILE_PREFIX + String(i) + FILE_TYPE;
  while (true) {
    File f = SD.open(filename, FILE_READ);
    if (not f) {
      // File does not exist yet, continue.
      break;
    }
    else {
      // File exists already, we dont want to overwrite it. Try next integer.
      i += 1;
      filename = FILE_PREFIX + String(i) + FILE_TYPE;
    }
  }

  // Open the file.
  Serial.print("\nWriting data to ");
  Serial.println(filename);
  Serial.println("card initialized.");
  
  // Blink the number of times for the log file id
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // LED on
  delay(3000);
  digitalWrite(LED_BUILTIN, HIGH);  // LED off
  delay(1000);
  for (int j=1; j<=i; j++){
    digitalWrite(LED_BUILTIN, LOW); // LED on
    delay(300);
    digitalWrite(LED_BUILTIN, HIGH);  // LED off
    delay(200);
  }
}

void log_sensor() {
  /*  Reads IMU sensors, saves data, and logs to SD card at specific
   *  interval.
   *   
   *  Must be called regularly, e.g. in loop(), to work correctly.
   */
  // Get accelerometer data
  sensors_event_t accel_event;
  accel.getEvent(&accel_event);
  float ax = accel_event.acceleration.x;
  float ay = accel_event.acceleration.y;
  float az = accel_event.acceleration.z;

  // Get gyroscope data
  sensors_event_t gyro_event;
  gyro.getEvent(&gyro_event);
  float rax = gyro_event.gyro.x;
  float ray = gyro_event.gyro.y;
  float raz = gyro_event.gyro.z;

  // Get pressure data
  sensors_event_t bmp_event;
  bmp.getEvent(&bmp_event);
  float pres = bmp_event.pressure;

  // Get temperature
  float temperature;
  bmp.getTemperature(&temperature);

  // Make a string for assembling the data to log:
  String dataString = "";

  // Log data to SD
  dataString += String(millis());
  dataString += String(",");
  dataString += String(ax);
  dataString += String(",");
  dataString += String(ay);
  dataString += String(",");
  dataString += String(az);
  dataString += String(",");
  dataString += String(rax);
  dataString += String(",");
  dataString += String(ray);
  dataString += String(",");
  dataString += String(raz);
  dataString += String(",");
  dataString += String(pres);
  dataString += String(",");
  dataString += String(temperature);

  // If the file is available, write to it
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    // Append to file
    dataFile.println(dataString);
    dataFile.close();
    
    // Print to the serial port too
    Serial.println(dataString);
  }
  // If the file isn't open, pop up an error
  else {
    Serial.println("error opening file");
  }

  // Delay so that the requested loggig rate is achieved
  delay(LOG_PERIOD_MS);
}

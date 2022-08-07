/*
  accelerometer_and_gyroscope.ino

    Functions to initialize and read sensor date from a MPU6050 3-axis accelerometer and gyroscope
    using the Adafruit library ...
    Read more: https://github.com/dxcfl/fall-detection-with-sony-spresense#readme
    Git: https://github.com/dxcfl/fall-detection-with-sony-spresense
    (c) 2022 by dxcfl
*/

// Accelerometer and gyroscope readings from a MPU6050 sensor using the Adafruit library ...

// To test/use this sketch standalone: #define ACC_GYR_STANDALONE

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <math.h>
#include "debug2serial.h"

Adafruit_MPU6050 mpu;

void accelerometer_and_gyroscope_setup(void)
{
  DEBUG_SERIAL_PRINTLN("INFO: Initialising accelerometer ...");
  // Try to initialize MPU6050!
  DEBUG_SERIAL_PRINTLN("INFO: Initialising MPU6050 ...");
  if (!mpu.begin(MPU6050_I2CADDR_DEFAULT, &Wire1, 0))
  {
    DEBUG_SERIAL_PRINTLN("ERROR: Failed to detect MPU6050 sensor.");
    while (1)
    {
      delay(10);
    }
  }
  DEBUG_SERIAL_PRINTLN("INFO: MPU6050 initialized.");
}

void read_acceleration_and_rotation(float *accX, float *accY, float *accZ, float *gyroX, float *gyroY, float *gyroZ)
{
  /* Get new sensor event with the reading */
  sensors_event_t eventA, eventG, eventT;
  DEBUG_SERIAL_PRINTLN("INFO: Reading acceleration / rotation values from MPU6050 ...");
  mpu.getEvent(&eventA, &eventG, &eventT);

  DEBUG_SERIAL_PRINT("INFO: Acceleration X: ");
  DEBUG_SERIAL_PRINT(eventA.acceleration.x);
  DEBUG_SERIAL_PRINT(", Y: ");
  DEBUG_SERIAL_PRINT(eventA.acceleration.y);
  DEBUG_SERIAL_PRINT(", Z: ");
  DEBUG_SERIAL_PRINT(eventA.acceleration.z);
  DEBUG_SERIAL_PRINTLN(" m/s^2");
  DEBUG_SERIAL_PRINT("INFO: Rotation X: ");
  DEBUG_SERIAL_PRINT(eventG.gyro.x);
  DEBUG_SERIAL_PRINT(", Y: ");
  DEBUG_SERIAL_PRINT(eventG.gyro.y);
  DEBUG_SERIAL_PRINT(", Z: ");
  DEBUG_SERIAL_PRINT(eventG.gyro.z);
  DEBUG_SERIAL_PRINTLN(" rad/s");

  *accX = eventA.acceleration.x;
  *accY = eventA.acceleration.y;
  *accZ = eventA.acceleration.z;
  *gyroX = eventG.gyro.x;
  *gyroY = eventG.gyro.y;
  *gyroZ = eventG.gyro.z;
}

inline float total(float x, float y, float z)
{
  return sqrt(x * x + y * y + z * z);
}

#ifdef ACC_GYR_STANDALONE
void setup(void)
{
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  accelerometer_and_gyroscope_setup();
}

void loop(void)
{
  float accX, accY, accZ;
  float gyrX, gyrY, gyrZ;
  read_acceleration_and_rotation(&accX, &accY, &accZ, &gyrX, &gyrY, &gyrZ);
  Serial.print(accX);
  Serial.print(",");
  Serial.print(accY);
  Serial.print(",");
  Serial.print(accZ);
  Serial.print(",");
  Serial.print(gyrX);
  Serial.print(",");
  Serial.print(gyrY);
  Serial.print(",");
  Serial.print(gyrZ);
  Serial.print(",");
  Serial.print(total(accX, accY, accZ));
  Serial.print(",");
  Serial.println(total(gyrX, gyrY, gyrZ));
#ifdef DEBUG
  delay(500);
#endif
}
#endif

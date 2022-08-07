/*
  fall_detection.ino

    Simple example/proof-of-concept for a fall detection device featuring the
    Sony Spresense with Spresense LTE extension board and a a MPU6050 3-axis accelerometer
    and gyroscope ...
    Read more: https://github.com/dxcfl/fall-detection-with-sony-spresense#readme
    Git: https://github.com/dxcfl/fall-detection-with-sony-spresense
    (c) 2022 by dxcfl
*/

#define DEBUG
#include "debug2serial.h"

// include application settings
#include "app_settings.h"

/* Setup
 */
void setup(void)
{
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
#endif

  accelerometer_and_gyroscope_setup();
  lte_web_client_setup();
}

/* Loop
 */
void loop()
{
  float accX, accY, accZ;
  float gyrX, gyrY, gyrZ;
  bool fall_detected;

  read_acceleration_and_rotation(&accX, &accY, &accZ, &gyrX, &gyrY, &gyrZ);
  fall_detected = fall_detection(accX, accY, accZ, gyrX, gyrY, gyrZ);

  if (fall_detected)
  {
    DEBUG_SERIAL_PRINTLN("INFO: FALL DETECTED!!!");
    send_request();
#ifdef DEBUG
    DEBUG_SERIAL_PRINTLN("WARNING: Pausing execution for 10 seconds ...");
    delay(10000);
#endif
  }
}

/* Fall detection: Determine fall detection based on 3-axis acceleration and rotation
 */
bool fall_detection(float accX, float accY, float accZ, float gyrX, float gyrY, float gyrZ)
{
  bool freeFallTrigger, impactTrigger, orientationChangeTrigger;

  // Thresholds and interval
  const unsigned long detectionInterval = APP_FD_DETECTION_TIME_INTERVAL;
  const float freeFallThreshold = APP_FD_FREE_FALL_ACCELERATION_THRESHOLD;
  const float impactThreshold = APP_FD_IMPACT_ACCELERATION_THRESHOLD;
  const float orientationChangeThreshold = APP_FD_ORIENTATION_CHANGE_THRESHOLD;

  // Last detection time for each trigger
  static unsigned long timeFreeFall = 0;
  static unsigned long timeImpact = 0;
  static unsigned long timeOrientationChange = 0;

  unsigned long currentTime = millis();

  // Total acceleration and rotation for all three axes
  float totalAcc = total(accX, accY, accZ);
  float totalRot = total(gyrX, gyrY, gyrZ);

  DEBUG_SERIAL_PRINT("INFO: Total acceleration = ");
  DEBUG_SERIAL_PRINTLN(totalAcc);
  DEBUG_SERIAL_PRINT("INFO: Rotation magnitude = ");
  DEBUG_SERIAL_PRINTLN(totalRot);

  // Free fall detected: Total acceleration below defined threshold
  if (totalAcc <= freeFallThreshold)
  {
    timeFreeFall = currentTime;
  }

  // Impact detected: Total acceleration above defined threshold
  if (totalAcc >= impactThreshold)
  {
    timeImpact = currentTime;
  }

  // Orientation change detected: Total rotation above defined threshold
  if (totalRot >= orientationChangeThreshold)
  {
    timeOrientationChange = currentTime;
  }

  DEBUG_SERIAL_PRINT("INFO: Detection time (millis current, free fall, impact, orientation): ");
  DEBUG_SERIAL_PRINT(currentTime);
  DEBUG_SERIAL_PRINT(",");
  DEBUG_SERIAL_PRINT(timeFreeFall);
  DEBUG_SERIAL_PRINT(",");
  DEBUG_SERIAL_PRINT(timeImpact);
  DEBUG_SERIAL_PRINT(",");
  DEBUG_SERIAL_PRINT(timeOrientationChange);
  DEBUG_SERIAL_PRINTLN("");

  // Events triggered within defined time interval?
  freeFallTrigger = (currentTime - timeFreeFall < detectionInterval);
  impactTrigger = (currentTime - timeImpact < detectionInterval);
  orientationChangeTrigger = (currentTime - timeOrientationChange < detectionInterval);
  DEBUG_SERIAL_PRINT("INFO: Trigger (free fall, impact, orientation change): ");
  DEBUG_SERIAL_PRINT(freeFallTrigger);
  DEBUG_SERIAL_PRINT(",");
  DEBUG_SERIAL_PRINT(impactTrigger);
  DEBUG_SERIAL_PRINT(",");
  DEBUG_SERIAL_PRINT(orientationChangeTrigger);
  DEBUG_SERIAL_PRINTLN("");

  // Fall detected: All three triggers occured within defined time interval.
  return freeFallTrigger && impactTrigger && orientationChangeTrigger;
}

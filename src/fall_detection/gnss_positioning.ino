/* gnss:positioning.ino

    derived from

    gnss.ino - GNSS example application
    Copyright 2018 Sony Semiconductor Solutions Corporation

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/**
   @file gnss.ino
   @author Sony Semiconductor Solutions Corporation
   @brief GNSS example application
   @details Spresense has an built in GNSS receiver which supports GPS and other
            GNSS satellites. This skecth provides an example the GNSS operation.
            Simply upload the sketch, reset the board and check the USB serial
            output. After 3 seconds status information should start to appear.\n\n

            This example code is in the public domain.
*/

#include "debug2serial.h"

// include application settings
#include "app_settings.h"

/* include the GNSS library */
#include <GNSS.h>

#define STRING_BUFFER_SIZE 128 /**< %Buffer size */

#define RESTART_CYCLE (60 * 5 * 100000) /**< positioning test term */

static SpGnss Gnss; /**< SpGnss object */

/**
   @enum ParamSat
   @brief Satellite system
*/
enum ParamSat
{
  eSatGps,            /**< GPS                     World wide coverage  */
  eSatGlonass,        /**< GLONASS                 World wide coverage  */
  eSatGpsSbas,        /**< GPS+SBAS                North America        */
  eSatGpsGlonass,     /**< GPS+Glonass             World wide coverage  */
  eSatGpsBeidou,      /**< GPS+BeiDou              World wide coverage  */
  eSatGpsGalileo,     /**< GPS+Galileo             World wide coverage  */
  eSatGpsQz1c,        /**< GPS+QZSS_L1CA           East Asia & Oceania  */
  eSatGpsGlonassQz1c, /**< GPS+Glonass+QZSS_L1CA   East Asia & Oceania  */
  eSatGpsBeidouQz1c,  /**< GPS+BeiDou+QZSS_L1CA    East Asia & Oceania  */
  eSatGpsGalileoQz1c, /**< GPS+Galileo+QZSS_L1CA   East Asia & Oceania  */
  eSatGpsQz1cQz1S,    /**< GPS+QZSS_L1CA+QZSS_L1S  Japan                */
};

/* Set this parameter depending on your current region. */
static enum ParamSat satType = eSatGps;

const long gnss_update_cycle = APP_GNSS_POSITION_UPDATE_CYCLE;
int gnss_error_flag = 0;

/**
   @brief Activate GNSS device and start positioning.
*/
void gnss_setup()
{

#ifdef DEBUG
  /* Set Debug mode to Info */
  Gnss.setDebugMode(PrintInfo);
#endif

  /* Activate GNSS device */
  int result = Gnss.begin();

  if (result != 0)
  {
    DEBUG_SERIAL_PRINTLN("Gnss begin error!!");
    gnss_error_flag = 1;
  }
  else
  {
    /* Setup GNSS
        It is possible to setup up to two GNSS satellites systems.
        Depending on your location you can improve your accuracy by selecting different GNSS system than the GPS system.
        See: https://developer.sony.com/develop/spresense/developer-tools/get-started-using-nuttx/nuttx-developer-guide#_gnss
        for detailed information.
    */
    switch (satType)
    {
    case eSatGps:
      Gnss.select(GPS);
      break;

    case eSatGpsSbas:
      Gnss.select(GPS);
      Gnss.select(SBAS);
      break;

    case eSatGlonass:
      Gnss.select(GLONASS);
      break;

    case eSatGpsGlonass:
      Gnss.select(GPS);
      Gnss.select(GLONASS);
      break;

    case eSatGpsBeidou:
      Gnss.select(GPS);
      Gnss.select(BEIDOU);
      break;

    case eSatGpsGalileo:
      Gnss.select(GPS);
      Gnss.select(GALILEO);
      break;

    case eSatGpsQz1c:
      Gnss.select(GPS);
      Gnss.select(QZ_L1CA);
      break;

    case eSatGpsQz1cQz1S:
      Gnss.select(GPS);
      Gnss.select(QZ_L1CA);
      Gnss.select(QZ_L1S);
      break;

    case eSatGpsBeidouQz1c:
      Gnss.select(GPS);
      Gnss.select(BEIDOU);
      Gnss.select(QZ_L1CA);
      break;

    case eSatGpsGalileoQz1c:
      Gnss.select(GPS);
      Gnss.select(GALILEO);
      Gnss.select(QZ_L1CA);
      break;

    case eSatGpsGlonassQz1c:
    default:
      Gnss.select(GPS);
      Gnss.select(GLONASS);
      Gnss.select(QZ_L1CA);
      break;
    }

    /* Start positioning */
    result = Gnss.start(COLD_START);
    if (result != 0)
    {
      DEBUG_SERIAL_PRINTLN("ERROR: Gnss start error!!");
      gnss_error_flag = 1;
    }
    else
    {
      DEBUG_SERIAL_PRINTLN("INFO: Gnss setup OK");
    }
  }
}

bool gnss_update(double *latitude, double *longitude, double *altitude)
{
  static long lastUpdateMillis = 0;
  static int LoopCount = 0;
  boolean update = false;

  if (lastUpdateMillis > 0 && lastUpdateMillis + gnss_update_cycle > millis())
  {
    return false;
  }

  /* Check update. */
  if (Gnss.isUpdate())
  {
    /* Get NaviData. */
    SpNavData NavData;
    Gnss.getNavData(&NavData);

#ifdef DEBUG
    char StringBuffer[STRING_BUFFER_SIZE];
    /* print time */
    snprintf(StringBuffer, STRING_BUFFER_SIZE, "%04d/%02d/%02d ", NavData.time.year, NavData.time.month, NavData.time.day);
    DEBUG_SERIAL_PRINT("INFO: ");
    DEBUG_SERIAL_PRINT(StringBuffer);
    snprintf(StringBuffer, STRING_BUFFER_SIZE, "%02d:%02d:%02d.%06ld, ", NavData.time.hour, NavData.time.minute, NavData.time.sec, NavData.time.usec);
    DEBUG_SERIAL_PRINT(StringBuffer);
    /* print satellites count */
    snprintf(StringBuffer, STRING_BUFFER_SIZE, "numSat:%2d, ", NavData.numSatellites);
    DEBUG_SERIAL_PRINT(StringBuffer);
    /* print position data */
    if (NavData.posFixMode == FixInvalid)
    {
      DEBUG_SERIAL_PRINT("No-Fix, ");
    }
    else
    {
      DEBUG_SERIAL_PRINT("Fix, ");
    }
    if (NavData.posDataExist == 0)
    {
      DEBUG_SERIAL_PRINT("No Position");
    }
    else
    {
      DEBUG_SERIAL_PRINT("Lat=");
      DEBUG_SERIAL_PRINT(NavData.latitude, 6);
      DEBUG_SERIAL_PRINT(", Lon=");
      DEBUG_SERIAL_PRINT(NavData.longitude, 6);
    }
    DEBUG_SERIAL_PRINTLN("");
#endif

    /* Check if position data is available  */
    if (NavData.posFixMode != FixInvalid && NavData.posDataExist != 0)
    {
      *latitude = NavData.latitude;
      *longitude = NavData.longitude;
      *altitude = NavData.altitude;

      DEBUG_SERIAL_PRINT("INFO: GNSS position data retrieved - Lat=");
      DEBUG_SERIAL_PRINT(*latitude, 6);
      DEBUG_SERIAL_PRINT(", Lon=");
      DEBUG_SERIAL_PRINT(*longitude, 6);
      DEBUG_SERIAL_PRINTLN("");

      lastUpdateMillis = millis();
      gnss_error_flag = 0;
      update = true;
    }
    else
    {
      /* No position */
      DEBUG_SERIAL_PRINTLN("WARNING: No position");
    }
  }
  else
  {
    /* No update. */
    DEBUG_SERIAL_PRINTLN("WARNING: Gnss data not updated");
  }

  /* Check loop count. */
  LoopCount++;
  if (LoopCount >= RESTART_CYCLE)
  {
    DEBUG_SERIAL_PRINTLN("WARNING: Restarting Gnss ...");
    /* Restart GNSS. */
    if (Gnss.stop() != 0)
    {
      DEBUG_SERIAL_PRINTLN("ERROR: Gnss stop error!!");
      gnss_error_flag = 1;
    }
    else if (Gnss.end() != 0)
    {
      DEBUG_SERIAL_PRINTLN("ERROR: Gnss end error!!");
      gnss_error_flag = 1;
    }
    else
    {
      DEBUG_SERIAL_PRINTLN("INFO: Gnss stop OK.");
    }

    if (Gnss.begin() != 0)
    {
      DEBUG_SERIAL_PRINTLN("ERROR: Gnss begin error!!");
      gnss_error_flag = 1;
    }
    else if (Gnss.start(HOT_START) != 0)
    {
      DEBUG_SERIAL_PRINTLN("ERROR: Gnss start error!!");
      gnss_error_flag = 1;
    }
    else
    {
      DEBUG_SERIAL_PRINTLN("INFO: Gnss restart OK.");
    }
    LoopCount = 0;
  }

  return update;
}

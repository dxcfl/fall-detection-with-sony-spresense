/*
  lte_web_client.ino

  derived from

    LteHttpSecureClient.ino - Example for secure HTTP client using LTE
    Copyright 2019, 2021, 2022 Sony Semiconductor Solutions Corporation

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

    This sketch connects to a website via LTE.
    In this example, an HTTP GET request is sent to https://httpin.org/get,
    and an HTTP POST request is sent to https://httpin.org/post.
*/

#include "debug2serial.h"

// include application settings
#include "app_settings.h"

// libraries
#include <ArduinoHttpClient.h>
#include <RTC.h>
#include <SDHCI.h>
#include <LTE.h>

// URL, path & port (for example: arduino.cc)
char server[] = APP_SEND_MESSAGE_HOST;
char getPath[] = APP_SEND_MESSAGE_PATH;
int port = APP_SEND_MESSAGE_PORT; 

// initialize the library instance
LTE lteAccess;
LTETLSClient tlsClient;
HttpClient client = HttpClient(tlsClient, server, port);
SDClass theSD;

/* LTE web client setup
 */
void lte_web_client_setup()
{
  char apn[LTE_NET_APN_MAXLEN] = APP_LTE_APN;
  LTENetworkAuthType authtype = APP_LTE_AUTH_TYPE;
  char user_name[LTE_NET_USER_MAXLEN] = APP_LTE_USER_NAME;
  char password[LTE_NET_PASSWORD_MAXLEN] = APP_LTE_PASSWORD;

  DEBUG_SERIAL_PRINTLN("INFO: Starting LTE web client.");

  /* "Stop" if APN is empty */
  if (strlen(APP_LTE_APN) == 0)
  {
    DEBUG_SERIAL_PRINTLN("ERROR: This sketch doesn't have a APN information.");
    while (1)
    {
      delay(10);
    }
  }
  else
  {
    DEBUG_SERIAL_PRINTLN("INFO: =========== APN information ===========");
    DEBUG_SERIAL_PRINT("INFO: Access Point Name  : ");
    DEBUG_SERIAL_PRINTLN(apn);
    DEBUG_SERIAL_PRINT("INFO: Authentication Type: ");
    DEBUG_SERIAL_PRINTLN(authtype == LTE_NET_AUTHTYPE_CHAP ? "CHAP" : authtype == LTE_NET_AUTHTYPE_NONE ? "NONE"
                                                                                                        : "PAP");
    if (authtype != LTE_NET_AUTHTYPE_NONE)
    {
      DEBUG_SERIAL_PRINT("INFO: User Name          : ");
      DEBUG_SERIAL_PRINTLN(user_name);
      DEBUG_SERIAL_PRINT("INFO: Password           : ");
      DEBUG_SERIAL_PRINTLN(password);
    }
  }

  while (true)
  {
    /* Power on the modem and Enable the radio function. */
    if (lteAccess.begin() != LTE_SEARCHING)
    {
      DEBUG_SERIAL_PRINTLN("ERROR: Could not transition to LTE_SEARCHING.");
      DEBUG_SERIAL_PRINTLN("ERROR: Please check the status of the LTE board.");
      for (;;)
      {
        sleep(1);
      }
    }

    /* The connection process to the APN will start.
       If the synchronous parameter is false,
       the return value will be returned when the connection process is started.
    */
    if (lteAccess.attach(APP_LTE_RAT,
                         apn,
                         user_name,
                         password,
                         authtype,
                         APP_LTE_IP_TYPE) == LTE_READY)
    {
      DEBUG_SERIAL_PRINTLN("INFO: attach succeeded.");
      break;
    }

    /* If the following logs occur frequently, one of the following might be a cause:
       - APN settings are incorrect
       - SIM is not inserted correctly
       - If you have specified LTE_NET_RAT_NBIOT for APP_LTE_RAT,
         your LTE board may not support it.
       - Rejected from LTE network
    */
    DEBUG_SERIAL_PRINTLN("ERROR: An error has occurred. Shutdown and retry the network attach process after 1 second.");
    lteAccess.shutdown();
    sleep(1);
  }

  // Set local time (not UTC) obtained from the network to RTC.
  DEBUG_SERIAL_PRINTLN("INFO: Obtaining time from network to RTC ...");
  RTC.begin();
  unsigned long currentTime;
  while (0 == (currentTime = lteAccess.getTime()))
  {
    sleep(1);
  }
  RtcTime rtc(currentTime);
  DEBUG_SERIAL_PRINTF("%04d/%02d/%02d %02d:%02d:%02d\n",
                      rtc.year(), rtc.month(), rtc.day(),
                      rtc.hour(), rtc.minute(), rtc.second());
  RTC.setTime(rtc);

  /* Initialize SD */
  DEBUG_SERIAL_PRINTLN("INFO: Mounting SD card ...");
  while (!theSD.begin())
  {
    ; /* wait until SD card is mounted. */
  }
}

/* Send HTTP(S) GET request
 */
void send_request()
{
  // Set certifications via a file on the SD card before connecting to the server
  DEBUG_SERIAL_PRINTLN("INFO: Reading root CA ...");
  File rootCertsFile = theSD.open(APP_ROOTCA_FILE, FILE_READ);
  tlsClient.setCACert(rootCertsFile, rootCertsFile.available());
  rootCertsFile.close();

  DEBUG_SERIAL_PRINT("INFO: Sending request to ");
  DEBUG_SERIAL_PRINT(server);
  DEBUG_SERIAL_PRINT(":");
  DEBUG_SERIAL_PRINT(port);
  DEBUG_SERIAL_PRINTLN(" ...");

  // HTTP GET method
  DEBUG_SERIAL_PRINT("INFO: Sending HTTP GET request for ");
  DEBUG_SERIAL_PRINTLN(getPath);
  client.get(getPath);

  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  DEBUG_SERIAL_PRINT("INFO: Status code: ");
  DEBUG_SERIAL_PRINTLN(statusCode);
  DEBUG_SERIAL_PRINT("INFO: Response: ");
  DEBUG_SERIAL_PRINTLN(response);
}

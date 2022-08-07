/*
  app_setting.h

    Simple example/proof-of-concept for a fall detection device featuring the
    Sony Spresense with Spresense LTE extension board and a a MPU6050 3-axis accelerometer
    and gyroscope ...

    Application configuration

    Read more: https://github.com/dxcfl/fall-detection-with-sony-spresense#readme
    Git: https://github.com/dxcfl/fall-detection-with-sony-spresense
    (c) 2022 by dxcfl
*/

#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

/* Fall detection parameters **************************************************
*/

#define APP_FD_DETECTION_TIME_INTERVAL 1000 // Fall detection interval in ms
#define APP_FD_FREE_FALL_ACCELERATION_THRESHOLD 2 // m/s^2
#define APP_FD_IMPACT_ACCELERATION_THRESHOLD 20 // m/s^2
#define APP_FD_ORIENTATION_CHANGE_THRESHOLD 5 // rad/s

/* Messaging parameters *******************************************************
*/
 #define APP_SEND_MESSAGE_HOST "api.callmebot.com" 
 #define APP_SEND_MESSAGE_PORT 443
 #define APP_SEND_MESSAGE_PATH "/signal/send.php?phone=+491631737743&apikey=123456&text="
 #define APP_ROOTCA_FILE "isrg_root_x1_dst_root_ca_x3_.cer"
 
/* LTE web client settings ****************************************************
*/

/* Settings for GNSS positioning **********************************************
*/

#define APP_USE_GNSS 1

#define APP_GNSS_POSITION_UPDATE_CYCLE 3000 // ms 

// APN name
#define APP_LTE_APN "iot.truphone.com" // replace your APN

/* APN authentication settings
 * Ignore these parameters when setting LTE_NET_AUTHTYPE_NONE.
 */
#define APP_LTE_USER_NAME "" // replace with your username
#define APP_LTE_PASSWORD  "" // replace with your password

// APN IP type
#define APP_LTE_IP_TYPE (LTE_NET_IPTYPE_V4V6) // IP : IPv4v6
// #define APP_LTE_IP_TYPE (LTE_NET_IPTYPE_V4) // IP : IPv4
// #define APP_LTE_IP_TYPE (LTE_NET_IPTYPE_V6) // IP : IPv6

// APN authentication type
// #define APP_LTE_AUTH_TYPE (LTE_NET_AUTHTYPE_CHAP) // Authentication : CHAP
// #define APP_LTE_AUTH_TYPE (LTE_NET_AUTHTYPE_PAP) // Authentication : PAP
#define APP_LTE_AUTH_TYPE (LTE_NET_AUTHTYPE_NONE) // Authentication : NONE

/* RAT to use
 * Refer to the cellular carriers information
 * to find out which RAT your SIM supports.
 * The RAT set on the modem can be checked with LTEModemVerification::getRAT().
 */

#define APP_LTE_RAT (LTE_NET_RAT_CATM) // RAT : LTE-M (LTE Cat-M1)
// #define APP_LTE_RAT (LTE_NET_RAT_NBIOT) // RAT : NB-IoT




#endif

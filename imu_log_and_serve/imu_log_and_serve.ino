/* IMU Logging and Wifi Download manager (imu_log_and_serve.ino)
 *  
 * Runs on startup and decides which mode (data logging or data 
 * serving) the ESP8266 should switch to, and then executes it.
 * 
 * The actual act of logging and sending out data is handled by 
 * `SDWebServer.h` and `imu_logger.h`.
 * 
 * Authors:
 *    Peter Moran (Fall 2016)
 */

///////////// User-defined parameters /////////////
// Wifi Server
const char* ssid = "TP-LINK_0084";  // Wifi SSID the ESP8266 will attempt to connect to on startup
const char* password = "not_used";  // Wifi password. Can be anything if password is not needed.
const char* host = "esp8266sd";     // Hostname the ESP8266 will use to identify itself.

// Data Logging
#define SEARCH_SECS 10            // How many seconds the ESP8266 will look for above SSID
#define LOG_PERIOD_MS 1000        // The period to wait between each measurement from 10-DOF sensor.
#define FILE_PREFIX "10dof_"      // Beginning of each log's file name to be followed by the lowest unused integer. Note: no more than 8 characters can be used in entire name.
#define FILE_TYPE ".csv"          // File type extension to give the log file.
const int CHIP_SELECT_PIN = 14;   // ESP8266 pin the chip select pin on the SD card is connected to.

///////////// Initialize code /////////////////////
// Hardware definitions
#define DBG_OUTPUT_PORT Serial
boolean server_running;

// Import functions (must be done after parameters are defined)
#include "SDWebServer.h"
#include "imu_logger.h"

///////////// Initialize hardware /////////////////
void setup(void){
  // Set up serial
  DBG_OUTPUT_PORT.begin(115200);
  DBG_OUTPUT_PORT.setDebugOutput(true);
  DBG_OUTPUT_PORT.print("\n");
  
  // Try to connect to wifi and create server
  server_running = SD_server_begin();

  // If we fail to connect to server, start the IMU
  if (!server_running) {
    setup_sensor();
  }
}

///////////// Log data or run server ///////////////////
void loop(void){
  if(server_running) {
    run_server();
  }
  else {
    log_sensor();
  }
}



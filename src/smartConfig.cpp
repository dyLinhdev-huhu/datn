#include "smartConfig.h"
#include "log.h"
#include "nodeId.h"

ESP8266WebServer esp8Server(80);

int i = 0;
int statusCode;
String ssid_AP = "esp8266-dust";
const char* password_AP = "dyLinhdev";
String st;
String content;
String _user_SSID = "";
String _user_PASSWORD = "";

#define NODE_NUM	NODE_1

void smartConfig_Init()
{
	WiFi.disconnect();
	EEPROM.begin(512);
	delay(10);

	logi("Start SConfig");
	getUserWifi();

	WiFi.begin(_user_SSID.c_str(), _user_PASSWORD.c_str());

	if (testWifi())
	{
		logi("Wifi connected");
		logi("IP Address: %d", WiFi.localIP());
		// esp8Server.send(200,"text/plain","Hello World"); 
		// esp8Server.begin();
		return;
	}
	else
	{
		logi("Connecting Status Negative");
		logi("Turning the HotSpot On");
		setupAP();
	}
}

bool testWifi(void)
{
	int c = 0;
	//logi("Waiting for Wifi to connect");
	while ( c < 20 ) {
		if (WiFi.status() == WL_CONNECTED)
		{
			return true;
		}
		delay(500);
		logis(".");
		c++;
	}
	logis("\r\n");
	logi("Connect timed out, opening AP");
	return false;
}

void setupAP(void)
{
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(100);
	int n = WiFi.scanNetworks();
	logi("scan done");
	if (n == 0)
		logi("no networks found");
	else
	{
		logi(" networks found : %d", n);
		uint8_t *packet = (uint8_t*)malloc(50);
		for (int i = 0; i < n; ++i)
		{
			// Print SSID and RSSI for each network found
			memset(packet, 0, 50);
			sprintf((char*)packet, "%d : %s ( %d )", i+1, WiFi.SSID(i), WiFi.RSSI(i));
			logi("%s", packet);
			//logi((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
			delay(10);
		}
		free(packet);
	}
	logi("");
	st = "<ol>";
	for (int i = 0; i < n; ++i)
	{
	// Print SSID and RSSI for each network found
	st += "<li>";
	st += WiFi.SSID(i);
	st += " (";
	st += WiFi.RSSI(i);

	st += ")";
	//st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
	st += "</li>";
	}
	st += "</ol>";
	delay(100);
	ssid_AP += NODE_NUM;
	WiFi.softAP(ssid_AP.c_str(), password_AP);
	launchWeb();

}

void getUserWifi(){

	logi("Reading EEPROM ssid");


	for (int i = 0; i < 32; ++i)
	{
		_user_SSID += char(EEPROM.read(i));
	}
	Serial.println("SSID: " + _user_SSID);
	logi("Reading EEPROM pass");

	for (int i = 32; i < 96; ++i)
	{
		_user_PASSWORD += char(EEPROM.read(i));
	}
	Serial.println("PASS: " + _user_PASSWORD);

}

void createWebServer()
{
  {
    /* ---------------Main Page -----------------*/
    esp8Server.on("/", []() {

      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html>Welcome to Wifi Credentials Update page";
      content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
      content += ipStr;
      content += "<br><br><label style = \"margin-left : 23px\" > WIFI AVAILABLE </lable>";
      content += "<p>";
      content += st;
      content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32 style = \"margin-left : 51px\"> <br>";
      content += "<label>PASSWORD: </label><input name='pass' length=64> <br>";
      content += "<input type='submit'></form>";
      content += "</html>";
      esp8Server.send(200, "text/html", content);
    });

    /* ---------------Setting Page -----------------
     
     ** Refresh wifi available.
     ** Back and reload page to perform new available wifi.
    
    */
    esp8Server.on("/scan", []() {
      setupAP();
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);

      content = "<!DOCTYPE HTML>\r\n<html>go back";
      esp8Server.send(200, "text/html", content);
    });

    /* ---------------Setting Page -----------------
     
     ** Reading User's SSID and PASSWORD submitted from Webserver.
    
    */
    esp8Server.on("/setting", []() {
      String qsid = esp8Server.arg("ssid");
      String qpass = esp8Server.arg("pass");
      if (qsid.length() > 0 && qpass.length() > 0) {
        logi("clearing eeprom");
        for (int i = 0; i < 96; ++i) {
          EEPROM.write(i, 0);
        }
        logi("%s",qsid);
        logi("");
        logi("%s",qpass);
        logi("");

        logi("writing eeprom ssid:");
        for (unsigned int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          logi("Wrote : %c", qsid[i]);
        }
        logi("writing eeprom pass:");
        for (unsigned int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          logi("Wrote : %c",qpass[i]);
        }
        EEPROM.commit();

        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        ESP.restart();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        logi("Sending 404");
      }
      esp8Server.sendHeader("Access-Control-Allow-Origin", "*");
      esp8Server.send(statusCode, "application/json", content);

    });
  }
}

void launchWeb()
{
	logi("");
	if (WiFi.status() == WL_CONNECTED)
		logi("WiFi connected");
	Serial.print("Local IP: ");
	Serial.println(WiFi.localIP());
	Serial.print("SoftAP IP: ");
	Serial.println(WiFi.softAPIP());
	createWebServer();
	// Start the server
	esp8Server.begin();
	logi("Server started");
}
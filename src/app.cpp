#include "app.h"
#include "log.h"
#include "smartConfig.h"
#include <ESP8266WiFi.h>
#include "PubSubClient.h"
#include <String>
#include "nodeId.h"
// using namespace std;
#define TOPIC "datn/dyLinh_dev/dustSensor/data"
#define NODE_SENSOR NODE_1
#define enaTopic "datn/dyLinh_dev/dustSensor/enable"
#define hsoTopic "datn/dyLinh_dev/dustSensor/heso"
#define stimeTopic "datn/dyLinh_dev/dustSensor/stime"

WiFiClient espClient;
PubSubClient mqtt(espClient);
extern ESP8266WebServer esp8Server;
static const char* mqtt_server = "broker.hivemq.com";
int startFlag = 0;
unsigned long logTime;
#define HS  0.002264
float heso = HS;
unsigned long sampletime_ms = 10000;

void onDataMqtt(char* topic, byte* payload, unsigned int length);

app::app()
{
    this->starttime = 0;
    this->duration = 0;
    // this->sampletime_ms = 10000;
    this->lowpulseoccupancy = 0;
    this->ratio = 0;
    this->concentration = 0;
}

void app::initHardware()
{
    log_init();
    pinMode(LED_PIN, OUTPUT);
    pinMode(SENSOR_PIN,INPUT);
}

void app::initClient()
{
    smartConfig_Init();
    mqtt.setServer(mqtt_server, 1883);
    mqtt.setCallback(onDataMqtt);

    mqtt.subscribe(TOPIC);
    mqtt.subscribe(enaTopic);
    mqtt.subscribe(stimeTopic);
}

void app::clientLoop()
{
    mqtt.loop();
    esp8Server.handleClient();

    if(!mqtt.connected()){
        app::reConnect();
    }
    // else{
    //     if(millis() - logTime >= 5000)
    //     {
    //         logi("*******");
    //         logTime = millis();
    //     }
    // }
}

void app::reConnect(void)
{
	String clientID = "esp8266-src-";
	clientID += String(random(0xffff), HEX);

	logi("wifi satus : %d", WiFi.status());

	if(mqtt.connect(clientID.c_str()))
	{
		logi("Mqtt connected !!!");
		mqtt.subscribe(TOPIC);
        mqtt.subscribe(enaTopic);
        mqtt.subscribe(stimeTopic);

		// mqtt.publish(TOPIC, "Hello");
		mqtt.setCallback(onDataMqtt);
	}
	else
	{
		logw("reconnect !!!");
		logi("state : %d", mqtt.state());
		delay(1000);
	}
}

void app::readSensor()
{
    duration = pulseIn(SENSOR_PIN, LOW);
    lowpulseoccupancy = lowpulseoccupancy + duration;
}



void app::uploadData()
{
    if ((millis()- starttime ) > sampletime_ms)
    {
        // logi("sampletime_ms : %d", sampletime_ms);
        ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
        concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
        float conTemp = concentration;
        concentration = (concentration/0.283)*heso;
        String str = "P10 : " + String(concentration);
        // String str = NODE_SENSOR + String(lowpulseoccupancy) + " | " + String(ratio) + " | " + String(concentration) + ";";
        Serial.println(str);
        // Serial.print(lowpulseoccupancy);
        // Serial.print(",");
        // Serial.print(ratio);
        // Serial.print(",");
        // Serial.println(concentration);
        // mqtt.publish(TOPIC, str.c_str());
        lowpulseoccupancy = 0;
        starttime = millis();
    }
}

// void app::setStartTime(unsigned long stime)
// {
//     this->starttime = stime;
// }

void onDataMqtt(char* topic, byte* payload, unsigned int length)
{
	// logi("payload : %s", payload);
    if(strstr(topic, enaTopic) != NULL)
    {
        if(strstr((char*)payload, "START"))
        {
            startFlag = 1;
        }
        if(strstr((char*)payload, "STOP"))
        {
            startFlag = 0;
        }
        
    }

    if(strstr(topic, hsoTopic) != NULL)
    {
        float hso = atof((char*)payload);
        heso = hso;
    }

    if(strstr(topic, stimeTopic) != NULL)
    {
        unsigned long stTemp = atoi((char*)payload);
        sampletime_ms = stTemp;
        // logi("sampletime_ms : %d", sampletime_ms);
    }
    // logi("startFlag : %d", startFlag);
}
#include "app.h"
#include "log.h"
#include "smartConfig.h"
#include <ESP8266WiFi.h>
#include "PubSubClient.h"
#include <String>
#include "nodeId.h"
#include "config_ID.h"
// using namespace std;

//**************  TOPIC    ******************
#define dataTopic       "datn/dyLinh_dev/dustSensor/data"
#define enaTopic        "datn/dyLinh_dev/dustSensor/enable"
#define hsonTopic        "datn/dyLinh_dev/dustSensor/hesonhan"
#define hsocTopic        "datn/dyLinh_dev/dustSensor/hesocong"
#define stimeTopic      "datn/dyLinh_dev/dustSensor/stime"

//**************  TOPIC    ******************

//**************  ID NODE    ******************


//**************  ID NODE    ******************

 
WiFiClient espClient;
PubSubClient mqtt(espClient);
extern ESP8266WebServer esp8Server;
static const char* mqtt_server = "broker.hivemq.com";
int startFlag = 0;
unsigned long logTime;

#define HSN  0.002264
#define HSC  18.03

//**************  VARIABLE    ******************
float hesonhan = HSN;
float hesocong = HSC;
unsigned long sampletime_ms = 10000;

//**************  VARIABLE    ******************

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
    digitalWrite(LED_PIN, LOW);
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(LED_PIN, HIGH);
}

static void subTopic()
{
    // mqtt.subscribe(dataTopic);
    mqtt.subscribe(enaTopic);
    mqtt.subscribe(stimeTopic);
    mqtt.subscribe(hsocTopic);
    mqtt.subscribe(hsonTopic);
}

void app::initClient()
{
    smartConfig_Init();
    mqtt.setServer(mqtt_server, 1883);
    mqtt.setCallback(onDataMqtt);
    subTopic();

}

void app::clientLoop()
{
    mqtt.loop();
    esp8Server.handleClient();

    if(!mqtt.connected()){
        app::reConnect();
        digitalWrite(LED_PIN, HIGH);
    }
    else{
        digitalWrite(LED_PIN, LOW);
    }
}

void app::reConnect(void)
{
	String clientID = "esp8266-src-";
	clientID += String(random(0xffff), HEX);

	logi("wifi satus : %d", WiFi.status());

	if(mqtt.connect(clientID.c_str()))
	{
		logi("Mqtt connected !!!");
        subTopic();

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

float arrConcen[10] = {0};
int indexConcen = 0;

static float calib(float* arrCon, int hsTB, float hscCalib)
{
    float conAvg = 0;
    for(int i = 0; i < hsTB; i++)
    {
        conAvg += arrCon[i];
    }

    return (conAvg/10 + hscCalib);
}

void app::uploadData()
{
    if ((millis()- starttime ) > sampletime_ms)
    {
        // logi("sampletime_ms : %d", sampletime_ms);
        ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
        concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
        // float conTemp = concentration;
        concentration = (concentration/0.283)*hesonhan;
        arrConcen[indexConcen] = concentration;
        indexConcen++;
        if(indexConcen >= 10)
        {
            indexConcen = 0;
        }

        float conTemp = calib(arrConcen, 10, hesocong);
        String str = ID_NODE + String(conTemp) + ";";
        // String str = NODE_SENSOR + String(lowpulseoccupancy) + " | " + String(ratio) + " | " + String(concentration) + ";";
        Serial.println(str);
        mqtt.publish(dataTopic, str.c_str());
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
    if((strstr((char*) payload, ID_NODE) != NULL) || (strstr((char*) payload, "*;") != NULL))
    {
        char* payloadTemp = ((char*) payload + 2);
        logi("payloadTemp : %s", payloadTemp);

        if(strstr(topic, enaTopic) != NULL)
        {
            if(strstr((char*)payloadTemp, "START"))
            {
                startFlag = 1;
            }
            if(strstr((char*)payloadTemp, "STOP"))
            {
                startFlag = 0;
            }
            
        }

        if(strstr(topic, hsonTopic) != NULL)
        {
            float hson = atof((char*)payloadTemp);
            hesonhan = hson;
            logi("hesonhan : %f", hesonhan);
        }

        if(strstr(topic, hsocTopic) != NULL)
        {
            float hsoc = atof((char*)payloadTemp);
            hesocong = hsoc;
            logi("hesocong : %d", hesocong);
        }

        if(strstr(topic, stimeTopic) != NULL)
        {
            unsigned long stTemp = atoi((char*)payloadTemp);
            sampletime_ms = stTemp;
            logi("sampletime_ms : %d", sampletime_ms);
        }
        // logi("startFlag : %d", startFlag);
    }

}
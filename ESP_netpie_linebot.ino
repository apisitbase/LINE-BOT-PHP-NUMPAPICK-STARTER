#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include "DHT.h"              // library สำหรับอ่านค่า DHT Sensor

//const char* ssid     = "AndroidAP";                  // ชื่อ ssid
//const char* password = "base1234";              // รหัสผ่าน wifi
const char* ssid     = "HUAWEI Mate 10 Pro";                  // ชื่อ ssid
const char* password = "75117411";   
#define APPID   "wait"                              // ให้แทนที่ด้วย AppID
#define KEY     "TQcw7EnGTmKr9sX"                                // ให้แทนที่ด้วย Key
#define SECRET  "Uw4TjSkL7gzYKCPyVyMYgnJC9"                             // ให้แทนที่ด้วย Secret
#define ALIAS   "esp8266"                              // ชื่ออุปกรณ์
//#define ALIAS  "FBwait"
//#def ine neighbor "neighbor"                             // ชื่ออุปกรณ์ที่ต้องการส่งข้อความไปให้
#define topicPublish "/dht/" ALIAS                        // topic ที่ต้องการ publish ส่งข้อความ

#define SWITCHPIN 4      // Switch pin
#define LEDPIN 5         // LED pin
#define DHTPIN 12         // GPIO2 ขาที่ต่อเข้ากับขา DATA(OUT) ของ DHT
#define DHTTYPE DHT22     // e.g. DHT11, DHT21, DHT22
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastTimeDHT = 0;
float humid = 0;
float temp = 0;
int stateLED = 0;
bool stateChange = false;

WiFiClient client;

int timer = 0;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) { // 
    Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    Serial.println((char *)msg);

    // สถานะ LED บน NodeMCU ที่แสดงผล จะติดก็ต่อเมื่อสั่ง LOW 
    // แต่ถ้าเป็น LED ที่ต่อแยก จะต้องสั่งเป็น HIGH 
    if(*(char *)msg == '1'){
        stateLED = 1;
        digitalWrite(LEDPIN, HIGH); // LED on
        //microgear.chat(neighbor,"1");
    }else{
        stateLED = 0;
        digitalWrite(LEDPIN, LOW); // LED off
        //microgear.chat(neighbor,"0");
    }
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    microgear.setAlias(ALIAS);
}


void setup() {
    // กำหนดฟังก์ชั้นสำหรับรับ event callback 
    microgear.on(MESSAGE,onMsghandler);
    microgear.on(CONNECTED,onConnected);

    // กำหนด baud rate สำหรับการสื่อสาร
    Serial.begin(115200);
    Serial.println("Starting...");

    dht.begin(); // setup ตัวแปรสำหรับอ่านค่า DHT Sensor

    // กำหนดชนิดของ PIN (ขาI/O) เช่น INPUT, OUTPUT เป็นต้น
    pinMode(LEDPIN, OUTPUT);          // LED pin mode กำหนดค่า
    stateLED = digitalRead(LEDPIN);
    pinMode(SWITCHPIN, INPUT);        // Switch pin mode รับค่า

    // เชื่อมต่อ wifi
    if (WiFi.begin(ssid, password)) {
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.print(".");
        }
    }

    
    microgear.init(KEY,SECRET,ALIAS);   // กำหนดค่าตันแปรเริ่มต้นให้กับ microgear
    microgear.connect(APPID);           // ฟังก์ชั่นสำหรับเชื่อมต่อ NETPIE
}

void loop() {
    if (microgear.connected()) { // microgear.connected() เป็นฟังก์ชั่นสำหรับตรวจสอบสถานะการเชื่อมต่อ
        // microgear.loop() เป็นฟังก์ชั่นสำหรับทวนสถานะการเชื่อมต่อกับ NETPIE (จำเป็นต้องมีใช้ loop)
        microgear.loop();
        timer = 0;
        int state = digitalRead(SWITCHPIN);
        if(state==LOW && !stateChange){
          stateChange = true;
        }

        if(state==HIGH && stateChange) {
          stateChange = false;
          stateLED = !stateLED;
          digitalWrite(LEDPIN, stateLED);
//          microgear.chat(neighbor,stateLED);
        }
        
        if(millis()-lastTimeDHT > 1000){
          lastTimeDHT = millis();
          
          float h = dht.readHumidity();     // อ่านค่าความชื้น
          float t = dht.readTemperature();  // อ่านค่าอุณหภูมิ
          
          Serial.print("Humidity: ");
          Serial.print(h);
          Serial.print(" %RH , ");
          Serial.print("Temperature: ");
          Serial.print(t);
          Serial.println(" *C ");

          // ตรวจสอบค่า humid และ temp เป็นตัวเลขหรือไม่
          if (isnan(h) || isnan(t)) {
            Serial.println("Failed to read from DHT sensor!");
          }else{
            humid = h;
            temp = t;

            String valuePublish = (String)humid+","+(String)temp+",";
            Serial.print("Sending --> ");
            Serial.println(valuePublish);
            microgear.publish(topicPublish,valuePublish);
          }
        }

        // FEED Code
        // FEED Code END
    }
    else {
        Serial.println("connection lost, reconnect...");
        if (timer >= 5000) {
            microgear.connect(APPID); 
            timer = 0;
        }
        else timer += 100;
        delay(100);
    }
}

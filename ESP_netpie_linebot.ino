          #include <Wire.h>
          #include <MicroGear.h>
          #include <OneWire.h>
          #include <DallasTemperature.h>
          #include <Adafruit_Sensor.h>
          #include <Adafruit_TSL2561_U.h>
          
          Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
          
          
          const char* ssid     = "TOPIC_DEV";
          const char* password = "1212312121";
          
          //const char* ssid     = "TOT_fiber_2U-5G_b313";
          //const char* password = "3edab313";
          
          const char* host = "https://basebots.herokuapp.com/bot.php";
          #define APPID   "Basebot"
          #define KEY     "VL3Ntfnptg9LOOh"
          #define SECRET  "Q7vloIhwHWBeCYOJkb5YT3qGM"
          
          #define ALIAS   "Bot1"
          #define topicPublish "/dht/" ALIAS
          #define topicPublish1 "/soil/" ALIAS 
          #define topicPublish2 "/light/" ALIAS 
          #define topicPublish3 "/DS/" ALIAS
          
          //DHTesp dht;
          //TSL2561 tsl(TSL2561_ADDR_FLOAT); 
          
          #include "DHT.h"
          #define DHTPIN 13 
          #define DHTTYPE DHT11
          #define ONE_WIRE_BUS 12
          
          DHT dht(DHTPIN, DHTTYPE);
          OneWire oneWire(ONE_WIRE_BUS);
          DallasTemperature sensors(&oneWire);
          
          unsigned long lastTimeDHT = 0;
          float humid = 0;
          float temp = 0;
          float soil = 0;
          float DS = 0;
          float light = 0;
          float h = 0;
          float t= 0;
          int stateLED = 0;
          bool stateChange = false;
          
          WiFiClient client;
          int timer = 0;
          MicroGear microgear(client);
               
          //********************************************************************************
          void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) { // 
              Serial.print("Incoming message --> ");
              msg[msglen] = '\0';
              Serial.println((char *)msg);
              
          }
          //*******************************************************************************
          void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
              Serial.println("Connected to NETPIE...");
              microgear.setAlias(ALIAS);
          }
          int moisture =0;
          //*******************************************************************************
          void setup()
          {
            microgear.on(MESSAGE,onMsghandler);
            microgear.on(CONNECTED,onConnected);
              Serial.begin(115200);  
              sensors.begin();
              dht.begin();
        //เช็คWIFI     
        if (WiFi.begin(ssid, password)) {
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.print(".");
        }
    }
            Serial.println("WiFi connected");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());
            
           microgear.init(KEY,SECRET,ALIAS);   // กำหนดค่าตันแปรเริ่มต้นให้กับ microgear
           microgear.connect(APPID);           // ฟังก์ชั่นสำหรับเชื่อมต่อ NETPIE
          }
//****************************************************************************          
          void loop() {
            
          //Temp0
            TEMP();{
                  
              
           if (isnan(h) || isnan(t)) {
                      Serial.println("Failed to read from DHT sensor!");
                    }else {  
                     humid = h ;
                     temp  = t ;
                      String valuePublish = (String)humid+","+(String)temp;
                      Serial.print("Sending --> ");
                      Serial.println(valuePublish);
                      microgear.publish(topicPublish,valuePublish);
                  
             }
            }
           
          //soil 1
             soilsensor();{
             soil = analogRead(A0); 
            if (isnan(soil)) {
                      Serial.println("Failed to read from light sensor!");
                    }else
                    {
                      
                      String valuePublish1 = (String)soil;
                      Serial.print("Sending --> ");
                      Serial.println(valuePublish1);
                      microgear.publish(topicPublish1,valuePublish1);
                    } 
             }                
       //light 2
      
     sensors_event_t event;
     tsl.getEvent(&event);
     lightsensor();
  float L = event.light;
  
  if (isnan(event.light)) {
            Serial.println("Failed to read from light sensor!");
          }else{
            light = L;
            String valuePublish2 = (String)light;
            Serial.print("Sending --> ");
            Serial.println(valuePublish2);
            microgear.publish(topicPublish2,valuePublish2); 
          }


  


//************************************************************************          
          //DS1820 3
            DS1820();{
            if (isnan(DS) ) {
             
                      Serial.println("Failed to read from DHT sensor!");
                    }else {
                     DS = sensors.getTempCByIndex(0);
                      String valuePublish3 = (String)DS;
                      Serial.print("Sending --> ");
                      Serial.println(valuePublish3);
                      microgear.publish(topicPublish3,valuePublish3);
                    }
                   
            }
          
          
          
          
                   
          }
//**********************************************************************
          void TEMP(){
           
            h = dht.readHumidity();
            t = dht.readTemperature();
          
          Serial.print("Humidity: ");  Serial.print(h); Serial.print(" %\t");
            
          Serial.print("Temperature: ");  Serial.print(t);  Serial.println(" *C ");
          
           
          }
          //**********************************************************************
          
          void soilsensor(){
          
          int moisture = analogRead(A0);
            Serial.print("Soil Moisture = ");
            Serial.println(moisture);
            
          }
          
          //********************************************************************
          void lightsensor(){
             
            sensors_event_t event;
            tsl.getEvent(&event);
           
            if (event.light)
            {
              Serial.print(event.light); Serial.println(" lux");
            }
            else
            {
              Serial.println("Sensor overload");
            }
            delay(500);
          }
          
          //**********************************************************************
          
          void DS1820() {
            
           // OneWire oneWire(ONE_WIRE_BUS);
            //DallasTemperature sensors(&oneWire);
          
            sensors.requestTemperatures(); 
            Serial.print("DS1820 temperature: ");
            Serial.println(sensors.getTempCByIndex(0)); 
            
          }
          
          
          
          
          
          
          
          
          
          
          
          
          

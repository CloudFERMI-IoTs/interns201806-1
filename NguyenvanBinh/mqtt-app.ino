/*
 * Hiển thị thông tin lên MQTT, điều khiển GPIO qua topic.
 * Broker: trang cloudmqtt.com ( đăng kí tài khoản và điền thông tin tài khoản vào code)
 * hien thi len app MQTT Dash
 * 
 * Hoặc dùng node-red
 */


#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 5
#define DHTTYPE DHT21
DHT dht(DHTPIN, DHTTYPE);


#define ssid "CTEKLab"
#define password "12345680*"



#define mqtt_server "m14.cloudmqtt.com"             // Broker: cloudmqtt.com ( nối với app điện thoại: Mqtt Dash 
#define mqtt_user "oivukdsj"                        //                                                     nhé cậu  )
#define mqtt_pwd "cWXO3ZZ1L3-v"
const uint16_t mqtt_port = 16099; 

/*
#define mqtt_server "broker.mqtt-dashboard.com"     //( Broker này với node-red           ( Broker :mosquitto)
const uint16_t mqtt_port = 1883;                    //                    nhé cậu  )
*/


#define ledPin 32
#define ledPin1 23

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

float temp = 0;
float humidity = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  setup_wifi();
client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Topic  : ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();


 if (String(topic) == "room/lamp") {                                                                                                               
    Serial.print("Topic ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, LOW);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, HIGH);
    }
  }
 if (String(topic) == "room/fan") {                                                                                                               
    Serial.print("Topic  ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin1, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin1, LOW);
    }
  }
  
  
}

void reconnect() {
  
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
      if (client.connect("ESP8266Client",mqtt_user, mqtt_pwd)) {                 // Dùng dòng này với Broker :Cloudmqtt.com
  //  if (client.connect("ESPClient")) {                                         // Dùng dong này với Broker : mosquitto

     Serial.println("connected");
    
     client.subscribe("room/lamp");
     client.subscribe("room/fan");
                                                                                                                                                        
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Đợi 5s
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;


temp = dht.readTemperature(); 
    char tempString[8];
    dtostrf(temp, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
   client.publish("room/temperature", tempString);
                                                   

 
humidity = dht.readHumidity();
    char humString[8];
    dtostrf(humidity, 1, 2, humString);
    Serial.print("Humidity: ");
    Serial.println(humString);
    client.publish("room/humidity", humString); 


    

   
  }
}

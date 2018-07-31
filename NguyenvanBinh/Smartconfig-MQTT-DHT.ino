/* 
- WiFi.mode (WIFI_AP_STA): đặt ESP32 sang chế độ Station (Để chạy SmartConfig nó phải được đặt ở chế độ Station) 
- WiFi.beginSmartConfig (): khởi động SmartConfig 
- WiFi .smartConfigDone (): kiểm tra xem SmartConfig có được thực hiện hay không 

Dùng phần mềm điện thoại : Espressif Esptouch (IOS), hoặc ESP8266 SmartConfig (android), để đăng nhập Wifi cho esp32, 
Hiển thị nhiệt độ độ ẩm ( DHT ) qua giao thức MQTT
Dùng trang cloudmqtt.com để làm broker( đăng kí tài khoản, điền thông tin vào tài khoản vào code. )

 */
#include "WiFi.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 5
#define DHTTYPE DHT21
DHT dht(DHTPIN, DHTTYPE);
                
#define mqtt_server "m14.cloudmqtt.com" 
#define mqtt_topic_pub "top2"   
#define mqtt_topic_sub "top/a/b"
#define mqtt_user "oivukdsj"    
#define mqtt_pwd "cWXO3ZZ1L3-v"
                
const uint16_t mqtt_port = 16099; //Port của CloudMQTT
                
WiFiClient espClient;
PubSubClient client(espClient);
                
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  Serial.begin(115200);
  /* Set ESP32 to WiFi Station mode */
  WiFi.mode(WIFI_AP_STA);
  /* start SmartConfig */
  WiFi.beginSmartConfig();

  /* Wait for SmartConfig packet from mobile */
  Serial.println("Waiting for SmartConfig.");
  while (!WiFi.smartConfigDone()) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("SmartConfig done.");

  /* Wait for WiFi to connect to AP */
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
                  
                    client.setServer(mqtt_server, mqtt_port); 
                    client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    char receivedChar = (char)payload[i];
    Serial.print(receivedChar);
    if (receivedChar == '0')
      // Kiểm tra nếu tin nhận được là 1 thì bật LED và ngược lại
      digitalWrite(32, LOW);
    if (receivedChar == '1')
      digitalWrite(32, HIGH);
  }
  Serial.println();
}
// Hàm reconnect thực hiện kết nối lại khi mất kết nối với MQTT Broker
void reconnect() {
  // Chờ tới khi kết nối
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Thực hiện kết nối với mqtt user và pass
    if (client.connect("ESP8266Client",mqtt_user, mqtt_pwd)) {
      Serial.println("connected");
      // Khi kết nối sẽ publish thông báo
      client.publish(mqtt_topic_pub, "ESP_reconnected");
      // ... và nhận lại thông tin này
      client.subscribe(mqtt_topic_sub);
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
  // Kiểm tra kết nối
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // Sau mỗi 2s sẽ thực hiện publish dòng hello world lên MQTT broker
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
//    ++value;
float h = dht.readHumidity();
float t = dht.readTemperature();
                              
snprintf (msg,100 , "Nhiet do:  #%f", t);
Serial.print("Publish message: ");
Serial.println(msg);
client.publish(mqtt_topic_sub, msg);
                              
snprintf (msg,100 , "Do am:  #%f", h);
Serial.print("Publish message: ");
Serial.println(msg);
client.publish(mqtt_topic_sub, msg);
  }
}

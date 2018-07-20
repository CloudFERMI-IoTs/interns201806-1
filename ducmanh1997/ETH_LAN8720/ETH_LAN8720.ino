/*
    This sketch shows the Ethernet event usage

*/

#define ETH_PHY_ADDR 1
#define ETH_PHY_TYPE ETH_PHY_LAN8720
#define ETH_PHY_POWER 33
#define ETH_PHY_MDC 16
#define ETH_PHY_MDIO 0
#define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT

#include <WiFi.h>
#include <ETH.h>
#include <PubSubClient.h>

// Thông tin về MQTT Broker
#define mqtt_server "m12.cloudmqtt.com" 
#define mqtt_topic_pub "topic"   
#define mqtt_topic_sub "topic1"
#define mqtt_user "cjtdarvp"    
#define mqtt_pwd "wam6k0pGKOYD"


const uint16_t mqtt_port = 18011; //Port của CloudMQTT

long lastMsg = 0;
char msg[50];
int value = 0;

#define SD_CD     35
#define SD_PWR    32      // LED Test
#define LAN_RST   33


WiFiClient espClient;
PubSubClient client(espClient);
uint32_t cnt = 0;


static uint8_t eth_connected = 0;

void WiFiEvent(WiFiEvent_t event)
{
  switch (event) {
    case SYSTEM_EVENT_ETH_START://ESP32 bắt đầu ethernet 
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      eth_connected = SYSTEM_EVENT_ETH_START;
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:// ESP32 ethernet phy link up
      Serial.println("ETH Connected");
      eth_connected = SYSTEM_EVENT_ETH_CONNECTED;
      break;
    case SYSTEM_EVENT_ETH_GOT_IP://ESP32 ethernet got IP from connected AP
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      eth_connected = SYSTEM_EVENT_ETH_GOT_IP;
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED://ESP32 ethernet phy link down
      Serial.println("ETH Disconnected");
      eth_connected = SYSTEM_EVENT_ETH_DISCONNECTED;
      break;
    case SYSTEM_EVENT_ETH_STOP://ESP32 ethernet stop
      Serial.println("ETH Stopped");
      eth_connected = 0;
      break;
    default:
      break;
  }
}

/*void testClient(const char * host, uint16_t port)
  {
  Serial.print("\nconnecting to ");
  Serial.println(host);

  WiFiClient client;
  if (!client.connect(host, port)) // client connect fail
  {
    Serial.println("connection failed");
    return;
  }
  client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
  while (client.connected() && !client.available());    // connect true, client ko co san 
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("closing connection\n");
  client.stop();
}*/
// Hàm call back để nhận dữ liệu
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  /*for (int i = 0; i < length; i++) {
    char receivedChar = (char)payload[i];
    Serial.print(receivedChar);
    if (receivedChar == '0')
      // Kiểm tra nếu tin nhận được là 1 thì bật LED và ngược lại
      digitalWrite(32, LOW);
    if (receivedChar == '1')
      digitalWrite(32, HIGH);
  }*/
  Serial.println();
}

// Hàm reconnect thực hiện kết nối lại khi mất kết nối với MQTT Broker
void reconnect() {
  // Chờ tới khi kết nối
 while (!client.connected()) 
 // while (eth_connected = SYSTEM_EVENT_ETH_CONNECTED)  
  {
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
void setup()
{
    Serial.begin(115200);
    
    client.setServer(mqtt_server, mqtt_port); 
    client.setCallback(callback);
    
    pinMode(SD_PWR, OUTPUT);

    // waiting 10s before init LAN
    for(uint8_t i = 0; i < 5; i++) {      
      Serial.printf("Waiting 1s...before init\n");
      digitalWrite(SD_PWR, LOW);
      delay(250);
      digitalWrite(SD_PWR, HIGH);
      delay(250);
      digitalWrite(SD_PWR, LOW);
      delay(250);
      digitalWrite(SD_PWR, HIGH);
      delay(250);      
    }
  
    pinMode(LAN_RST, OUTPUT);
    digitalWrite(LAN_RST, LOW);
    /*
    digitalWrite(PWR_LAN, LOW);
    delay(5);
    digitalWrite(PWR_LAN, HIGH);
    delay(5);      
    */

    
    WiFi.onEvent(WiFiEvent);
    ETH.begin();
    //delay(1);
    //digitalWrite(LAN_RST, HIGH);
    delay(2000);
}


void loop()
{
  /*if (eth_connected == SYSTEM_EVENT_ETH_GOT_IP) {
    testClient("google.com", 80);
    if(cnt > 0) {
      cnt = 0;
   }
  } else {
    if(eth_connected != SYSTEM_EVENT_ETH_DISCONNECTED) // neu eth_connected ko bang ETH_DISCONNECTED
    {
      cnt++;
      if(cnt >= 10) {
        digitalWrite(LAN_RST, LOW);
        delay(5);
        digitalWrite(LAN_RST, HIGH);
        delay(5);  
        cnt = 0;
        if(eth_connected != 0) {
          ETH.begin();    
       }
        
        
      } else {
         Serial.printf("Waiting 10s...before re-init, %d\n", cnt);
      }           
    }*/
  if (!client.connected()) {
      reconnect();
    }
   
    client.loop();
  // Sau mỗi 2s sẽ thực hiện publish dòng hello world lên MQTT broker
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    ++value;
  
    snprintf (msg, 75, "Duc Manh %ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(mqtt_topic_sub, msg);
    

 //}
  digitalWrite(SD_PWR, LOW);
  delay(500);
  digitalWrite(SD_PWR, HIGH);
  delay(500); 
}
}

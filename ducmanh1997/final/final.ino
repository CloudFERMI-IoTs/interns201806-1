#define ETH_PHY_ADDR 1
#define ETH_PHY_TYPE ETH_PHY_LAN8720
#define ETH_PHY_POWER 33
#define ETH_PHY_MDC 16
#define ETH_PHY_MDIO 0
#define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT


#include <ETH.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <WiFiClient.h>
#include <WebServer.h>

// Thông tin về MQTT Broker
const char* mqtt_server = "m12.cloudmqtt.com" ;
const char* mqtt_user = "cjtdarvp"  ;  
const char* mqtt_pwd = "wam6k0pGKOYD";
int mqtt_port = 18011; //Port của CloudMQTT

const char* wifi;
const char* mk;

const char *ssid = "AP";
const char *password = "12345678";

#define DHTPIN 5
#define DHTTYPE DHT21
DHT dht(DHTPIN, DHTTYPE);


WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
float temperature = 0;
float humidity = 0;

uint32_t cnt = 0;

static uint8_t eth_connected = 0;

void handleRoot();              // function prototypes for HTTP handlers
void handleLogin();
void handleNotFound(); 

void handleRoot() {                          // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", "<form action=\"/login\" method=\"POST\"><label class=\"label\">Network Name</label><input type=\"text\" name=\"username\" placeholder=\"Username\"></br><label>Password</label><input type=\"text\" name=\"password\" placeholder=\"Password\"></br><input type=\"submit\" value=\"Login\"></form>");
}

void handleLogin() {                         // If a POST request is made to URI /login

  
  
  
  String ssid = server.arg("username");/* Lấy giá trị của biến username trên server */
  Serial.println(ssid);
  String pass = server.arg("password");
  Serial.println(pass);
  
  wifi = ssid.c_str();
  mk = pass.c_str();
  
  setup_wifi();
}

void setup_wifi(){
  WiFi.begin(wifi, mk);

  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
  eth_connected = SYSTEM_EVENT_ETH_CONNECTED;
  server.send(200, "text/html", "<h1>Welcome, " + server.arg("username") + "!</h1><p>Login successful</p>");
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

// Hàm call back để nhận dữ liệu
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic : ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();


 if (String(topic) == "room/lamp") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(32, LOW);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(32, HIGH);
    }
  }
  
}
void reconnect() {
  // Chờ tới khi kết nối
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Thực hiện kết nối với mqtt user và pass
    if (client.connect("ESP32Client",mqtt_user, mqtt_pwd)) {
      Serial.println("connected");
      // Khi kết nối sẽ publish thông báo
 //client.publish(mqtt_topic_pub, "ESP_reconnected");
      // ... và nhận lại thông tin này
 //client.subscribe(mqtt_topic_sub);
      client.subscribe("room/lamp");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Đợi 5s
      delay(5000);
    }
  }
}
void WiFiEvent(WiFiEvent_t event)
{
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      eth_connected = SYSTEM_EVENT_ETH_START;
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      eth_connected = SYSTEM_EVENT_ETH_CONNECTED;
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
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
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = SYSTEM_EVENT_ETH_DISCONNECTED;
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = 0;
      break;
    default:
      break;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(32, OUTPUT);
  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);
  WiFi.onEvent(WiFiEvent);
  ETH.begin();
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  server.on("/html", HTTP_GET, handleRoot);        // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/login", HTTP_POST, handleLogin); // Call the 'handleLogin' function when a POST request is made to URI "/login"
  server.onNotFound(handleNotFound);           // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
  
  server.begin();                            // Actually start the server
  Serial.println("HTTP server started");

}
void loop() {
  // put your main code here, to run repeatedly:
   server.handleClient(); 
   /*if (!client.connected()) {
   // setup_wifi();   
    reconnect();    
  }*/
  if(eth_connected == SYSTEM_EVENT_ETH_CONNECTED)
  {reconnect();}
  if (client.connected()){
  client.loop();
// gửi thông tin lên MQTT sau mỗi   10s
  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;


temperature = dht.readTemperature(); 
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);
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
}

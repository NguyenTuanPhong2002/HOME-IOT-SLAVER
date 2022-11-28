#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <espnow.h>

//set pin output
#define port1 2
#define port2 4
#define port3 5

//set pin input
#define status1 12
#define status2 13
#define status3 14

//set serial status
int statusport1 = 0;
int statusport2 = 0;
int statusport3 = 0;


// set wifi interface
const char* ssid = "NNE";
const char* password = "01112002";

// Insert your SSID
constexpr char WIFI_SSID[] = "NNE";
int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
    for (uint8_t i=0; i<n; i++) {
      if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
        return WiFi.channel(i);
      }
    }
  }
  return 0;
}

// REPLACE WITH RECEIVER MAC Address
uint8_t master[] = {0x3c,0x61,0x05,0xe4,0xaf,0x2c};

// Must match the sender structure
typedef struct struct_message {
  int id = 2;
  int port; //
  int status;
} struct_message;

struct_message data;
struct_message data1;
struct_message data2;
struct_message data3;
// Create a struct_message called myData

unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;  // send readings timer

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&data, incomingData, sizeof(data));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("PORT: ");
  Serial.println(data.port);
  if(data.port == 1){
    data1.status = !data1.status;
    esp_now_send(master, (uint8_t *) &data1, sizeof(data1));
  }else if (data.port == 2)
  {
    data2.status = !data2.status;
    esp_now_send(master, (uint8_t *) &data2, sizeof(data2));
  }else if (data.port == 3)
  {
    data3.status = !data3.status;
    esp_now_send(master, (uint8_t *) &data3, sizeof(data3));
  }
  Serial.print("STATUS: ");
  Serial.println(data.status);
}

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

// Callback when set pin
void setpin(){
  //set pinmode
  pinMode(port1, OUTPUT);
  pinMode(port2, OUTPUT);
  pinMode(port3, OUTPUT);

  data1.status = 0;
  data2.status = 0;
  data2.status = 0;

  digitalWrite(port1, data1.status);
  digitalWrite(port2, data2.status);
  digitalWrite(port3, data3.status);

  pinMode(status1, INPUT);
  pinMode(status2, INPUT);
  pinMode(status3, INPUT);

}

void config_wifi(){
  Serial.println("---------------------------");
  WiFi.mode(WIFI_STA);

  int32_t channel = getWiFiChannel(WIFI_SSID);

  WiFi.printDiag(Serial); // Uncomment to verify channel number before
  wifi_promiscuous_enable(1);
  wifi_set_channel(channel);
  wifi_promiscuous_enable(0);
  WiFi.printDiag(Serial); // Uncomment to verify channel change after
  Serial.println("---------------------------");
}
void setup() {
  //set pinmode
  setpin();
  // Init Serial Monitor
  Serial.begin(9600);
  //Mac address
  Serial.println("---------------------------");
  Serial.print("ESP8266 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  Serial.println("---------------------------");
 
  // Set device as a Wi-Fi Station
  config_wifi();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(master, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  data1.port = 1;
  data2.port = 2;
  data3.port = 3;
  esp_now_send(master, (uint8_t *) &data1, sizeof(data1));
  esp_now_send(master, (uint8_t *) &data2, sizeof(data2));
  esp_now_send(master, (uint8_t *) &data3, sizeof(data3));
}

void loop() {
  //int keymode = 0;
  //if(digitalRead(status1)==1){keymode = 1;}
  //if(digitalRead(status2)==1){keymode = 2;}
  //if(digitalRead(status3)==1){keymode = 3;}
//
  //switch(keymode) {
  //case 1:
  //  // code block
  //  if(data1.status == 1){
  //    data1.status = 0;
  //  }
  //  else{
  //    data1.status = 1;
  //  }
  //  data1.port = 1;
  //  keymode = 0;
  //  break;
  //case 2:
  //  // code block
  //  if(data2.status == 1){
  //    data2.status = 0;
  //  }
  //  else{
  //    data2.status = 1;
  //  }
  //  data2.port = 2;
  //  keymode = 0;
  //  break;
  //case 3:
  //if(data3.status == 1){
  //    data3.status = 0;
  //  }
  //  else{
  //    data3.status = 1;
//
  //  }
  //  data3.port = 3;
  //  keymode = 0;
  //  break;
  //}
  //digitalWrite(port1, data1.status);
  //digitalWrite(port2, data2.status);
  //digitalWrite(port3, data3.status);
  //keymode = 0;
  esp_now_send(master, (uint8_t *) &data1, sizeof(data1));
  esp_now_send(master, (uint8_t *) &data2, sizeof(data2));
  esp_now_send(master, (uint8_t *) &data3, sizeof(data3));
  delay(1000);
}
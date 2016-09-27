#include <ESP8266WiFi.h>

IPAddress local_IP(192,168,4,21);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

#define SERVER_IP_ADDR "192.168.4.22"
#define SERVER_PORT        4011

WiFiServer  _server(4011);
WiFiClient  _client;

String myname = "ESPsoftAP_01";



void setup()
{
  
  Serial.begin(115200);
  Serial.println();

  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  WiFi.mode(WIFI_AP_STA);
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(myname.c_str()) ? "Ready" : "Failed!");
  _server.begin();

  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());

  
}

bool waitForClient(WiFiClient curr_client, int max_wait){
  int wait = max_wait;
  while(curr_client.connected() && !curr_client.available() && wait--)
    delay(3);

  /* Return false if the client isn't ready to communicate */
  if (WiFi.status() == WL_DISCONNECTED || !curr_client.connected())
    return false;
  
  return true;
}

bool SendMessage(String message, WiFiClient curr_client){
  curr_client.println( message.c_str() );

  if (!waitForClient(curr_client, 1000))
    return false;

  String response = curr_client.readStringUntil('\r');
  curr_client.readStringUntil('\n');

  if (response.length() <= 2) 
    return false;

  /* Pass data to user callback 
  _handler(response);
  */
  Serial.print("Response: ");
  Serial.println(response);
  
  return true;
}

void connectToNode(String target_ssid, String message){



  
  WiFiClient curr_client;
  
  WiFi.begin( target_ssid.c_str() );

 
  int wait = 1500;
  while((WiFi.status() == WL_DISCONNECTED) && wait--)
    delay(3);

  /* If the connection timed out */
  if (WiFi.status() != 3){
    Serial.print("connection timed out ");
    Serial.print(WiFi.status());
    Serial.println(target_ssid);
    return;
  }

  /* Connect to the node's server */
  if (!curr_client.connect(SERVER_IP_ADDR,4011)) 
    Serial.print("Cannot Connect to server at ");
    Serial.print(SERVER_IP_ADDR);
    Serial.print(",");
    Serial.print(SERVER_PORT);
    Serial.print(",");
    Serial.println(WiFi.status());
    return;

  if (!SendMessage(message, curr_client))
    return;
}

void loop() {
  
  
  connectToNode("ESPsoftAP_02","Hello from "+myname);
  
  //if(SendMessage("Hello from "+myname, curr_client)){
  //  Serial.println("message sent!");
  //} 
}

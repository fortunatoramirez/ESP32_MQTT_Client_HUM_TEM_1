#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

//**************************************
//*********** DHT SENSOR  **************
//**************************************

#include <DHT.h>
#define DHTPIN 23     // Digital pin connected to the DHT sensor
// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);


//**************************************
//***** SENSOR LUZ (AnalogRead)  *******
//**************************************
int inputPin = 34;


//**************************************
//*********** MQTT CONFIG **************
//**************************************
const char *mqtt_server = "34.123.165.38"; //34.123.165.38

const int mqtt_port = 1883;
const char *mqtt_user = "ESP32_1";
const char *mqtt_pass = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJkZXZpY2VfaWQiOjYsImlhdCI6MTY0MDY4MjY4Nn0.G_vBzovkADMoZ17J2clew2WU24-GRiEzf2SRsgOoZRk";
const char *root_topic_subscribe = "/dispositivos/otra";
const char *root_topic_publish = "/dispositivos/dwmwlDaUuK"; //: 73ObRck7uA //5OICajUFGe



//**************************************
//*********** WIFICONFIG ***************
//**************************************
const char* ssid = "INFINITUMC2A2_2.4"; //S7F
const char* password =  "Dus2qzqyFC"; //UyQb1737



//**************************************
//*********** GLOBALES   ***************
//**************************************
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
long count=0;
/* SENSOR DHT*/
float h=0;
float t=0;
int   l=0;


//************************
//** F U N C I O N E S ***
//************************
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  /* SENSOR DHT */
  dht.begin();
}

void loop() {
  
  if (!client.connected()) {
    reconnect();
  }

  /* SENSOR DHT */
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!")); // F(): Flash Mem instead RAM
    return;
  }

  /* SENSOR DE LUZ*/
  l = analogRead(inputPin);

  if (client.connected()){
    //String str = "La cuenta es -> " + String(count);
    String str = "{\"temperatura\":"+String(t)+", \"luz\":"+String(l)+"}";
    str.toCharArray(msg,50);
    client.publish(root_topic_publish,msg);
    Serial.println("Enviado -> "+str);
    //count++;
    delay(1000);
  }
  
  client.loop(); //cada que se ejectua revisa si han llegado mensajes
  delay(1000);
}




//*****************************
//***    CONEXION WIFI      ***
//*****************************
void setup_wifi(){
  delay(10);
  // Nos conectamos a nuestra red Wifi
  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}



//*****************************
//***    CONEXION MQTT      ***
//*****************************

void reconnect() {

  while (!client.connected()) {
    Serial.print("Intentando conexión Mqtt...");
    // Creamos un cliente ID
    String clientId = "IOTICOS_H_W_";
    clientId += String(random(0xffff), HEX);
    // Intentamos conectar
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
      Serial.println("Conectado!");
      // Nos suscribimos
      if(client.subscribe(root_topic_subscribe)){
        Serial.println("Suscripcion ok");
      }else{
        Serial.println("fallo Suscripciión");
      }
    } else {
      Serial.print("falló :( con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}


//*****************************
//***       CALLBACK        ***
//*****************************

void callback(char* topic, byte* payload, unsigned int length){
  String incoming = "";
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);
  Serial.println("");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Mensaje -> " + incoming);

}

#include <WiFi.h>
#include <FirebaseESP32.h>

// Configuración de WiFi
const char* ssid = "WIN UNI";
const char* password = "0123456789";

// Configuración de Firebase
#define FIREBASE_HOST "manipuladorweb-c1b90-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "U0UNLBmYWQQDvqQRuOyYsnrzKB5KaMNxqtQTqmG6"

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;


// Pines para el encoder
const int encoder1A = 34;
const int encoder1B = 35;
const int encoder2A = 32;
const int encoder2B = 33;

// Pines para el TB6612FNG
const int pwma = 27;
const int ain1 = 25;
const int ain2 = 26;
const int pwmb = 14;
const int bin1 = 12;
const int bin2 = 13;
// Variables para contar los pulsos del encoder
volatile int encoder1Count = 0;
volatile int encoder2Count = 0;

// Prototipos de funciones de interrupción
void IRAM_ATTR handleEncoder1();
void IRAM_ATTR handleEncoder2();

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());

  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  pinMode(encoder1A, INPUT);
  pinMode(encoder1B, INPUT);
  pinMode(encoder2A, INPUT);
  pinMode(encoder2B, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoder1A), handleEncoder1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoder2A), handleEncoder2, CHANGE);
  pinMode(pwma, OUTPUT);
  pinMode(ain1, OUTPUT);
  pinMode(ain2, OUTPUT);
  pinMode(pwmb, OUTPUT);
  pinMode(bin1, OUTPUT);
  pinMode(bin2, OUTPUT);
}

void loop() {
  if (Firebase.get(firebaseData, "/sensorData")) {
    if()
  } else {
    Serial.println("Failed to get data: " + firebaseData.errorReason());
  }

  delay(5000); // Obtener datos cada 5 segundos
}

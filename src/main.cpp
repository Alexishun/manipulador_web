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
}

void loop() {
  if (Firebase.get(firebaseData, "/sensorData/angQ1")) {
    if (firebaseData.dataType() == "string") {
      int angQ1;
      angQ1 = firebaseData.stringData().toInt();
      Serial.print("angQ1: ");
      Serial.println(angQ1);
    }
  } else {
    Serial.println("Failed to get data: " + firebaseData.errorReason());
  }

  delay(5000); // Obtener datos cada 5 segundos
}

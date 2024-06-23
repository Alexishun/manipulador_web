#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

//const char* ssid = "WIN UNI";
//const char* ssid = "Daniel";
//const char* password = "0123456789";
//const char* password = "robin0106";

WebServer server(80); // Usar WebServer en lugar de WiFiServer

// Variables para almacenar los movimientos
String angQ1 = "60";
String angQ2 = "45";  
String angQ3 = "60";
String angQ4 = "50";
int currentAngle1 = 60;
int currentAngle2 = 45;
int currentAngle3 = 60;
int currentEffectorAngle = 50;
// Variables para almacenar las cantidades en los almacenes
int cantA = 0;
int cantB = 0;

// Declarar los objetos Servo
Servo servo1;
Servo servo2;
Servo servo3;
Servo effector;

// FUNCIONES DE MOVIMIENTO
void interpolateServo(Servo &servo, int startAngle, int endAngle, int delayTime) {
  if (startAngle < endAngle) {
    for (int angle = startAngle; angle <= endAngle; angle++) {
      servo.write(angle);
      delay(delayTime);
    }
  } else {
    for (int angle = startAngle; angle >= endAngle; angle--) {
      servo.write(angle);
      delay(delayTime);
    }
  }
}
void moveToPosition(int angleMotor1, int angleMotor2, int angleMotor3, int angleEffector, int delayTime) {
  interpolateServo(servo2, currentAngle2, angleMotor2, delayTime);
  currentAngle2 = angleMotor2;
  interpolateServo(servo1, currentAngle1, angleMotor1, delayTime);
  currentAngle1 = angleMotor1;
  interpolateServo(servo3, currentAngle3, angleMotor3, delayTime);
  currentAngle3 = angleMotor3;
  interpolateServo(effector, currentEffectorAngle, angleEffector, delayTime);
  currentEffectorAngle = angleEffector;
}
void moveToPosition_1(int angleMotor1, int angleMotor2, int angleMotor3, int angleEffector, int delayTime) {
  interpolateServo(servo3, currentAngle3, angleMotor3, delayTime);
  currentAngle3 = angleMotor3;
  interpolateServo(effector, currentEffectorAngle, angleEffector, delayTime);
  currentEffectorAngle = angleEffector;
  interpolateServo(servo1, currentAngle1, angleMotor1, delayTime);
  currentAngle1 = angleMotor1;
  interpolateServo(servo2, currentAngle2, angleMotor2, delayTime);
  currentAngle2 = angleMotor2;
}
void handleRoot() {
  String html = "<!DOCTYPE html><html lang='es'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Inventariado de Almacén</title><style>body {font-family: Arial, sans-serif;background-color: #f4f4f4;color: #333;display: flex;justify-content: center;align-items: center;height: 100vh;margin: 0;}.container {background: #fff;padding: 20px;border-radius: 10px;box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);width: 500px;}.header {text-align: center;margin-bottom: 20px;}.header h1 {margin: 0;font-size: 24px;color: #007BFF;}.section {margin-bottom: 20px;}.section h2 {font-size: 18px;margin-bottom: 10px;color: #555;}.webcam {background: #333;height: 200px;border-radius: 10px;margin-bottom: 20px;}.almacen {display: flex;justify-content: space-between;margin-bottom: 20px;}.almacen div {width: 45%;}.almacen label {font-size: 16px;display: block;margin-bottom: 5px;}.almacen input {width: 100%;padding: 5px;font-size: 16px;border: 1px solid #ccc;border-radius: 5px;}.movimientos, .rutina {margin-bottom: 20px;}.movimientos label, .rutina label {font-size: 16px;display: block;margin-bottom: 5px;}.movimientos input, .rutina button {width: 100%;padding: 5px;font-size: 16px;border: 1px solid #ccc;border-radius: 5px;margin-bottom: 10px;}.rutina button {background-color: #007BFF;color: white;cursor: pointer;border: none;}.rutina button:hover {background-color: #0056b3;}.footer {text-align: center;font-size: 14px;color: #777;}.go-button {width: 100%;padding: 10px;font-size: 16px;border: none;border-radius: 5px;background-color: #28a745;color: white;cursor: pointer;}.go-button:hover {background-color: #218838;}</style><script>"
  "function sendData() {var angQ1 = document.getElementById('mov1').value;var angQ2 = document.getElementById('mov2').value;var angQ3 = document.getElementById('mov3').value;var angQ4 = document.getElementById('mov4').value;var xhr = new XMLHttpRequest();xhr.open('POST', '/update', true);xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');xhr.send('angQ1=' + angQ1 + '&angQ2=' + angQ2 + '&angQ3=' + angQ3 + '&angQ4=' + angQ4);xhr.onload = function() {if (xhr.status == 200) {var response = xhr.responseText;if (response.startsWith(\"Error:\")) {alert(response);} else {console.log('Valores enviados: ' + response);}}}}"
  "function updateAlmacen(almacen) {var xhr = new XMLHttpRequest();xhr.open('POST', '/routine', true);xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');xhr.send('almacen=' + almacen);xhr.onload = function() {if (xhr.status == 200) {console.log('Rutina ejecutada: ' + xhr.responseText);var data = JSON.parse(xhr.responseText);document.getElementById('almacen-a').value = data.cantA;document.getElementById('almacen-b').value = data.cantB;}}}"
  "function captureImage(resolution) {var xhr = new XMLHttpRequest();xhr.open('GET', resolution, true);xhr.send();xhr.onload = function() {if (xhr.status == 200) {console.log('Image captured: ' + xhr.responseText);}}}"
  "</script></head><body><div class='container'><div class='header'><h1>Inventariado Almacén</h1></div><div class='section webcam'><h2>Webcam</h2><button onclick=\"captureImage('/capture_low')\">Capture Low Resolution</button><button onclick=\"captureImage('/capture_mid')\"Capture Mid Resolution</button><button onclick=\"captureImage('/capture_high.jpg')\">Capture High Resolution</button></div><div class='section almacen'><div><label for='almacen-a'>Almacén A - Cantidad:</label><input type='number' id='almacen-a' name='cantA' value='0'></div><div><label for='almacen-b'>Almacén B - Cantidad:</label><input type='number' id='almacen-b' name='cantB' value='0'></div></div><div class='section movimientos'><h2>Movimientos</h2><label for='mov1'>Q1:</label><input type='text' id='mov1' name='angQ1' value='60'><label for='mov2'>Q2:</label><input type='text' id='mov2' name='angQ2' value='45'><label for='mov3'>Q3:</label><input type='text' id='mov3' name='angQ3' value='60'><label for='mov4'>Q4:</label><input type='text' id='mov4' name='angQ4' value='50'><button class='go-button' type='button' onclick='sendData()'>Go</button></div><div class='section rutina'><h2>Rutina</h2><button id='rutina-a' name='RutA' onclick='updateAlmacen(\"A\")'>A</button><button id='rutina-b' name='RutB' onclick='updateAlmacen(\"B\")'>B</button></div><div class='footer'>UNI Grupo 8 - MT136-B</div></div></body></html>";
  server.send(200, "text/html", html);
}

void handleCaptureLow() {
  // Enviar comando al ESP32-CAM para capturar imagen en baja resolución
  WiFiClient client;
  if (client.connect("192.168.18.101", 80)) {
    client.println("GET /capture_low HTTP/1.1");
    client.println("Host: 192.168.18.111");
    client.println("Connection: close");
    client.println();
  }
}

void handleCaptureMid() {
  // Enviar comando al ESP32-CAM para capturar imagen en resolución media
  WiFiClient client;
  if (client.connect("192.168.18.111", 80)) {
    client.println("GET /capture_mid HTTP/1.1");
    client.println("Host: 192.168.18.111");
    client.println("Connection: close");
    client.println();
  }
}

void handleCaptureHigh() {
  // Enviar comando al ESP32-CAM para capturar imagen en alta resolución
  WiFiClient client;
  if (client.connect("192.168.18.111", 80)) {
    client.println("GET /capture_high HTTP/1.1");
    client.println("Host: 192.168.18.111");
    client.println("Connection: close");
    client.println();
  }
}
void handleUpdate() {
  if (server.method() == HTTP_POST) {
    if (server.hasArg("angQ1")) {
      angQ1 = server.arg("angQ1");
    }
    if (server.hasArg("angQ2")) {
      angQ2 = server.arg("angQ2");
    }
    if (server.hasArg("angQ3")) {
      angQ3 = server.arg("angQ3");
    }
    if (server.hasArg("angQ4")) {
      angQ4 = server.arg("angQ4");
    }
    // Convertir los valores de angQ1 a angQ4 a enteros
    int pos1 = angQ1.toInt();
    int pos2 = angQ2.toInt();
    int pos3 = angQ3.toInt();
    int pos4 = angQ4.toInt();

    // Verificar los rangos de los ángulos y enviar una respuesta adecuada
    if (pos1 < 0 || pos1 > 80) {
      server.send(200, "text/plain", "Error: Ángulo motor 1 fuera de rango");
    } else if (pos2 < 0 || pos2 > 100) {
      server.send(200, "text/plain", "Error: Ángulo motor 2 fuera de rango");
    } else if (pos3 < 0 || pos3 > 180) {
      server.send(200, "text/plain", "Error: Ángulo motor 3 fuera de rango");
    } else if (pos4 < 50 || pos4 > 150) {
      server.send(200, "text/plain", "Error: Ángulo efector final fuera de rango");
    } else {
      // Mover los servos a las posiciones especificadas
      moveToPosition(pos1, pos2, pos3, pos4, 20);
      String response = "Q1: " + angQ1 + " Q2: " + angQ2 + " Q3: " + angQ3 + " Q4: " + angQ4;
      server.send(200, "text/plain", response);
    }  
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void handleRoutine() {
  if (server.method() == HTTP_POST) {
    if (server.hasArg("almacen")) {
      String almacen = server.arg("almacen");
      if (almacen == "A") {
        cantA++;
        int angleMotor1_1 = 0;  // Ángulo para motor 1
        int angleMotor2_1 = 90;  // Ángulo para motor 2
        int angleMotor3_1 = 0; // Ángulo para motor 3
        int angleEffector_1 = 120; // Ángulo para el efector final
        moveToPosition_1(angleMotor1_1, angleMotor2_1, angleMotor3_1, angleEffector_1, 20);
        // Esperar un momento antes de moverse a la segunda trayectoria
        delay(1000);

        int angleMotor1_2 = 0;  // Ángulo para motor 1
        int angleMotor2_2 = 90;  // Ángulo para motor 2
        int angleMotor3_2 = 0; // Ángulo para motor 3
        int angleEffector_2 = 50; // Ángulo para el efector final

        // Mover los servomotores a los ángulos de la primera trayectoria
        moveToPosition(angleMotor1_2, angleMotor2_2, angleMotor3_2, angleEffector_2, 10);

        // Esperar un momento antes de moverse a la segunda trayectoria
        delay(2000);
        
  
        int angleMotor1_3 = 60;  // Ángulo para motor 1
        int angleMotor2_3 = 45;  // Ángulo para motor 2
        int angleMotor3_3 = 0; // Ángulo para motor 3
        int angleEffector_3 = 50; // Ángulo para el efector final

        // Mover los servomotores a los ángulos de la segunda trayectoria
        moveToPosition(angleMotor1_3, angleMotor2_3, angleMotor3_3, angleEffector_3, 20);

        // Esperar un momento antes de repetir el ciclo
        delay(2000);
        // Definir la cuarta trayectoria
        int angleMotor1_4 = 60;  // Ángulo para motor 1
        int angleMotor2_4 = 45;  // Ángulo para motor 2
        int angleMotor3_4 = 100; // Ángulo para motor 3
        int angleEffector_4 = 50; // Ángulo para el efector final

        // Mover los servomotores a los ángulos de la segunda trayectoria
        moveToPosition(angleMotor1_4, angleMotor2_4, angleMotor3_4, angleEffector_4, 20);

        // Esperar un momento antes de repetir el ciclo
        delay(2000);
        // Definir la quinta trayectoria
        int angleMotor1_5 = 0;  // Ángulo para motor 1
        int angleMotor2_5 = 90;  // Ángulo para motor 2
        int angleMotor3_5 = 100; // Ángulo para motor 3
        int angleEffector_5 = 50; // Ángulo para el efector final

        // Mover los servomotores a los ángulos de la segunda trayectoria
        moveToPosition(angleMotor1_5, angleMotor2_5, angleMotor3_5, angleEffector_5, 20);

        // Esperar un momento antes de repetir el ciclo
        delay(2000);
        // Definir la sexta trayectoria
        int angleMotor1_6 = 0;  // Ángulo para motor 1
        int angleMotor2_6 = 90;  // Ángulo para motor 2
        int angleMotor3_6 = 100; // Ángulo para motor 3
        int angleEffector_6 = 100; // Ángulo para el efector final

        // Mover los servomotores a los ángulos de la segunda trayectoria
        moveToPosition(angleMotor1_6, angleMotor2_6, angleMotor3_6, angleEffector_6, 10);

        // Esperar un momento antes de repetir el ciclo
        delay(2000);
        // Definir la septima trayectoria
        int angleMotor1_7 = 60;  // Ángulo para motor 1
        int angleMotor2_7 = 45;  // Ángulo para motor 2
        int angleMotor3_7 = 100; // Ángulo para motor 3
        int angleEffector_7 = 100; // Ángulo para el efector final

        // Mover los servomotores a los ángulos de la segunda trayectoria
        moveToPosition(angleMotor1_7, angleMotor2_7, angleMotor3_7, angleEffector_7, 20);
        
        // Esperar un momento antes de repetir el ciclo
        delay(1000);
      // Definir la octava trayectoria
        int angleMotor1_8 = 60;  // Ángulo para motor 1
        int angleMotor2_8 = 45;  // Ángulo para motor 2
        int angleMotor3_8 = 60; // Ángulo para motor 3
        int angleEffector_8 = 50; // Ángulo para el efector final

        // Mover los servomotores a los ángulos de la segunda trayectoria
        moveToPosition(angleMotor1_8, angleMotor2_8, angleMotor3_8, angleEffector_8, 20);
      } else if (almacen == "B") {
        cantB++;
        //la primera trayectoria
        int angleMotor1_1 = 0;  // Ángulo para motor 1
        int angleMotor2_1 = 80;  // Ángulo para motor 2
        int angleMotor3_1 = 120; // Ángulo para motor 3
        int angleEffector_1 = 120; // Ángulo para el efector final
        moveToPosition_1(angleMotor1_1, angleMotor2_1, angleMotor3_1, angleEffector_1, 20);
        delay(1000);
        //la segunda trayectoria
        int angleMotor1_2 = 0;  // Ángulo para motor 1
        int angleMotor2_2 = 80;  // Ángulo para motor 2
        int angleMotor3_2 = 120; // Ángulo para motor 3
        int angleEffector_2 = 50; // Ángulo para el efector final
        moveToPosition(angleMotor1_2, angleMotor2_2, angleMotor3_2, angleEffector_2, 10);
        delay(2000);
        // Definir la tercera trayectoria
        int angleMotor1_3 = 60;  // Ángulo para motor 1
        int angleMotor2_3 = 45;  // Ángulo para motor 2
        int angleMotor3_3 = 120; // Ángulo para motor 3
        int angleEffector_3 = 50; // Ángulo para el efector final
        moveToPosition(angleMotor1_3, angleMotor2_3, angleMotor3_3, angleEffector_3, 20);
        delay(2000);
        //la cuarta trayectoria
        int angleMotor1_4 = 60;  // Ángulo para motor 1
        int angleMotor2_4 = 45;  // Ángulo para motor 2
        int angleMotor3_4 = 60; // Ángulo para motor 3
        int angleEffector_4 = 50; // Ángulo para el efector final
        moveToPosition(angleMotor1_4, angleMotor2_4, angleMotor3_4, angleEffector_4, 20);
        delay(2000);
        //la quinta trayectoria
        int angleMotor1_5 = 0;  // Ángulo para motor 1
        int angleMotor2_5 = 90;  // Ángulo para motor 2
        int angleMotor3_5 = 60; // Ángulo para motor 3
        int angleEffector_5 = 50; // Ángulo para el efector final
        moveToPosition(angleMotor1_5, angleMotor2_5, angleMotor3_5, angleEffector_5, 20);
        delay(2000);
        //la sexta trayectoria
        int angleMotor1_6 = 0;  // Ángulo para motor 1
        int angleMotor2_6 = 90;  // Ángulo para motor 2
        int angleMotor3_6 = 60; // Ángulo para motor 3
        int angleEffector_6 = 100; // Ángulo para el efector final
        moveToPosition(angleMotor1_6, angleMotor2_6, angleMotor3_6, angleEffector_6, 10);
        delay(2000);
        //la septima trayectoria
        int angleMotor1_7 = 60;  // Ángulo para motor 1
        int angleMotor2_7 = 45;  // Ángulo para motor 2
        int angleMotor3_7 = 60; // Ángulo para motor 3
        int angleEffector_7 = 100; // Ángulo para el efector final
        moveToPosition(angleMotor1_7, angleMotor2_7, angleMotor3_7, angleEffector_7, 20);
        delay(1000);
        //la octava trayectoria
        int angleMotor1_8 = 60;  // Ángulo para motor 1
        int angleMotor2_8 = 45;  // Ángulo para motor 2
        int angleMotor3_8 = 60; // Ángulo para motor 3
        int angleEffector_8 = 50; // Ángulo para el efector final
        moveToPosition(angleMotor1_8, angleMotor2_8, angleMotor3_8, angleEffector_8, 20);
      }
      String response = "{\"cantA\": " + String(cantA) + ", \"cantB\": " + String(cantB) + "}";
      server.send(200, "application/json", response);
    }
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  // Inicializar los servos
  servo1.attach(18); // Conectar el primer servo al pin GPIO 18
  servo2.attach(19); // Conectar el segundo servo al pin GPIO 19
  servo3.attach(21); // Conectar el tercer servo al pin GPIO 21
  effector.attach(22); // Conectar el cuarto servo al pin GPIO 22

  // Conectar a la red Wi-Fi con SSID y contraseña
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  
  Serial.println("Conectado a la red WiFi.");
  Serial.println(WiFi.localIP());
  
  server.on("/", handleRoot);
  server.on("/update", HTTP_POST, handleUpdate);
  server.on("/routine", HTTP_POST, handleRoutine);
  server.on("/capture_low", handleCaptureLow);  
  server.on("/capture_mid", handleCaptureMid);  
  server.on("/capture_high", handleCaptureHigh);
  server.begin();
}

void loop() {
  server.handleClient();
}

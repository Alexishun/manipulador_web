#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ESP32Servo.h>

// Configuración de WiFi
const char* ssid = "WIN UNI";
const char* password = "0123456789";

// Configuración de Firebase
#define FIREBASE_HOST "manipuladorweb-c1b90-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "U0UNLBmYWQQDvqQRuOyYsnrzKB5KaMNxqtQTqmG6"

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

// Servomotor
Servo gripper;
Servo servo1;
Servo servo2;
int angserv1;
int angserv2;
int anggripper;
// Variables para el control
int PWM_RESOLUTION = 1023; 

int M1_encoderA = 34; 
int M1_encoderB = 35; 
volatile long M1_enc_count = 0;
byte M1_A_old = 0;
byte M1_B_old = 0;

int M1_pin_PWM = 15; 
int M1_pin_AN1 = 2; 
int M1_pin_AN2 = 4;

int M2_encoderA = 32; 
int M2_encoderB = 33; 
volatile long M2_enc_count = 0;
byte M2_A_old = 0;
byte M2_B_old = 0;

int M2_pin_PWM = 5; 
int M2_pin_AN1 = 16; 
int M2_pin_AN2 = 17;

float M1_CPR = 5764; 
float M2_CPR = 5764; 
long te = 0;
volatile bool FLAG_TIMER = false;
hw_timer_t *timer = NULL;

float Tms = 20; // Tiempo de muestreo
float Ts = Tms / 1000; 
float Kp = 0.5; 
float Ki = 0.00;  
float Kd = 0.04; 

float outMax = 3.3; 
float outMin = -3.3;

float Input1, Output1, Setpoint1;
float ITerm1 = 0;
float lastInput1 = 0;

float Input2, Output2, Setpoint2;
float ITerm2 = 0;
float lastInput2 = 0;

float M1_th_ref = 0;
float M1_ang_old = 0;

float M2_th_ref = 0;
float M2_ang_old = 0;

// Variables para control de tiempo
unsigned long previousMillis = 0;
const long interval = 500; 

// Funciones
void IRAM_ATTR onTimer();
void streamCallback(StreamData data);
void streamTimeoutCallback(bool timeout);
void send_float(float arg);
int set_direccion(int val_pwm, int pin_AN1, int pin_AN2);
void IRAM_ATTR M1_encoder_isr();
void IRAM_ATTR M2_encoder_isr();

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
  if (!Firebase.beginStream(firebaseData, "/sensorData")) {
    Serial.println("Could not begin stream");
    Serial.println("REASON: " + firebaseData.errorReason());
    while (true);
  }
  Firebase.setStreamCallback(firebaseData, streamCallback, streamTimeoutCallback);

  ledcSetup(0, 7800, 10);
  ledcSetup(1, 7800, 10); 
  ledcAttachPin(M1_pin_PWM, 0);
  pinMode(M1_pin_AN1, OUTPUT);
  pinMode(M1_pin_AN2, OUTPUT);

  pinMode(M1_encoderA, INPUT);
  pinMode(M1_encoderB, INPUT);

  ledcAttachPin(M2_pin_PWM, 1);
  pinMode(M2_pin_AN1, OUTPUT);
  pinMode(M2_pin_AN2, OUTPUT);

  pinMode(M2_encoderA, INPUT);
  pinMode(M2_encoderB, INPUT);
  M1_A_old = digitalRead(M1_encoderA);
  M1_B_old = digitalRead(M1_encoderB);
  M2_A_old = digitalRead(M2_encoderA);
  M2_B_old = digitalRead(M2_encoderB);
  attachInterrupt(digitalPinToInterrupt(M1_encoderA), M1_encoder_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(M1_encoderB), M1_encoder_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(M2_encoderA), M2_encoder_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(M2_encoderB), M2_encoder_isr, CHANGE);
  timer = timerBegin(0, 80, true); 
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 5000, true); // Configura el temporizador para 5 ms
  timerAlarmEnable(timer);

  //gripper.attach();
  servo2.attach(23);
  servo1.attach(18);
}

void loop() {
  servo1.write(angserv1);
  servo2.write(angserv2);
  
  if (FLAG_TIMER) {
    FLAG_TIMER = false;
    noInterrupts();
    float M1_counter_encoder = M1_enc_count;
    float M2_counter_encoder = M2_enc_count;
    interrupts();
    float M1_ang = (M1_counter_encoder / M1_CPR) * 2 * PI;
    float M1_delta_ang = M1_ang - M1_ang_old;
    float M1_w = M1_delta_ang / Ts;
    M1_ang_old = M1_ang;

    float M2_ang = (M2_counter_encoder / M2_CPR) * 2 * PI;
    float M2_delta_ang = M2_ang - M2_ang_old;
    float M2_w = M2_delta_ang / Ts;
    M2_ang_old = M2_ang;

    //--------------------------------------------------------------//
    // B. APLICAMOS EL CONTROL PID PARA M1
    //--------------------------------------------------------------//
    Input1 = M1_ang;
    Setpoint1 = M1_th_ref;
    float error1 = Setpoint1 - Input1;
    float PTerm1 = Kp * error1;
    ITerm1 += Ki * Ts * error1;
    if (ITerm1 > outMax)
      ITerm1 = outMax;
    else if (ITerm1 < outMin)
      ITerm1 = outMin;
    float DTerm1 = Kd * M1_w;
    Output1 = PTerm1 + ITerm1 - DTerm1;
    if (Output1 > outMax)
      Output1 = outMax;
    else if (Output1 < outMin)
      Output1 = outMin;
    lastInput1 = Input1;

    //--------------------------------------------------------------//
    // C. GENERAMOS EL PWM CORRESPONDIENTE PARA M1
    //--------------------------------------------------------------//
    int M1_pwm_motor = Output1 * (PWM_RESOLUTION / outMax);
    M1_pwm_motor=M1_pwm_motor>0.1?50+M1_pwm_motor:M1_pwm_motor;
    M1_pwm_motor=M1_pwm_motor<0.1?M1_pwm_motor-50:M1_pwm_motor;
    if(abs(M1_pwm_motor)>1023){
      if(M1_pwm_motor<0) M1_pwm_motor=-1023;
      else M1_pwm_motor=1023;
    }
    if(abs(error1)<0.01){
      M1_pwm_motor=0;
      error1=0;
      M1_ang=Setpoint1;
    }
    M1_pwm_motor = set_direccion(M1_pwm_motor, M1_pin_AN1, M1_pin_AN2);
    ledcWrite(0, M1_pwm_motor);

    //--------------------------------------------------------------//
    // B. APLICAMOS EL CONTROL PID PARA M2
    //--------------------------------------------------------------//
    Input2 = M2_ang;
    Setpoint2 = M2_th_ref;
    float error2 = Setpoint2 - Input2;
    float PTerm2 = Kp * error2;
    ITerm2 += Ki * Ts * error2;
    if (ITerm2 > outMax)
      ITerm2 = outMax;
    else if (ITerm2 < outMin)
      ITerm2 = outMin;
    float DTerm2 = Kd * M2_w;
    Output2 = PTerm2 + ITerm2 - DTerm2;
    if (Output2 > outMax)
      Output2 = outMax;
    else if (Output2 < outMin)
      Output2 = outMin;
    lastInput2 = Input2;

    //--------------------------------------------------------------//
    // C. GENERAMOS EL PWM CORRESPONDIENTE PARA M2
    //--------------------------------------------------------------//
    int M2_pwm_motor = Output2 * (PWM_RESOLUTION / outMax);
    M2_pwm_motor=M2_pwm_motor>0.1?50+M2_pwm_motor:M2_pwm_motor;
    M2_pwm_motor=M2_pwm_motor<0.1?M2_pwm_motor-50:M2_pwm_motor;
    if(abs(M2_pwm_motor)>1023){
      if(M2_pwm_motor<0) M2_pwm_motor=-1023;
      else M2_pwm_motor=1023;
    }
    if(abs(error2)<0.01){
      M2_pwm_motor=0;
      error2=0;
      M2_ang=Setpoint2;
    }
    M2_pwm_motor = set_direccion(M2_pwm_motor, M2_pin_AN1, M2_pin_AN2);
    ledcWrite(1, M2_pwm_motor);  // Asegúrate de usar el canal 1 para 

    //Serial.print("M1_ang: ");
    Serial.println(M1_ang);
    //Serial.print("M2_Output: ");
    Serial.println(Output1);
  }
  delay(10);
}

void IRAM_ATTR M1_encoder_isr() {
  long t1 = micros();
  byte A = digitalRead(M1_encoderA);
  byte B = digitalRead(M1_encoderB);

  if (M1_A_old == 0 && M1_B_old == 0) {
    if (A == 1)
      M1_enc_count++;
    if (B == 1)
      M1_enc_count--;
  } else if (M1_A_old == 0 && M1_B_old == 1) {
    if (A == 1)
      M1_enc_count--;
    if (B == 0)
      M1_enc_count++;
  } else if (M1_A_old == 1 && M1_B_old == 0) {
    if (A == 0)
      M1_enc_count--;
    if (B == 1)
      M1_enc_count++;
  } else if (M1_A_old == 1 && M1_B_old == 1) {
    if (A == 0)
      M1_enc_count++;
    if (B == 0)
      M1_enc_count--;
  }
  M1_A_old = A;
  M1_B_old = B;
  long t2 = micros();
  te = t2 - t1;
}

void IRAM_ATTR M2_encoder_isr() {
  long t1 = micros();
  byte A = digitalRead(M2_encoderA);
  byte B = digitalRead(M2_encoderB);

  if (M2_A_old == 0 && M2_B_old == 0) {
    if (A == 1)
      M2_enc_count++;
    if (B == 1)
      M2_enc_count--;
  } else if (M2_A_old == 0 && M2_B_old == 1) {
    if (A == 1)
      M2_enc_count--;
    if (B == 0)
      M2_enc_count++;
  } else if (M2_A_old == 1 && M2_B_old == 0) {
    if (A == 0)
      M2_enc_count--;
    if (B == 1)
      M2_enc_count++;
  } else if (M2_A_old == 1 && M2_B_old == 1) {
    if (A == 0)
      M2_enc_count++;
    if (B == 0)
      M2_enc_count--;
  }
  M2_A_old = A;
  M2_B_old = B;
  long t2 = micros();
  te = t2 - t1;
}

void IRAM_ATTR onTimer() {
  FLAG_TIMER = true;
}

int set_direccion(int val_pwm, int pin_AN1, int pin_AN2) {
  if (val_pwm == 0) {
    digitalWrite(pin_AN1, 1);
    digitalWrite(pin_AN2, 1);
  } else if (val_pwm > 0) {
    digitalWrite(pin_AN1, 1);
    digitalWrite(pin_AN2, 0);
  } else {
    val_pwm = -val_pwm;
    digitalWrite(pin_AN1, 0);
    digitalWrite(pin_AN2, 1);
  }
  return val_pwm;
}

void streamCallback(StreamData data) {
  Serial.println("Stream data received");
    if (Firebase.get(firebaseData, "/sensorData/angQ1")) {
    int ang1=firebaseData.intData();
    M1_th_ref=(4.0*PI*ang1*12)/(66.88*20);
    Serial.println(M1_th_ref);
  }
  delay(10);
  if (Firebase.get(firebaseData, "/sensorData/angQ2")) {
    int ang2=firebaseData.intData();
    M2_th_ref=ang2*13.22/180;
  }
  delay(10);
  if (Firebase.get(firebaseData, "/sensorData/angQ3")) {
    int ang3=firebaseData.intData();
    angserv1=ang3;
  }
  delay(10);
  if (Firebase.get(firebaseData, "/sensorData/angQ4")) {
    int ang4=firebaseData.intData();
    angserv2=ang4;
  }
}

void streamTimeoutCallback(bool timeout) {
  if (timeout) {
    Serial.println("Stream timeout, reconnecting...");
    Firebase.beginStream(firebaseData, "/sensorData");
  }
}
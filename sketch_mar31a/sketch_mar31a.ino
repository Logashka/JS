//Подключаем необходимые библиотеки
#include <Adafruit_CircuitPlayground.h>
#include <Adafruit_Circuit_Playground.h>
#include <DHT.h>
#include <DHT_U.h>
#include <MQ2.h>
#include <TroykaMQ.h>
#include <Servo.h>
#include <Ultrasonic.h>

//Объявлям пины подключения
#define SmokePin A0 // Датчик широкого спектра газов
#define CoolerPin 2 // Вентилятор охлаждения
#define TemperaturePin 3 // Датчик температуры и влажности
#define DoorTrigPin2 4 // Ультразвуковой дальномер
#define DoorEchoPin2 5 // Ультразвуковой дальномер
#define SignalisationPin 6 //кнопка
#define BuzzerPin 7 // Зуммер
#define WatherPin 8 // Датчик влажности почвы
#define WatherServoPin 9 // Микросервопривод
#define DoorServoPin 10 // Микросервопривод
#define DoorTrigPin 11 // Ультразвуковой дальномер
#define DoorEchoPin 12 // Ультразвуковой дальномер
#define PeltierPin 13 // Элемент Пельтье

// создаём объекты для работы 
MQ2 mq2(SmokePin);
Servo WatherServo;
Servo DoorServo;
Ultrasonic DoorUltrasonic(DoorTrigPin, DoorEchoPin);
Ultrasonic DoorUltrasonic2(DoorTrigPin2, DoorEchoPin2);
DHT TemperatureSensor(TemperaturePin, DHT11);

void setup() {
  //назначаеи частоту serial-порта
  Serial.begin(9600);

  //Выставляем режимы работы пинов
  pinMode(SmokePin, INPUT);// Датчик широкого спектра газов
  pinMode(WatherPin, INPUT);// Датчик влажности почвы
  pinMode(DoorTrigPin2, INPUT);// Ультразвуковой дальномер
  pinMode(DoorTrigPin1, INPUT);// Ультразвуковой дальномер
  pinMode(SignalisationPin,INPUT); // Кнопка
  pinMode(WatherPin, INPUT); // Датчик влажности почвы
  pinMode(TemperaturePin, INPUT) // Датчик ткмпературы и влажности
  pinMode(WatherServoPin, OUTPUT);// Микросервопривод
  pinMode(BuzzerPin, OUTPUT);// Зуммер
  pinMode(DoorTrigPin2, OUTPUT);// Ультразвуковой дальномер
  pinMode(DoorTrigPin1, OUTPUT);// Ультразвуковой дальномер
  pinMode(DoorServoPin, OUTPUT);// Микросервопривод
  pinMode(PeltierPin, OUTPUT)
  // Калибровка микросервопривода
  WatherServo.attach(WatherServoPin);
  WatherServo.write(0);
  DoorServo.attach(WatherServoPin);
  DoorServo.write(0);
  //проводим калибровку датчика широкого спектра газов
  delay(60000);
  mq2.calibrate();
  //Запуск датчика температуры и влажности
  TemperatureSensor.begin();
}

int SmokeNormal = mq2.readSmoke(); // Задаём нормальное значение для датчика широкого спектра газов
int StatusSignalisation = 0; // Задаём начальный статус сигнализации

//Функции
void SmokeSensor() { //Если на складу произошло задымление, то подать звуковой сигнал(Линейная функция)
  Serial.print("\tSmoke: ");
  Serial.print(mq2.readSmoke());
  if (mq2.readSmoke() >= 20) {
    tone(BuzzerPin, 2000);
  }
  else {
    noTone(BuzzerPin);
  }
}
void WatherSensor(){ // Если произошло затопление подсмобного помещения, то открыть водосток и подать звуковой сигнал(Линейная функция)
  if( analogRead(WatherPin) >= 100){
    tone(BuzzerPin, 2000);
    WatherServo.write(180);
  }
  else{
    noTone(BuzzerPin);
    WatherServo.write(0);
  }
}
void Signalisation(){ // Если нажата кнопка, то поставить магазин на сигналицаю путём блокировки дверей(Линейная функция) 
  if(digitalRead(SignalisationPin) == 1){
    StatusSignalisation ++;
  }
}
void Cooling(){ // Если в торговом зале жарко, то включаем охлаждение(Линейная функция)
  float t = dht.readTemperature();
  if ( t > 28) {
    digitalWrite(CoolerPin, 1);
  }
  else{
    digitalWrite(CoolerPin, 0);
  }
}
void Heating(){ // Если в торговом зале холодно, то аключаем обогрев(Линейная функция)
  float t = dht.readTemperature();
  if ( t < 18) {
    digitalWrite(PeltierPin, 1);
  }
  else{
    digitalWrite(PeltierPin, 0);
  }
}
void DoorSensor(){ //Если кто- то подходит к входной двери и сагазин не на сигнализации, то открыть входную дверь(Многофакторная функция)
  float distCm = DoorUltrasonic.Ranging(CM);
  float distCm2 = DoorUltrasonic2.Ranging(CM);
  if ( (distCm < 7 && StatusSignalisation%2 != 1) || (distCm2 < 7 && StatusSignalisation%2 != 1) ){
    DoorServo.write(180);
  }
  else{
    DoorServo.write(0);
  }
}
void OutLight(){ // Если на улице тёмное время суток и кто то подходит к входной двери, то включить освещение(Многофакторная функция)
  float distCm = DoorUltrasonic.Ranging(CM);
  if (analogRead(LightSensorPin)>600 && distCm < 7){
    digitalWrite(LightPin, 1);
  }
  else{
    digitalWrite(LightPin, 0);
  }
}

void loop() {
  //Линейные функции
  SmokeSensor();
  WatherSensor();
  Signalisation();
  Cooling();
  Heating();

  //Многофакторные фуекции
  DoorSensor();
  OutLight();
}

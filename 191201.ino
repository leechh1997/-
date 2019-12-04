#include <Servo.h>          //馬達 library
#include <Wire.h>           //I2C library
#include <BH1750.h>         //Light sensor library
#include <SoftwareSerial.h> //BT library
char num;                   // num = 字元

BH1750 lightMeter1(0x23);  // I2C address
BH1750 lightMeter2(0x5c);  // I2C address
SoftwareSerial BT(10, 11); // RX , TX
Servo mysevro;             // sevro = mysevro

int IR_SensorTOP = 6; //上層紅外線偵測PIN6
int IR_SensorBOT = 7; //底部紅外線偵測PIN7

int downState = 0; //紅外線開關狀態
int upState = 1;   //紅外線開關狀態

int Auto = 0; // 0 = 手動 , 1 = 自動 , 2 = clock mode


void setup()
{
  Serial.begin(9600);           //usb baud rate
  BT.begin(9600);               //bluetooth baud rate
  mysevro.attach(9, 500, 2400); //馬達讀取PIN9 , 修正PWM訊號寬度 500,2400
  pinMode(IR_SensorTOP, INPUT); //IR1 input
  pinMode(IR_SensorBOT, INPUT); //IR2 input

  Wire.begin(); // I2C debug
  if (lightMeter1.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
  { // light sensor 初始化
    Serial.println(F("BH1750 Advanced begin-lux1"));
  }
  else
  {
    Serial.println(F("Error initialising - LuxSensor1 missing")); //error
  }
  if (lightMeter2.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
  { // light sensor 初始化
    Serial.println(F("BH1750 Advanced begin-lux2"));
  }
  else
  {
    Serial.println(F("Error initialising - LuxSensor2 missing")); //error
  }
}
void loop()
{
  float lux1 = lightMeter1.readLightLevel(); // 浮點數 lightMeter1 = lux1
  float lux2 = lightMeter2.readLightLevel(); // 浮點數 lightMeter1 = lux2
  int x1 = digitalRead(IR_SensorTOP); //x1 = 上層紅外線開關
  int x2 = digitalRead(IR_SensorBOT); //x2 = 底部紅外線開關

  Serial.println("Begining loop");
  Serial.print("x1 =");
  Serial.println(x1);
  Serial.print("x2 =");
  Serial.println(x2);
  Serial.print("lux1 =");
  Serial.println(lux1);
  Serial.print("lux2 =");
  Serial.println(lux2);
  Serial.print("downState =");
  Serial.println(downState);
  Serial.print("upState =");
  Serial.println(upState);
  Serial.print("Auto =");
  Serial.println(Auto);
  Serial.println("===================");
  Serial.print("numBT =");
  Serial.println(BT.available());
  Serial.println("===================");
  Serial.print("test0\n");

  delay(1000);

  //====================Switch========================
  if (BT.available() > 0)
  {
    Serial.println("test3");
    num = BT.read(); // 讀取字元
    Serial.println("num = ");
    Serial.println(num);

    if (num == 97)
    { // "a" 自動
      Auto = 1;
      delay(200);
    }

    else if (num == 98)
    { // "b" 取消自動
      Auto = 0;
      upState = 0;
      downState = 0;
      Serial.println("stop bt\n");
      delay(200);
    }

    else if (num == 99)
    {                    // "c"
      mysevro.write(85); //下降
      Serial.println("down");
      delay(200);
    }

    else if (num == 100)
    {                     // "d"
      mysevro.write(102); //上升
      Serial.println("up");
      delay(200);
    }

    else if (num == 101)
    {                    // "e"
      mysevro.write(90); //停止
    }

    else if (num == 102)
    {                     //"f"
      Auto = 2;           // clock mode
      mysevro.write(102); //上升
      upState = 1;
      delay(200);
    }
  }

  //================Servo Limit=====================
  Serial.print("test1\n");
  if (Auto == 1 && downState == 1)
  {
    if (x2 == 0)
    { //底部紅外線輸出 = 0 , 停止馬達
      Serial.println("AutoStop1");
      mysevro.write(90); //停止馬達
      //return ;
    }
  }
  else if (Auto == 1 && upState == 1)
  {
    if (x1 == 1)
    { //上層紅外線輸出 = 1 , 停止馬達
      Serial.println("AutoStop2");
      mysevro.write(90); //停止馬達
      //return ;
    }
  }

  else if (Auto == 2 && upState == 1)
  { // clock mode
    if (x1 == 1)
    { //上層紅外線輸出 = 1 , 停止馬達
      Serial.println("AutoStop3");
      mysevro.write(90); //停止馬達
      delay(1000);
      Auto = 0; //切回手動
      upState = 0;
      //return ;
    }
  }

  //=======================Auto=======================

  if (Auto == 1)
  {
    Serial.println("test2");
    if (abs(lux1 + 100 - lux2) <= 50)
    { // 絕對值(室內+100 - 室外) <= 50
      mysevro.write(90);
      delay(1000);
    }
    else if (lux1  > lux2 + 500)
    { // 室內 > 室外 + 500
      
      Serial.println("AutoDown");
      if(x2 != 0) 
      {
        mysevro.write(85);
        downState = 1;
      }
        
      delay(1000);
    }
    else if (lux1+500  <= lux2 )
    { // 室內+500 <= 室外
      
      Serial.println("AutoUp");
      if(x1!=1)
      {
        mysevro.write(102);
        upState = 1;
      }
        
      delay(1000);
    }

  }

}

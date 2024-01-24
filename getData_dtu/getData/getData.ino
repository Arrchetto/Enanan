#include "DFRobot_AS7341.h"
#include "Wire.h"
#include <WiFi.h>
#include <WebServer.h>
#include "html.h"

TwoWire myWire(0);
#define MY_SDA_PIN 10
#define MY_SCL_PIN 11
#define LENGTH 5
#define B1 255

HardwareSerial cardSerial(1);//声明串口1
WebServer server(80);
const char* ssid = "ESP3232";        /*Enter Your SSID*/
const char* password = "ESP323232"; /*Enter Your Password*/


DFRobot_AS7341 as7341(&myWire);  // 声明为全局变量
float alpha = 0.8; // 低通滤波系数
int beta = 10;//抖动滤波参数

int light;//灯的亮度

int a;//调度器

float R,G,B,r,g,b;

float X,Y,Z,R1,G1;

int index1,sum5,sum7,index7;
int buffer5[LENGTH],buffer7[LENGTH];

int F1value,F1value_old,F1value_now,delta_F1;
int F2value,F2value_old,F2value_now,delta_F2;
int F3value,F3value_old,F3value_now,delta_F3;
int F4value,F4value_old,F4value_now,delta_F4;
int F5value,F5value_old,F5value_now,delta_F5,F5value_1;
int F6value,F6value_old,F6value_now,delta_F6;
int F7value,F7value_old,F7value_now,delta_F7,F7value_1;
int F8value,F8value_old,F8value_now,delta_F8;

float temperature;

int clear,clear_old,clear_now;
int nir,nir_old,nir_now;

struct IntegralTerms {
  int F1;
  int F2;
  int F3;
  int F4;
  int F5;
  int F6;
  int F7;
  int F8;
};

DFRobot_AS7341::sModeOneData_t data1;
DFRobot_AS7341::sModeTwoData_t data2;

String jsonCode;

void MainPage() {
  String _html_page = html_page;              /*Read The HTML Page*/
  server.send(200, "text/html", _html_page);  /*Send the code to the web server*/
}

void Colors() {

String data = "[\""+String(F1value_now)+"\",\""+String(F2value_now)+"\",\""+String(F3value_now)+"\",\""+
                String(F4value_now)+"\",\""+String(F5value_now)+"\",\""+String(F6value_now)+"\",\""+
                String(F7value_now)+"\",\""+String(F8value_now)+"\",\""+
                String(clear_now)+"\",\""+String(nir_now)+"\",\""+String(temperature)+"\",\""+
                String(R1)+"\",\""+String(G1)+"\",\""+String(B1)+"\"]";

server.send(200, "text/plane", data);

}

void uplight(){

  ledcWrite(0, r*0.99);
  ledcWrite(1, g*0.99);
  ledcWrite(2, b*0.99);
  server.send(200, "text/plain", "up");

}

void XYZ_CAL(){
    
    double var_R,var_G,var_B;

    X=(0.39814*F1value_now+ 1.29540*F2value_now+ 0.36956*F3value_now+ 0.10902*F4value_now+ 0.71942*F5value_now+ 1.78180*F6value_now+ 1.10110*F7value_now -0.03991*F8value_now -0.27597*clear_now -0.02347*nir_now)/100;
    Y=(0.01396*F1value_now+ 0.16748*F2value_now+ 0.23538*F3value_now+ 1.42750*F4value_now+ 1.88670*F5value_now+ 1.14200*F6value_now+ 0.46497*F7value_now -0.02702*F8value_now -0.24468*clear_now -0.01993*nir_now)/100;
    Z=(1.95010*F1value_now+ 6.45490*F2value_now+ 2.78010*F3value_now+ 0.18501*F4value_now+ 0.15325*F5value_now+ 0.09539*F6value_now+ 0.10563*F7value_now+ 0.08866*F8value_now -0.61140*clear_now -0.00938*nir_now)/100;

    var_R = X * 3.2406 + Y * -1.5372 + Z * -0.4986;
    var_G = X * -0.9689 + Y * 1.8758 + Z * 0.0415;
    var_B = X * 0.0557 + Y * -0.2040 + Z * 1.0570;

    if ( var_R > 0.0031308 ) {
 var_R = 1.055 * pow(var_R,1/2.4) - 0.055;
}
else {
  var_R = 12.92 * var_R;
}

if ( var_G > 0.0031308 ) {
  var_G = 1.055 * pow(var_G,1/2.4) - 0.055;
 }
else{
  var_G = 12.92 * var_G;
}

if ( var_B > 0.0031308 ) {
  var_B = 1.055 * pow(var_B,1/2.4) - 0.055;
}
else{
  var_B = 12.92 * var_B;
}

  R1 = (var_R/var_B) * 255;
  G1 = (var_G/var_B) * 255;

  //Serial.println(R1);

}

void downlight(){

  ledcWrite(0, r*1.01);
  ledcWrite(1, g*1.01);
  ledcWrite(2, b*1.01);
  server.send(200, "text/plain", "down");
}

void handleReceiveInputs() {

  float highlight;

  String inputR = server.arg("input1");
  String inputG = server.arg("input2");
  String inputB = server.arg("input3");;
  String input4 = server.arg("input4");

  R = inputR.toFloat();
  G = inputG.toFloat();
  B = inputB.toFloat();
  highlight = input4.toFloat();

 // request->send(200, "text/plain", "Inputs received");

  Serial.println(inputR);
  Serial.println(inputG);
  Serial.println(inputB);
  Serial.println(input4);

  r = 1024 - highlight*R*1024;
  g = 1024 - highlight*G*1024;
  b = 1024 - highlight*B*1024;

  ledcWrite(0, r);
  ledcWrite(1, g);
  ledcWrite(2, b);

}

int32_t FilterJitter(int x, int oldy, int threshold)//当前输入值 上次输入值 阈值
{
    int32_t result;
    
    result = x - oldy;
    if (result >= threshold)
    {
        return x - threshold;
    }
    else if (result <= -threshold)
    {
        return x + threshold;
    }
    
    return oldy;
}

int sum_f5(){   

    for (int i = 0; i < LENGTH; ++i) {
        sum5 += buffer5[i];
    }
    sum5 = sum5 / LENGTH;

    return sum5;
}

int sum_f7(){   

    for (int i = 0; i < LENGTH; ++i) {
        sum7 += buffer7[i];
    }
    sum7 = sum7 / LENGTH;

    return sum7;
}

void NewDataF5(int data)
{
	buffer5[index1++] = data;
	index1 %= LENGTH;
}

void NewDataF7(int data)
{
	buffer7[index7++] = data;
	index7 %= LENGTH;
}

String buildJson2() {
  jsonCode = "{";

  // 构建 JSON 字符串
  jsonCode += "\"temperature1\":" + String(F5value_now) + ",";
  jsonCode += "\"humanity1\":" + String(F7value_now) + ",";
  jsonCode += "\"batteryLevel1\":" + String(clear_now);

  jsonCode += "}\n";

  return jsonCode;
}

void setup(void)
{
  myWire.begin(MY_SDA_PIN, MY_SCL_PIN);
  cardSerial.begin(115200,SERIAL_8N1,12,13);//rt

  pinMode(4, OUTPUT);//R 
  pinMode(5, OUTPUT);//G
  pinMode(6, OUTPUT);//B 

  ledcSetup(0, 5000, 10);
  ledcSetup(1, 5000, 10);
  ledcSetup(2, 5000, 10);
  
  ledcAttachPin(4, 0);     //输出信号的GPIO，产生信号的通道。
  ledcAttachPin(5, 1);    //输出信号的GPIO，产生信号的通道。
  ledcAttachPin(6, 2);     //输出信号的GPIO，产生信号的通道。

  Serial.begin(115200);
  // Detect if IIC can communicate properly 
  while (as7341.begin() != 0) {
    Serial.println("IIC init failed, please check if the wire connection is correct");
    delay(1000);
  }
  //Integration time = (ATIME + 1) x (ASTEP + 1) x 2.78µs
  as7341.setAtime(499);
  as7341.setAstep(499);
//  //设置增益(0~10对应 X0.5,X1,X2,X4,X8,X16,X32,X64,X128,X256,X512)
  as7341.setAGAIN(7);
//  //使能LED
  as7341.enableLed(false);
//  //设置引脚电流控制亮度(1~20对应电流 4mA,6mA,8mA,10mA,12mA,......,42mA)
  as7341.controlLed(10);

  WiFi.mode(WIFI_STA);        /*Set the WiFi in STA Mode*/
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  delay(1000);                /*Wait for 1000mS*/
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(200);
  } /*Wait while connecting to WiFi*/
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("Your Local IP address is: ");
  Serial.println(WiFi.localIP());       /*Print the Local IP*/

  server.on("/", MainPage);             /*Display the Web/HTML Page*/
  server.on("/readColors", Colors);     /*Display the updated Distance value(CM and INCH)*/
  server.on("/uplight", uplight);
  server.on("/downlight", downlight);
  server.on("/sendInputs", handleReceiveInputs);

  server.begin();                       /*Start Server*/

  Serial.print("OK");

  delay(1000);                          /*Wait for 1000mS*/

}

void loop(void)
{
  server.handleClient();
 // a++;
  // Channel mapping mode: 1.eF1F4ClearNIR,2.eF5F8ClearNIR
  //as7341.startMeasure(as7341.eF1F4ClearNIR);
  // Read the value of sensor data channel 0~5, under eF1F4ClearNIR
  as7341.startMeasure(as7341.eF5F8ClearNIR);
  // Read the value of sensor data channel 0~5, under eF5F8ClearNIR
    //data1 = as7341.readSpectralDataOne();
  data2 = as7341.readSpectralDataTwo();\

  NewDataF5(data2.ADF5);
  F5value_1 = sum_f5();

  F5value = alpha * F5value_1 + (1 - alpha) * F5value_old;
  F5value_now = FilterJitter(F5value, F5value_old, beta);
  if(data2.ADF5 == 0) F5value_now = 0;

  NewDataF7(data2.ADF7);
  F7value_1 = sum_f7();

  F7value = alpha * F7value_1 + (1 - alpha) * F7value_old;
  F7value_now = FilterJitter(F7value, F7value_old, beta);
  if(data2.ADF7 == 0) F7value_now = 0;

  clear = alpha * data2.ADCLEAR + (1 - alpha) * clear_old;
  clear_now = FilterJitter(clear, clear_old, beta);
  if(data2.ADCLEAR == 0) clear_now = 0;
  clear_old = clear_now;

  //XYZ_CAL();

  cardSerial.print(buildJson2());
  Serial.print(buildJson2());
  a = 0;
  }

  // 在 loop 函数中对每个通道进行滤波
/*F1value = alpha * data1.ADF1 + (1 - alpha) * F1value_old;
F2value = alpha * data1.ADF2 + (1 - alpha) * F2value_old;
F3value = alpha * data1.ADF3 + (1 - alpha) * F3value_old;
F4value = alpha * data1.ADF4 + (1 - alpha) * F4value_old;
F5value = alpha * data2.ADF5 + (1 - alpha) * F5value_old;
F6value = alpha * data2.ADF6 + (1 - alpha) * F6value_old;
F7value = alpha * data2.ADF7 + (1 - alpha) * F7value_old;
F8value = alpha * data2.ADF8 + (1 - alpha) * F8value_old;

clear = alpha * data2.ADCLEAR + (1 - alpha) * clear_old;
nir = alpha * data2.ADNIR + (1 - alpha) * nir_old;

F1value_now = FilterJitter(F1value, F1value_old, beta);
F2value_now = FilterJitter(F2value, F2value_old, beta);
F3value_now = FilterJitter(F3value, F3value_old, beta);
F4value_now = FilterJitter(F4value, F4value_old, beta);
F5value_now = FilterJitter(F5value, F5value_old, beta);
F6value_now = FilterJitter(F6value, F6value_old, beta);
F7value_now = FilterJitter(F7value, F7value_old, beta);
F8value_now = FilterJitter(F8value, F8value_old, beta);

clear_now = FilterJitter(clear, clear_old, beta);
nir_now = FilterJitter(nir, nir_old, beta);

if(data1.ADF1 == 0) F1value_now = 0;
if(data1.ADF2 == 0) F2value_now = 0;
if(data1.ADF3 == 0) F3value_now = 0;
if(data1.ADF4 == 0) F4value_now = 0;
if(data2.ADF5 == 0) F5value_now = 0;
if(data2.ADF6 == 0) F6value_now = 0;
if(data2.ADF7 == 0) F7value_now = 0;
if(data2.ADF8 == 0) F8value_now = 0;
if(data2.ADCLEAR == 0) clear_now = 0;
if(data2.ADNIR == 0) nir_now = 0;


// 更新旧值
F1value_old = F1value_now;
F2value_old = F2value_now;
F3value_old = F3value_now;
F4value_old = F4value_now;
F5value_old = F5value_now;
F6value_old = F6value_now;
F7value_old = F7value_now;
F8value_old = F8value_now;
clear_old = clear_now;
nir_old = nir_now;*/

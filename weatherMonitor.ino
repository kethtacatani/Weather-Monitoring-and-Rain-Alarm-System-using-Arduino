  #include <virtuabotixRTC.h>
#include <TFT.h>  
#include <SPI.h>
#include "DHT.h"
#include <math.h>

//for 
#define DHTPIN 2 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


//for tft
#define cs   10
#define dc   9
#define rst  8

virtuabotixRTC myRTC(5, 6, 7);


//for TFT
TFT tft = TFT(cs, dc, rst);
char tftPrintout[20];
//for rtc
char minutePrintout[4];
char hourPrintout[4];
char weekDayPrintout[10];
char dayPrintout[4];
char monthPrintout[4];
char yearPrintout[6];
char amPmPrintout[4];
//for temp and hum
char temperaturePrintout[6];
char humidityPrintout[6];
char heatIndexPrintout[6];
//for raindrop
char rainDetectPrintout[15];
char rainDetectPrintout2[15];
//testPrintout
char testPrintout[70];

//for RTC time
String secondsTime;
String minutesTime;
String hourTime;
String weekDayTime;
String dayTime;
String monthTime;
String yearTime;
String amPmTime;



//for windspeed 
const int maxSamples = 10;  // Number of samples to average
unsigned long rotationTimes[maxSamples];  // Array to store rotation times
int sampleCount = 0;  // Counter for the number of samples collected
float rpm = 0;   // Variable to store the time for one full rotation
unsigned long lastRotationTime = 0;  // Variable to store the time of the last rotation
const float decayRate = 0.65; 
float windSpeed = 0;
const float radius =8.0; 
unsigned long fullRotationTime = 0;

boolean detected = false;
      
//float rpm = 0;


//ldr rpm
int threshold=200;
int ldrRPM=A0;
int sensorValue=0;

//ldr clear sky
int ldrSky=A2;

//dht reads
int humidity;
int temperature;
int heatIndex;

//for raind drop
bool raining=false;

//buzzer
int buzzer = 4;
String playing= "";

//previousTime
unsigned long previousTime = -60000;
unsigned long windPreviousTime = -1100;
unsigned long windPreviousTime2 = 0;
unsigned long tempPreviousTime = -11000; 
unsigned long skyPreviousTime = -11000; 
unsigned long rainPreviousTime= -11000; 


void setup() {
Serial.begin(9600);   // 시리얼통신 초기화
pinMode(buzzer, OUTPUT); 
tft.begin();
dht.begin();
tft.background(0,0,0);
tft.setRotation(2);
 tft.setTextSize(1);
  tft.stroke(255,255,255);
  tft.text("BISU Calape Weather",2,2);
  tft.text("Temperature",3,65);
  tft.text("Sky",87,65);
  tft.text("Humidity",3,97);
  tft.text("Heat Index",67,97);
  tft.text("Wind Speed",3,128);
  tft.setTextSize(2);
    // seconds, minutes, hours, day of the week, day of the month, month, year
//myRTC.setDS1302Time(18, 39, 10, 2, 05, 06, 2023);
}

void loop() {
  

 //displayTFT();
  getTime();
    tft.setTextSize(2);
  checkWindSpeed();
  getTempHumidity();
  checkRain();
  getSkyState();
//delay(100);
//tft.fillScreen(0);
}




/////////////////////////////////////////////////////////////
void checkWindSpeed(){

  sensorValue=analogRead(ldrRPM); 
//    Serial.print("Sensor value ="); //Show “Sensor value=” on serial monitorde
// Serial.println(sensorValue);

  if(sensorValue>threshold && detected==false){
     detected=true;
  
      Serial.print("Sensor value ="); //Show “Sensor value=” on serial monitorde
 Serial.println(sensorValue);
    unsigned long currentTime = micros();  // Get the current time in microseconds
    rotationTimes[sampleCount % maxSamples] = currentTime;  // Store the rotation time in the array

    sampleCount++;  // Increment the sample count

    // Calculate the number of samples to use for averaging
    int numSamples = min(sampleCount, maxSamples);

    // Calculate the total rotation time
    unsigned long totalRotationTime = rotationTimes[(sampleCount - 1) % maxSamples] - rotationTimes[(sampleCount - numSamples) % maxSamples];

    // Calculate the average rotation time and convert it to RPM
    float averageRotationTime = (float)totalRotationTime / numSamples;
     rpm = 56858000.0 / averageRotationTime;
     

    lastRotationTime = millis(); 
    
    windSpeed = ((2 * 3.14159 * radius * rpm * 60) / 100000)*6;
    if (isnan(windSpeed) || isinf(windSpeed)) {
    windSpeed = 0.0; // Set the value to 0 if it is NaN (Not a Number) or infinity
    }
//  Serial.println("rpm is "+String(rpm));
//    Serial.println("Wind Speed:"+String(windSpeed)+"km/hr");
    String windSpeedStr=String(windSpeed)+"km/hr";
  windSpeedStr.toCharArray(tftPrintout,20);
    if(millis() - windPreviousTime <1000){
    return;
  }
  windPreviousTime=millis();
     tft.fillRect(10,140, 117, 15, 0);
    tft.text(tftPrintout,10,140);
   
   
  }
  else if (sensorValue<threshold){
    detected=false;
  }

  
  if(millis() - windPreviousTime2 <1000){
    return;
  }
  windPreviousTime2=millis();
  
  if (millis() - lastRotationTime > 5000 && rpm > 1) {
    rpm *= decayRate;  // Apply the decay rate to gradually reduce the RPM
   
    windSpeed = ((2 * 3.14159 * radius * rpm * 60) / 100000)*6;
     if (isnan(windSpeed) || isinf(windSpeed)) {
      windSpeed = 0.0; // Set the value to 0 if it is NaN (Not a Number) or infinity
    }
//  Serial.println("rpm is "+String(rpm));
//    Serial.println("Wind Spd:"+String(windSpeed)+"km/hr");

  String windSpeedStr=String(windSpeed)+"km/hr";
    
   
  windSpeedStr.toCharArray(tftPrintout,20);

//
//String test=String(windPreviousTime2)+" prev";
//test.toCharArray(testPrintout,20);
//  tft.fillRect(70,128, 50, 9, 0);
//  tft.setTextSize(1);
//   tft.text(testPrintout,70,128);
//tft.setTextSize(2);


    tft.fillRect(10,140, 117, 15, 0);
    tft.text(tftPrintout,10,140);
    

  }

}

///////////////////////////////////////////////////////////////
void getTime(){
  
  if(millis() - previousTime <58000){
    return;
  }
  previousTime=millis();
  tft.fillRect(26, 19, 77, 41, 0);
 myRTC.updateTime();

  // Convert hours to 12-hour format
  int hours12 = myRTC.hours;
  bool isPM = false;
  if (hours12 >= 12) {
    isPM = true;
    if (hours12 > 12) {
      hours12 -= 12;
    }
  }
  if (hours12 == 0) {
    hours12 = 12;
  }

  
  int dayofweek=myRTC.dayofweek;
  if(dayofweek==1){
    weekDayTime="Sunday";
  }else if(dayofweek==2){
    weekDayTime="Monday";
  }else if(dayofweek==3){
    weekDayTime="Tuesday";
  }else if(dayofweek==4){
    weekDayTime="Wednesday";
  }else if(dayofweek==5){
    weekDayTime="Thursday";
  }else if(dayofweek==6){
    weekDayTime="Friday";
  }else if(dayofweek==7){
    weekDayTime="Saturday";
  }

int monthTimeInt=myRTC.month;
  if(monthTimeInt==1){
    monthTime="Jan";
  }else if(monthTimeInt==2){
    monthTime="Feb";
  }else if(monthTimeInt==3){
    monthTime="Mar";
  }else if(monthTimeInt==4){
    monthTime="Apr";
  }else if(monthTimeInt==5){
    monthTime="May";
  }else if(monthTimeInt==6){
    monthTime="Jun";
  }else if(monthTimeInt==7){
    monthTime="Jul";
  }else if(monthTimeInt==8){
    monthTime="Aug";
  }else if(monthTimeInt==9){
    monthTime="Sep";
  }else if(monthTimeInt==10){
    monthTime="Oct";
  }else if(monthTimeInt==11){
    monthTime="Nov";
  }else if(monthTimeInt==12){
    monthTime="Dec";
  }
  

//   secondsTime =String(myRTC.seconds);
  minutesTime = myRTC.minutes < 10? "0"+String(myRTC.minutes):String(myRTC.minutes);
  hourTime =hours12 < 10? "0"+String(hours12):hours12;
  dayTime =myRTC.dayofmonth < 10? "0"+String(myRTC.dayofmonth):String(myRTC.dayofmonth);
  yearTime =String(myRTC.year);
  amPmTime=isPM?"PM":"AM";
  tft.setTextSize(2);

    
    hourTime.toCharArray(hourPrintout,4);
    tft.text(hourPrintout,30,22);

    tft.text(":",58,24);

    minutesTime.toCharArray(minutePrintout,4);
    tft.text(minutePrintout,67,22);
    
    tft.setTextSize(1);
    amPmTime.toCharArray(amPmPrintout,4);
    tft.text(amPmPrintout,91,29);

    monthTime.toCharArray(monthPrintout,4);
    tft.text(monthPrintout,31,40);

    dayTime.toCharArray(dayPrintout,4);
    tft.text(dayPrintout,53,40);

    tft.text(",",63,41);

    yearTime.toCharArray(yearPrintout,6);
    tft.text(yearPrintout,75,40);

    int textWidth = weekDayTime.length() * 6; 
     int textX = (tft.width() - textWidth) / 2;
    weekDayTime.toCharArray(weekDayPrintout,10);
    tft.text(weekDayPrintout,textX,50);
}
////////////////////////////////////////////////
void getTempHumidity()
{

  if(millis() - tempPreviousTime <10000){
    return;
  }
  tempPreviousTime=millis();
//  if(isnan(humidity) || isnan(temperature) {
//    Serial.println("Failed to read from DHT sensor!");
//    return;
//  }
  
  temperature = int(dht.readTemperature());
  humidity = int(dht.readHumidity());
   float h = dht.readHumidity();
  float t = dht.readTemperature();
   float hic = dht.computeHeatIndex(t, h, false);
  heatIndex = round(hic);

  //clear
   tft.fillRect(13,78, 50, 16, 0);
   tft.fillRect(13,109, 50, 16, 0);
   tft.fillRect(77,109, 50, 16, 0);
  
  String temp= String(temperature)+"\367C";
  temp.toCharArray(temperaturePrintout,6);
    tft.text(temperaturePrintout,13,78);

String hum= String(humidity)+"%";
    hum.toCharArray(humidityPrintout,6);
    tft.text(humidityPrintout,13,109);
    
String heat= String(heatIndex)+"\367C";
    heat.toCharArray(heatIndexPrintout,6);
    tft.text(heatIndexPrintout,77,109);
}
////////////////////////////////////////////////
void checkRain(){

  if(millis() - rainPreviousTime <5000){
    return;
  }
  rainPreviousTime=millis();
 
    int value = analogRead(A3);//read value
   // Serial.println(value);
    String rainEq="";
    String rainEq1="Rain";

    bool isSound=false;
     tft.setTextSize(1);
     if (value < 1000){
      raining = true;
       tft.fillRect(64,75, 70, 19, 0);
     }
     else{
      raining = false;
      playing = "";
      return;
     }
     if(windSpeed > 25){
      rainEq="Light";
        soundBuzzer("Light",2);
        playing="Light";
     }
      
    else if (value < 1000 && value >500) {//check condition
      rainEq="Light";
        soundBuzzer("Light",2);
        playing="Light";
      
      
    }
    else if (value <= 500 && value > 300) {//check condition
      rainEq="Moderate";
      soundBuzzer("Moderate",3);
        playing="Moderate";
    }
    else if (value <= 300) {//check condition
      rainEq="Heavy";
      soundBuzzer("Heavy",4);
        playing="Heavy";
    } 
    
      int textWidth = rainEq.length() * 6;
      int startingX = (122 + 67) / 2 - textWidth / 2;
      rainEq.toCharArray(rainDetectPrintout,15);
      tft.text(rainDetectPrintout,startingX,75);

      textWidth = rainEq1.length() * 6;
      startingX = (122 + 67) / 2 - textWidth / 2;
      rainEq1.toCharArray(rainDetectPrintout2,15);
      tft.text(rainDetectPrintout2,startingX,83);
      
    
//    else{
//      int textWidth = rainEq.length() * 6;
//      int startingX = (122 + 67) / 2 - textWidth / 2;
//      rainEq.toCharArray(rainDetectPrintout,15);
//      tft.text(rainDetectPrintout,startingX,80);
//    }
}

void soundBuzzer(String playing2, int count){
      if(playing2!=playing){
        for(int i=0;i<count;i++){
        tone(buzzer, 400);  // Start the buzzer at a frequency of 1000 Hz
        delay(400);  // Keep the buzzer on for 0.5 seconds
        noTone(buzzer);  // Stop the buzzer
        delay(100);  // Delay for 0.5 seconds
        }
      }
}
////////////////////////////////////////////////////
void getSkyState(){
  tft.setTextSize(1);
  if(millis() - skyPreviousTime <10000){
    return;
  }
   tft.fillRect(64,75, 50, 19,0);
  skyPreviousTime=millis();

  if(raining){
    return;
  }
  String sky;
  int skyValue=analogRead(ldrSky); 
//  Serial.print("Sky ");
  Serial.println(skyValue);

  if(myRTC.hours<8 || myRTC.hours>16){
    skyValue *=1.10;
  } 
 
  if(skyValue>=1100 && skyValue<1200){
    sky="Sunny";
  }
  else if(skyValue>= 1000 && skyValue <1100){
    sky="Clear";
  }
  else if(skyValue>= 100 && skyValue <1000){
    sky="Cloudy";
  }
  else if(skyValue<100){
    sky="Dark";
  }
    int textWidth = sky.length() * 6;
      int startingX = (127 + 67) / 2 - textWidth / 2;
      sky.toCharArray(rainDetectPrintout,15);
      tft.text(rainDetectPrintout,startingX,80);
}

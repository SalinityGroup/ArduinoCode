#include <RTClib.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h> 
#include <SPI.h>                      
#define rx 5                         
#define tx 6
#define ONE_WIRE_BUS 7   
File myFile;
SoftwareSerial myserial(rx, tx);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
RTC_DS1307 rtc;
const int chipSelect = 10;
String inputstring = "";
String sensorstring = "";
boolean input_string_complete = false;
boolean sensor_string_complete = false;

//Set the time to take a single measurement
//Measurements will be taken at internals based on the current minute
int yr = 2021;
int mon = 7;
int dy = 30;
int hr = 20;
int mi = 12;
int time_interval = 1;
boolean measurement_check = true; //set true if measurement has not been done yet

void setup()  {
  Serial.begin(9600);
  myserial.begin(9600);
  sensors.begin();
  rtc.begin();
  SD.begin(10);
  inputstring.reserve(10);
  sensorstring.reserve(30);
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //Resets the time on the RTC to the time that the code was compiled
}

void serialEvent() {                                  //if the hardware serial port receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC
}

void loop() {                                    
    delay(10000);
    if(check_correct_time()&&measurement_check) {     //Check for correct time, and that measurement wasn't already done
      Serial.println("Taking measurement...");
      take_sal_and_temp();
      Serial.println("Recording Data...");
      record_EC_data();
    }
}

void take_sal_and_temp(void) {
 sensors.requestTemperatures(); // Send the command to get temperatures
    //Serial.println(sensors.getTempCByIndex(0));

  if (input_string_complete == true) {                //if a string from the PC has been received in its entirety
    myserial.print(inputstring);                      //send that string to the Atlas Scientific product
    myserial.print('\r');                             //add a <CR> to the end of the string
    inputstring = "";                                 //clear the string
    input_string_complete = false;                    //reset the flag used to tell if we have received a completed string from the PC
  }

  if (myserial.available() > 0) {                     //if we see that the Atlas Scientific product has sent a character
    char inchar = (char)myserial.read();              //get the char we just received
    sensorstring += inchar;                           //add the char to the var called sensorstring
    if (inchar == '\r') {                             //if the incoming character is a <CR>
      sensor_string_complete = true;                  //set the flag
    }
  }


  if (sensor_string_complete == true) {               //if a string from the Atlas Scientific product has been received in its entirety
    if (isdigit(sensorstring[0]) == false) {          //if the first character in the string is a digit
      Serial.println(sensorstring);                   //send that string to the PC's serial monitor
    }
    else                                              //if the first character in the string is NOT a digit
    {
      record_EC_data();                                //then call this function 
    }
    sensorstring = "";                                //clear the string
    sensor_string_complete = false;                   //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
  }
}

void record_EC_data(void) {                            //this function will pars the string  

  char sensorstring_array[30];                        //we make a char array
  char *EC;                                           //char pointer used in string parsing
  char *TDS;                                          //char pointer used in string parsing
  char *SAL;                                          //char pointer used in string parsing
  char *GRAV;                                         //char pointer used in string parsing
  float f_ec;                                         //used to hold a floating point number that is the EC
  
  sensorstring.toCharArray(sensorstring_array, 30);   //convert the string to a char array 
  EC = strtok(sensorstring_array, ",");               //let's pars the array at each comma
  TDS = strtok(NULL, ",");                            //let's pars the array at each comma
  SAL = strtok(NULL, ",");                            //let's pars the array at each comma
  GRAV = strtok(NULL, ",");                           //let's pars the array at each comma
  
  myFile = SD.open("test.txt", FILE_WRITE);
  DateTime now = rtc.now();
  myFile.print(now.year());
  myFile.print('/');
  myFile.print(now.month(), DEC);
  myFile.print('/');
  myFile.print(now.day(), DEC);
  myFile.print('_');
  myFile.print(now.hour(), DEC);
  myFile.print(':');
  myFile.print(now.minute(), DEC);
  myFile.print(':');
  myFile.print(now.second(), DEC);
  myFile.print(", ");
  myFile.print("EC:");                                //we now print each value we parsed separate
  myFile.print(EC);                                 //this is the EC value
  myFile.print(", Temp:");
  myFile.println(sensors.getTempCByIndex(0));
  myFile.close();
  measurement_check = false;
}

boolean check_correct_time(){
  DateTime now = rtc.now();
  Serial.print("Currently: ");
  Serial.println(now.minute());
  if(now.minute()%time_interval==0){
    Serial.println("Correct time!");
    if(!measurement_check)
      Serial.println("Measurement Already Completed!");
    return true;}
  Serial.println("Incorrect time!");
    measurement_check = true;
    return false;
}

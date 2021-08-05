#include<SPI.h>
#include<SD.h>

File myFile;

const int chipSelect = 10;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial){
    ;
  }

  Serial.print("Initalizing SD card...");

  if(!SD.begin(10)){
    Serial.println("Initialization failed!");
    while(1);
  }
  Serial.println("Initialization done.");

  //The following will open a new file
  myFile = SD.open("test.txt", FILE_WRITE);

  //Write to file
  if(myFile){
    Serial.print("Writing to test file");
    for(int counter = 0; counter < 1000; counter++){
      myFile.print(counter);
      myFile.print(" ");
    }
    myFile.close();
    Serial.println("Done.");
  } else{
    //Error message for file not opening...
    Serial.println("Error opening file");
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}

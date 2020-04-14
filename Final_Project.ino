/*
 * Fix The Internet! Program Code
 * Team DuckDuctTape
 * 
 * Program by: Matthew Low
 * Version 2.1
 * contact: Low.m@Northeastern.edu
 */

#include <ShiftOutX.h> //Helps with the shift registers
#include <ShiftPinNo.h> //helps with the SR pin numbers, otherwise they grow large and unweildly.

//shiftOutX(_latchPin, _dataPin, _clockPin, _bitOrder, _NofRegisters);
shiftOutX regOne(8,11,12,MSBFIRST,4);


//City class is designed to help manage data for each city induvidually. The cities are stored in an array of city objects.
class City {
private:
  //input and output pins are unique to each city
  //led state is 0 for off and 1 for green, 2 is for blinking red
  //failed indicates wheter the city works
  //The pin assignments are made unique to each city to allow easy adjustment later.
  int inputPin, outputPin, ledState;
  long redLedPin, greenLedPin;
  bool isFailed;
  String cityName;
  
  //This is the constructor function for the city class
 public: City(String citName, int input, int output, int led, bool failed, long redLed, long greenLed){
  cityName = citName; //name to send to MATLAB
  inputPin = input; //input pin num (On Arduino board)
  outputPin = output; //output pin num (On shift register), later decided to not use this.
  isFailed = failed;
  ledState = led; 
  redLedPin = redLed; //the red pin for this city on the shift register
  greenLedPin = greenLed; //green led pin on the shift register for this city. Tied to the output port for the city (thus no need for output pin)
  
  }
   //get and set functions below to help with managing the cities.
  public: int getInputPin(){
    return inputPin;
  }
  public: String getName(){
    return cityName;
  }
  public: long getOutputPin(){
    return outputPin;
  }
  public: int getLedState(){
    return ledState;
  }
  public: void setLedState(int state){
    ledState = state;
  }
  public: bool getIsFailed(){
    return isFailed;
  }
  public: long getRedLedPin(){
    return redLedPin;
  }
  public: long getGreenLedPin(){
    return greenLedPin;
  }  
//These functions fail or unfail a city,
//changing settings so code later can adjust outputs.
public: void failCity(){
    isFailed = true;
    ledState = 2;
  }

  public: void unfailCity(){
    isFailed = false;
    ledState = 0;
    regOne.pinOff(redLedPin);
  }
}; //end of class definition

static int NUM_CITIES = 8; 
//define each city, then create an array with each of these cities. The shPinXX refers to shift register pins and is defined in the imported library
City c1("Las Vegas",2,shPin1,0,false,shPin9,shPin17);
City c2("Spokane",4,shPin2,0,false,shPin10,shPin18);
City c3("Denver",3,shPin3,0,false,shPin11,shPin19);
City c4("Kansas City",5,shPin4,0,false,shPin12,shPin20);
City c5("Dallas",6,shPin5,0,false,shPin13,shPin21);
City c6("Chicago",7,shPin6,0,false,shPin14,shPin22);
City c7("Charlotte",9,shPin7,0,false,shPin15,shPin23);
City c8("Washington D.C.",10,shPin8,0,false,shPin16,shPin24);
//create the array of city objects for use later.
City cityArr[8] = {c1,c2,c3,c4,c5,c6,c7,c8};  //stores the cities
//more pin definitions.
int buttonPin = 13; //for reset purposes
int finishPin = A0; //the pin Boston is connected to so the board knows when the game is over
int bostonOutput = A3; //green LED for boston
bool bosState = false; //false when boston is disconnected
long count = 0;
void setup() {
regOne.allOff(); //make sure board is reset
pinMode(buttonPin, INPUT);
pinMode(bostonOutput, OUTPUT);
pinMode(c2.getInputPin(),INPUT);
pinMode(c3.getInputPin(),INPUT);
pinMode(c4.getInputPin(),INPUT);
pinMode(c5.getInputPin(),INPUT);
pinMode(c6.getInputPin(),INPUT);
pinMode(c7.getInputPin(),INPUT);
pinMode(c8.getInputPin(),INPUT);
pinMode(A1,INPUT);
pinMode(finishPin, INPUT);
Serial.begin(9600);
Serial.println("Initializing Game");
randomSeed(analogRead(A0));//random seed
for(int i = 0;i < NUM_CITIES;i++){
  Serial.print(cityArr[i].getName());
  Serial.print(" Failed: ");
  Serial.println(cityArr[i].getIsFailed());
}
}
void loop() { //Loops forever through game usage:
  int buttonState = digitalRead(buttonPin);
  if(buttonState == 1){ //make the cycle end if the button is pressed
    count = 45001;
  }
if((count>45000)/*or button, which makes this happen preemtivley*/){
  Serial.println("MATLAB Reset"); //command to reset the game on MATLAB screen
  Serial.println("Unfailing all cities");
  for(int i=0;i<NUM_CITIES;i++){
    if(cityArr[i].getIsFailed()){ 
    cityArr[i].unfailCity(); 
    Serial.println(cityArr[i].getName() + " has been unfailed sucessfully."); //debugging use
    }else{
      Serial.println(cityArr[i].getName() + " did not need to be unfailed.");
    }
  }//end for
  int randomNum;
  int loopCheck = 0;
  //makes sure the city to be failed is connected
  while(true){
  randomNum = random(1,8);
  loopCheck++;
  if(cityArr[randomNum].getLedState()==1 || loopCheck >= 5){
    break;
  }
}
  cityArr[randomNum].failCity();
  Serial.println("CITY " + cityArr[randomNum].getName());
  
  count = 0;
  
}//end if counter

//LED AND POWER LOGIC LOOP
for(int i = 0; i < NUM_CITIES; i++){
  //detects if city has become connected
if((digitalRead(cityArr[i].getInputPin())==HIGH)&&(cityArr[i].getIsFailed()==false)){
    if(cityArr[i].getLedState() == 0){
      Serial.print(cityArr[i].getName());
      Serial.println(" is now recieving power.");
      count = 0;
      cityArr[i].setLedState(1);
      regOne.pinOff(cityArr[i].getRedLedPin());
      regOne.pinOn(cityArr[i].getOutputPin());
      regOne.pinOn(cityArr[i].getGreenLedPin());
    }

    //detects if a city has become disconnected
  }else if((digitalRead(cityArr[i].getInputPin())==LOW)&&(cityArr[i].getIsFailed()==false)){
    if(cityArr[i].getLedState() == 1){
      Serial.print(cityArr[i].getName());
      Serial.print(" is no longer recieving input.");
      count = 0;
          cityArr[i].setLedState(0);
    regOne.pinOff(cityArr[i].getGreenLedPin());
    regOne.pinOff(cityArr[i].getRedLedPin());
    regOne.pinOff(cityArr[i].getOutputPin());
    }
  //makes sure led is red if the city is failed.
  }else if(cityArr[i].getIsFailed()==true){
    cityArr[i].setLedState(2);
    regOne.pinOn(cityArr[i].getRedLedPin());
    regOne.pinOff(cityArr[i].getGreenLedPin());
    regOne.pinOff(cityArr[i].getOutputPin());
    }
}//end for
int finishState = digitalRead(finishPin);
if((finishState == 1)&& (bosState==false)){
  digitalWrite(bostonOutput, HIGH);
  bosState = true;
  Serial.println("MATLAB BOSTON ON"); //command to tell MATLAB game is complete
 // count = 0;
}
if((finishState == 0)&&(bosState ==true)){
  digitalWrite(bostonOutput, LOW);
  bosState = false;
}

if(count%1000==0){
  Serial.println(count);
}
count+=500;
delay(500);
}

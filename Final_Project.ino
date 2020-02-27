//Including external libraries to assist the program.
#include <LiquidCrystal_I2C.h>
#include <ShiftOutX.h>
#include <ShiftPinNo.h>
//create an instance of shiftOutX to help with managing the shift registers.
//shiftOutX(_latchPin, _dataPin, _clockPin, _bitOrder, _NofRegisters);
shiftOutX regOne(8, 11, 12, MSBFIRST, 4);

//Create a class to help with managing the data for each city. 
class City {
private:
  //input and output pins are unique to each city
  //led state is 0 for red and 1 for green, 2 is for blinking red
  //failed indicates wheter the city works
  //The pin assignments are made unique to each city to allow easy adjustment later.
  int inputPin, outputPin, ledState, rank, redLedPin, greenLedPin;
  bool isFailed, outputState;
  
  //This is the constructor function
 public: City(int input, int output, int led, int r, bool failed, int redledP, int greenled){
  inputPin = input;
  outputPin = output;
  ledState = led;
  rank = r;
  isFailed = failed;
  redLedPin = redledP;
  greenLedPin = greenled;
  }
  
   //get and set functions below
  public: int getInputPin(){
    return inputPin;
  }

  public: int getOutputPin(){
    return outputPin;
  }

  public: int getLedState(){
    return ledState;
  }

  public: int getRank(){
    return rank;
  }

  public: bool getIsFailed(){
    return isFailed;
  }

  public: int getRedLedPin(){
    return redLedPin;
  }
  
  public: int getGreenLedPin(){
    return greenLedPin;
  }  

//these set functions shouldn't be nessesary for our purposes.
  //public: void setInputPin(int pin){
  //  inputPin = pin;
  //}

//  public: void setOutputPin(int pin){
 //   outputPin = pin;
 // }

//
  public: void setLedState(int state){
    ledState = state;
  }

 // public: void setRank(int r){
  //  rank = r;
 // }

 // public: void setIsFailed(bool fail){
  //  isFailed = fail;
  //}

 // public: void setLedPin(int pin){
 //   ledPin = pin;
 // }
//These functions fail or unfail a city, changing settings so code later can adjust outputs.
public: void failCity(){
    isFailed = true;
    ledState = 2;
    outputState = false;
  }

  public: void unfailCity(){
    isFailed = false;
    ledState = 0;
    outputState = true;
  }
}; //end of class definition

//Set to the number of cities in the array because C++ makes finding array lengths more difficult.
static int NUM_CITIES = 8; 

//define pinouts here
//City(input pin, output pin, led state, rank,is failed,red led pin, green led pin)
City c1(shPin1,A1,0,0,true,shPin9,shPin17);
City c2(shPin2,A2,0,1,false,shPin10,shPin18);
City c3(shPin3,A3,0,2,false,shPin11,shPin19);
City c4(shPin4,A0,0,3,false,shPin12,shPin20);
City c5(shPin5,7,0,4,false,shPin13,shPin21);
City c6(shPin6,8,0,5,false,shPin14,shPin22);
City c7(shPin7,9,0,6,false,shPin15,shPin23);
City c8(shPin8,6,0,7,false,shPin16,shPin24);
//create the array of city objects for use later.
City cityArr[8] = {c1,c2,c3,c4,c5,c6,c7,c8};

//more pin definitions.
int latchPin = 2;   // pin  12 on the 74hc595
int dataPin = 3;   // pin 14 on the 74hc595
int clockPin = 4;  // pin 11 on the 74hc595
int buttonPin = 5;
int finishPin = 10;


void setup(){
//defining pinmodes
pinMode(latchPin, OUTPUT);
pinMode(clockPin, OUTPUT);
pinMode(dataPin, OUTPUT);
pinMode(buttonPin, INPUT);
/* These are only used if the output pins are NOT on the shift register.
pinMode(c1.getOutputPin(),OUTPUT);
pinMode(c2.getOutputPin(),OUTPUT);
pinMode(c3.getOutputPin(),OUTPUT);
pinMode(c4.getOutputPin(),OUTPUT);
pinMode(c5.getOutputPin(),OUTPUT);
pinMode(c6.getOutputPin(),OUTPUT);
pinMode(c7.getOutputPin(),OUTPUT);
pinMode(c8.getOutputPin(),OUTPUT);
*/
pinMode(c1.getInputPin(),INPUT);
pinMode(c2.getInputPin(),INPUT);
pinMode(c3.getInputPin(),INPUT);
pinMode(c4.getInputPin(),INPUT);
pinMode(c5.getInputPin(),INPUT);
pinMode(c6.getInputPin(),INPUT);
pinMode(c7.getInputPin(),INPUT);
pinMode(c8.getInputPin(),INPUT);
/* Only used if these pins are NOT on the shift register.
pinMode(c1.getRedLedPin(),OUTPUT);
pinMode(c2.getRedLedPin(),OUTPUT);
pinMode(c3.getRedLedPin(),OUTPUT);
pinMode(c4.getRedLedPin(),OUTPUT);
pinMode(c5.getRedLedPin(),OUTPUT);
pinMode(c6.getRedLedPin(),OUTPUT);
pinMode(c7.getRedLedPin(),OUTPUT);
pinMode(c8.getRedLedPin(),OUTPUT);
pinMode(c1.getGreenLedPin(),OUTPUT);
pinMode(c2.getGreenLedPin(),OUTPUT);
pinMode(c3.getGreenLedPin(),OUTPUT);
pinMode(c4.getGreenLedPin(),OUTPUT);
pinMode(c5.getGreenLedPin(),OUTPUT);
pinMode(c6.getGreenLedPin(),OUTPUT);
pinMode(c7.getGreenLedPin(),OUTPUT);
pinMode(c8.getGreenLedPin(),OUTPUT); */
//serial used for debugging.
  Serial.begin(9600);
  Serial.println("Initializing Game");
  for(int i = 0;i < NUM_CITIES;i++){
     Serial.print("City ");
     Serial.print(i+1);
     Serial.print(" Has In/Out pins: ");
     Serial.print(cityArr[i].getInputPin());
     Serial.print("/");
     Serial.println(cityArr[i].getOutputPin());
  }
}
long count = 0;
void loop(){
Serial.println(count);
//Executes the loop if there has been 1 minute of inactivity or button is pressed.
if((count > 60000)||(digitalRead(buttonPin)==LOW)){
//makes all cities working before one is randomly failed.
for(int i=0;i<NUM_CITIES;i++){
 if(cityArr[i].getIsFailed()){ 
  cityArr[i].unfailCity(); 
 }
 //randomly picks connected cities to fail. Will loop infintley until a city is picked.
 bool isComplete = true;
 while(!isComplete){
  int randomNum = random(0,7);
  if(cityArr[randomNum].getLedState()==1){
    cityArr[randomNum].failCity();
    //isComplete = true; //temporarily disabled due to infinite loop when testing.
 }

 }
}
 count = 0; //1 min clock is reset after the new city is failed.
} //end of changing failed city loop

bool flashState = false; //variable to help blinking the LED.
for(int i = 0; i < NUM_CITIES; i++){
  if(digitalRead(cityArr[i].getInputPin()) == HIGH){
    if(cityArr[i].getLedState() == 1){
     count = 0;//clock is reset because user activity is detected.
      if(cityArr[i].getLedState() != 2){
        cityArr[i].setLedState(1);
      }else{
        cityArr[i].setLedState(0);
    }
  }//end of if else logic block

  if(cityArr[i].getLedState() == 0){
    regOne.pinOff(cityArr[i].getOutputPin());
    regOne.pinOff(cityArr[i].getGreenLedPin());
    regOne.pinOff(cityArr[i].getRedLedPin()); 
  }else if(cityArr[i].getLedState() == 1){
    regOne.pinOff(cityArr[i].getOutputPin());
    regOne.pinOff(cityArr[i].getGreenLedPin());
    regOne.pinOn(cityArr[i].getRedLedPin()); 
  }else{ 
    //code below blinks the led without putting delays into the code.
    //this allows the code to continue looping at 1000 Hz instead of 1 Hz.
    regOne.pinOff(cityArr[i].getOutputPin());
    if((count%1000)==0){
      if(flashState){
        Serial.println("Blinking LED off");
        regOne.pinOff(cityArr[i].getRedLedPin());
        flashState = false;
      }else{
        
       regOne.pinOn(cityArr[i].getRedLedPin());
       flashState = true;
    }   
  } 
}//end of if else logic block

if(digitalRead(finishPin==HIGH)){
  //do something here when they win.
  //count = 0;
}

}//end of for loop


}
delay(10); //ensures code doesn't run too fast for the timer. We don't need 1ms resolution however.
count+=20; //adjust the scaling factor here.
}

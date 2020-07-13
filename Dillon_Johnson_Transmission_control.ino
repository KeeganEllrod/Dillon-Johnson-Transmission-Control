/*
 * GEAR logic:
 * 
 * GEAR   SOLENOID #1   SOLENOID #2   SOLENOID#3 (TCL)
 * 1      ON            OFF           OFF
 * 2      ON            ON            OFF
 * 3      OFF           ON            OFF
 * 4      OFF           OFF           DELAY THEN ON
 * -1     ??            ??            ??                BUMP BOX MODE FOR FUTURE USE
 * 
 */


//This defines your constants:
const int Solenoid1Pin = 5;
const int Solenoid2Pin = 6;
const int Solenoid3Pin = 7;
const int UpShiftPin = A0;
const int DownShiftPin = A1;
const int TCLockupDelay = 1000; //when you shift into 4th, there's a 1000 ms delay before the lockup is engaged, tune to suit.

//This defines your variables
int CurrentGearPosition = 1; //boots up in first gear
int buttonState[] = {1, 1};        // the current state of which buttons are pressed (if you want more buttons, you'd add more numbers)
int lastButtonState[] = {1, 1};   // the previous reading from which buttons are pressed (for the debouncer)

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time a button was pressed
unsigned long debounceDelay = 50;    // the debounce time (in ms); adjust as necessary, prevents one button press from accidentally turning into several fast ones, only allows one shift per press.


void setup() 
{
  Serial.begin(9600);  //start serial connection
  pinMode(Solenoid1Pin, OUTPUT);  //Sets the output pin types
  pinMode(Solenoid2Pin, OUTPUT);
  pinMode(Solenoid3Pin, OUTPUT);  
  pinMode(UpShiftPin, INPUT_PULLUP);  //configure A0 as an input and enable the internal pull-up resistor, grounding this pin means "button is pressed"
  pinMode(DownShiftPin, INPUT_PULLUP);  
}

void loop() 
  { 
    int reading[] = {digitalRead(UpShiftPin), digitalRead(DownShiftPin)}; // read the state of the switch into a local variable
    if (reading[0] != lastButtonState[0] || reading[1] != lastButtonState[1])  // If the switch changed, due to noise or pressing
      { 
        lastDebounceTime = millis(); // reset the debouncing timer
      }
    if ((millis() - lastDebounceTime) > debounceDelay) // whatever the reading is at, it's been there for longer than the debounce delay, so take it as the actual current state:
      {
        if (reading[0] != buttonState[0] || reading[1] != buttonState[1])  // if the button state has indeed changed
          {
            buttonState[0] = reading[0];
            buttonState[1] = reading[1];
            if (buttonState[0] == 0 && buttonState[1] == 1) // detect upshift button configuration
              {
                if (CurrentGearPosition <4) //only upshift if you're below 4th gear, aka no 5th gear
                {
                  CurrentGearPosition++;
                }
              } 
              if (buttonState[0] == 1 && buttonState[1] == 0) // detect downshift detect downshift configuration
              {
                if (CurrentGearPosition >1) //only downshift if you're above 1st gear aka dont downshift into 0 gear
                {
                  CurrentGearPosition--;
                }
              }
//              if (buttonState[0] == 0 && buttonState[1] == 0) // detect both upshift and downshift held, maybe future launch/bump box?
//              {
//                if (CurrentGearPosition =1) //only go into bump mode when in 1st gear
//                {
//                  CurrentGearPosition = -1;
//                }
//              }
          }
    }
  lastButtonState[0] = reading[0]; // save the reading. Next time through the loop, it'll be the lastButtonState
  lastButtonState[1] = reading[1]; 
  
  switch (CurrentGearPosition) 
    {
      case 1:
        digitalWrite(Solenoid1Pin, HIGH);
        digitalWrite(Solenoid2Pin, LOW);
        digitalWrite(Solenoid3Pin, LOW);
//        Serial.println("1st gear");
        break;
      case 2:
        digitalWrite(Solenoid1Pin, HIGH);
        digitalWrite(Solenoid2Pin, HIGH);
        digitalWrite(Solenoid3Pin, LOW);
//        Serial.println("2nd gear");
        break;
      case 3:
        digitalWrite(Solenoid1Pin, LOW);
        digitalWrite(Solenoid2Pin, HIGH);
        digitalWrite(Solenoid3Pin, LOW);
//        Serial.println("3rd gear");
        break;
      case 4:
        digitalWrite(Solenoid1Pin, LOW);
        digitalWrite(Solenoid2Pin, LOW);
//        Serial.println("4th gear");
        delay(TCLockupDelay);
        digitalWrite(Solenoid3Pin, HIGH);
//        Serial.println("TC lockup");
        break;
      default:
        // put error behavior here
//        Serial.println("case default");
        break;
    } 
  } //end of loop function

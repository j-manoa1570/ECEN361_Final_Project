//
//
//  Final Project for ECEN 361 v .9 (build 26)
//
//  As of 12 Jul 2017, most code has been written by Jonathan Manoa
//  change() was based off of the fade example sketch. Kace Chandler
//  helped with understand concepts with using serial reads and other
//  neat things. Colby Robbins added Laudio, Raudio, maxAudioValue
//  definitions; input variables section; getPPMsample() function;
//  and fade code in loop() function.
//
//  This code is set to be ran on an Arduino Uno. 
//  Make sure to set the baud rate (basically serial monitor refresh
//  rate) to 19200. If this is not checked, you will receive garbage 
//  values in the serial monitor.
//
//  There are 13 color different modes. See line 34 for list of colors
//  available.
//
//

#define COLORS     3      // Number of colors
#define RED        9      // RGB pins
#define GREEN      10
#define BLUE       11
#define MODEMINUS  13     // Mode selector button pins
#define MODEPLUS   8
#define FADE       .99    // Fade value
#define MFADE      4      // Fade value for color cycle
#define KNOB       A2     // Potentiometer pin
#define Laudio     A0     //audio left input pin
#define Raudio     A1     //audio right input pin
#define MODES      14     // Number of modes
#define maxAudioValue   150.0
int mode = 0;             // Stores mode number
int maxValue = 0;         // Sets maxValue

int modeValues[MODES][3] = { { 0, 0, 0 },   // Blackout
                         { 255, 0, 0 },     // Red
                         { 255, 60, 0 },    // Orange
                         { 255, 255, 0 },   // Yellow
                         { 128, 255, 0 },   // Yellow Green
                         { 0, 255, 0 },     // Green
                         { 0, 255, 255 },   // Turquise
                         { 0, 128, 255 },   // Cyan
                         { 0, 0, 255 },     // Blue
                         { 167, 0, 255 },   // Indigo/UV
                         { 255, 0, 255 },   // Violet
                         { 255, 0, 127 },   // Hot Pink
                         { 255, 255, 255 }, // White
                         { 0, 125, 225} };  // color fade

int brightness[COLORS] = {0, 0, 0};         // LED value to be written
int LED[COLORS] = {RED, GREEN, BLUE};       // LED pins
int reverse[COLORS] = {1, 1, 1};            // Used ni color cycle mode

bool CHANGED = true;

byte input = 0;

//input variables
float dBAudio;
unsigned int dBaverage = 0;
unsigned int dBrawBase = 0;
unsigned int baseCount = 0;
float fade = 0;
unsigned int fadeAdj = 7;
float newZero = 512;
int ch = 0;
int dbPerLed = 3;

/************************************************
 * CHANGE 
 * This will do a color change setting the values
 * to high values to low values and vice versa.
 ***********************************************/
void change() {
    
    // Cycles through all three LEDs and changes their properties
    for (int i = 0; i < COLORS; i++) {
        
        // writes the adjusted value to the output
        analogWrite(LED[i], brightness[i] * (input/maxAudioValue * (maxValue/1024.0)));
        
        // Checks if the fade needs to go the other way
        if ((brightness[i] <= 0 || brightness[i] >= 255)) {
            if (reverse[i] == 1)
                reverse[i] = 0;
            else
                reverse[i] = 1;
        }
        
        // Writes the new brightness value for the color
        if (reverse[i] == 0) {
            brightness[i] = (brightness[i] - MFADE) ;
            if (brightness[i] < 0)
              brightness[i] = 0;
        } else {
            brightness[i] = (brightness[i] + MFADE) ;
            if (brightness[i] > 0)
              brightness[i] == 255;
        }
    }
    
    // Delay is so you can see the fade happen.
    delay(30);
}

/*******************************************************
 * COLOR
 * This is the function that writes values to the LEDs.
 * Values for the modes are determined prior to entering
 * this function.
 ******************************************************/
void color() { 
  
    // Writes the value to the pin
    for (int i = 0; i < COLORS; i++) 
        analogWrite(LED[i], brightness[i]);
        
    // Updates the brightness value using a fade variable
    for (int i = 0; i < COLORS; i++) {
      if (brightness[i] >= 0) {
        brightness[i] = brightness[i] * FADE;
      if (brightness[i] < 0)
        brightness[i] = 0;
      }
    }
      
    // So we can see the magic
    delay(2);
}


/*******************************************************
 * getAudioSample
 * Thisis a function that samples both the left and
 * right audio channels and returns the peak of the two
 * channels added together
 ******************************************************/
void getPPMsample() {
  float maxAudioL = 0;
  float minAudioL = 0;
  float maxAudioR = 0;
  float minAudioR = 0;
  float rawAudio;
  for (int sample = 0; sample < 50  ; sample++)
  {
    rawAudio = analogRead(Laudio); //sample left audio 
    if (rawAudio > maxAudioL)
    {
      maxAudioL = rawAudio;
    }
    if(rawAudio < minAudioL);
    {
      minAudioL = rawAudio;
    }
    rawAudio = analogRead(Raudio); //sample right audio
    if (rawAudio > maxAudioR)
    {
      maxAudioR = rawAudio;
    }
    if(rawAudio < minAudioR);
    {
      minAudioR = rawAudio;
    }
  }
  dBAudio = (maxAudioL - minAudioL)+(maxAudioR - minAudioR);
}

/********************************************************
 * SETUP
 * Setups the board to what I need it to do.
 ******************************************************/
void setup() {
  // initialize serial communication at 19200 bits per second:
  Serial.begin(19200);
  Serial.setTimeout(5);
  
  // pins for both buttons, each LED color pin, and potentiometer
  pinMode(MODEMINUS, INPUT);
  pinMode(MODEPLUS, INPUT); 
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(KNOB, INPUT);

  analogReference(INTERNAL); //set the ADC reference voltage to internal reference
  pinMode(Laudio, INPUT);
  pinMode(Raudio, INPUT);

  // places a value on the button
  digitalWrite(MODEMINUS, HIGH);
  digitalWrite(MODEPLUS, HIGH);
  
}

/**********************************
 * LOOP
 * This is basically main. Runs over and over again and
 * makes everything work.
 *********************************/
void loop() {

  // Used with potentiometer to increase and decrease the max
  // values that can be sent to the LEDs
  maxValue = analogRead(KNOB);

    // Decreases color mode
    if (digitalRead(MODEMINUS) == LOW) {
        mode -= 1;

        // Ensures no integer underflow problems
        if (mode < 0)
          mode = MODES - 1;
        mode = mode % MODES;
        CHANGED = true;
    }

    // This is to debounced the button
    while (digitalRead(MODEMINUS) == LOW) {}

    // Increases the color mode
    if (digitalRead(MODEPLUS) == LOW) {
        mode += 1;
        mode = mode % MODES;
        CHANGED = true;
    }

    // This is to debounced the button
    while (digitalRead(MODEPLUS) == LOW) {}

     getPPMsample();
    if(dBAudio >= fade) //fade the LED value unless input is higher
    {
      fade = dBAudio;
    }
    else if(fade > fadeAdj)
    {
      fade -= fadeAdj;
    }
    else
    {
      fade = fade / 2;
    }
    input = fade;

  // Sets values for all no color changing modes
  if (mode < (MODES - 1)) {
    // Sets brightness array to be the values found in mode and
    // multiplied by the input value times the max value (max brightness).
    brightness[0] = modeValues[mode][0] * (input/maxAudioValue * (maxValue/1024.0));
    brightness[1] = modeValues[mode][1] * (input/maxAudioValue * (maxValue/1024.0));
    brightness[2] = modeValues[mode][2] * (input/maxAudioValue * (maxValue/1024.0));
    color();
  } 
  else {
    
    // Sets values for the color cycling mode
    if (CHANGED) {
    brightness[0] = modeValues[mode][0];
    brightness[1] = modeValues[mode][1];
    brightness[2] = modeValues[mode][2];
    }
    
    CHANGED = false;
    change();
  }
 
}

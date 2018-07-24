// Written by Albert Gunneström
// Code also taken from Notes&Volts
#include <MIDI.h>  
#include <Adafruit_NeoPixel.h>

// Try remove this and see if it works
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED 13    // Arduino Board LED is on Pin 13
#define STRIP_PIN 6

// Initialization of color 
int red = 0;
int green = 0;
int blue = 0;
int fadetime = 2;

// Lighting program change
int mode = 0;
const int NUMBEROFMODES = 2;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(144, STRIP_PIN, NEO_GRB + NEO_KHZ800);


//Create an instance of the library with default name, serial port and settings
MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
   /* MIDI SETUP */
  pinMode (LED, OUTPUT); // Set Arduino board pin 13 to output
  MIDI.begin(MIDI_CHANNEL_OMNI); // Initialize the Midi Library.
  // OMNI sets it to listen to all channels.. MIDI.begin(2) would set it 
  // to respond to notes on channel 2 only.
  MIDI.setHandleNoteOn(MyHandleNoteOn); // This is important!! This command
  // tells the Midi Library which function you want to call when a NOTE ON command
  // is received. In this case it's "MyHandleNoteOn".
  MIDI.setHandleNoteOff(MyHandleNoteOff); // This command tells the Midi Library 
  // to call "MyHandleNoteOff" when a NOTE OFF command is received.


  // ***********NOT YET TESTED *******
  // Adds possibility for MIDI CC messages
  MIDI.setHandleControlChange(MyHandleControlChange);
//*******************


  /* NEOPIXEL SETUP */
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code 
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
}

void loop() { // Main loop 
  MIDI.read(); // Interrupt functions MyHandleNoteOn and MyHandleNoteOff
  fader(0,0,0); // Continually decreases light
}

// MyHandleNoteON is the function that will be called by the Midi Library
// when a MIDI NOTE ON message is received.
// It will be passed bytes for Channel, Pitch, and Velocity
// Switch case for different lighting modes
void MyHandleNoteOn(byte channel, byte pitch, byte velocity) { 
  switch(mode){
    case 0:
      lightAddConst(channel, pitch, velocity);
      break;
    case 1:
      lightAddVelocity(channel, pitch, velocity);
      break;
    case 2: 
      lightStrobe(channel, pitch, velocity);
      break;
   default:
   break;
  }
}

// MyHandleNoteOFF is the function that will be called by the Midi Library
// when a MIDI NOTE OFF message is received.
// * A NOTE ON message with Velocity = 0 will be treated as a NOTE OFF message *
// It will be passed bytes for Channel, Pitch, and Velocity
void MyHandleNoteOff(byte channel, byte pitch, byte velocity) { 
  // Do nothing
}

void MyHandleControlChange(byte channel, byte cc, byte value){
  if(cc == 74){ // The cc for fader on Axiom 25??
    mode = map(value, 0, 127, 0, NUMBEROFMODES);
  }
  if(cc == 7){ // The cc for fader on Axiom 25??
    fadetime = map(value, 0, 127, 20, 0);
  }
}

// Decreases light by the amount fadetime
void fader(int r, int g, int b){
  // Makes sure max brightness within range
  red += r; // adds to globla variable
  green += g;
  blue += b;
  if(red > 255) red = 255;
  if(red < 0) red = 0;
  if(green > 255) green = 255;
  if(green < 0) green = 0;
  if(blue > 255) blue = 255;
  if(blue < 0) blue = 0;

// Set all pixels to the same color
  uint32_t c = strip.Color(red, green, blue);
  for(int i = 0; i < strip.numPixels(); i++){
  strip.setPixelColor(i, c);
  }
  strip.show();

  // Decrease light by value fadetime
  red -= fadetime; 
  green -= fadetime;
  blue -= fadetime;
}

void lightAddConst(byte channel, byte pitch, byte velocity){  
      if(pitch%12 == 0){ // For all notes C
        fader(200,0,0);
      }
      if(pitch%12 == 2){ // For all notes D 
        fader(0,200,0);
      }
      if(pitch%12 == 4){ // For all notes E
        fader(0,0,200);
      }
      if(pitch%12 == 6){ // For all notes F# 
        fader(200,0,160);
      }
      if(pitch%12 == 8){ // For all notes G#
        fader(0,180,200);
      }
      if(pitch%12 == 10){ // For all notes A# 
        fader(200,200,0);
      }
      // Following to change the fadetime
      if(pitch%12 == 5){ // For all notes F 
        fadetime = 1;
      }
      if(pitch%12 == 7){ // For all notes G 
        fadetime = 5;
      }
      if(pitch%12 == 9){ // For all notes A 
        fadetime = 20;
      }
}

void lightAddVelocity(byte channel, byte pitch, byte velocity){
      if(pitch%12 == 0){ // For all notes C 
        fader(velocity * 2,0,0);
      }
      if(pitch%12 == 2){ // For all notes D 
        fader(0,velocity * 2,0);
      }
      if(pitch%12 == 4){ // For all notes E
        fader(0,0,velocity * 2);
      }
      if(pitch%12 == 6){ // For all notes F# 
        fader(velocity * 2,0,0);
      }
      if(pitch%12 == 8){
        fader(0,velocity * 2,0); // For all notes G# 
      }
      if(pitch%12 == 10){
        fader(0,0,velocity * 2); // For all notes A#
      }
      
      // Following to change the fadetime
      if(pitch%12 == 5){ // For all notes F 
        fadetime = 1;
      }
      if(pitch%12 == 7){ // For all notes G 
        fadetime = 5;
      }
      if(pitch%12 == 9){ // For all notes A 
        fadetime = 20;
      }
}


// Will only work if midi read is an interupt
void lightStrobe(byte channel, byte pitch, byte velocity){
  fadetime = 200;
  while(true){
    if(pitch ==48) break;
    fader(200,200,200);
    delay(5);
  }
}

// For testing purposes
void sameColorFade(uint32_t r, uint32_t g, int32_t b, uint8_t wait) {
  uint32_t c = strip.Color(r,g,b);
  for(uint16_t i = 0; i < strip.numPixels(); i++){
    strip.setPixelColor(i, c);  
  }
  strip.show();
  while (r > 1){
    r--;
    c = strip.Color(r--,g,b);
  for(uint16_t i = 0; i < strip.numPixels(); i++){
    strip.setPixelColor(i, c);  
  }
    strip.show();
  }  
  for(uint16_t i = 0; i < strip.numPixels(); i++){
    strip.setPixelColor(i, 0);  
  }
  strip.show();
}


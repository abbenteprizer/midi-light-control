#include <MIDI.h>  
#include <Adafruit_NeoPixel.h>

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

// Button and program change
const int btn1 = 7;
int mode = 0;

/*
// Function prototypes
void MyHandleNoteOn(byte channel, byte pitch, byte velocity);
void MyHandleNoteOff(byte channel, byte pitch, byte velocity);
void fader(int r, int g, int b);
*/

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
  /* Mode setup */
  pinMode(btn1, INPUT);
  
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
  MIDI.read();
  fader(0,0,0);
  if(digitalRead(btn1)){
    mode++;
    mode = mode%2;
    delay(20);  
  }
}

// MyHandleNoteON is the function that will be called by the Midi Library
// when a MIDI NOTE ON message is received.
// It will be passed bytes for Channel, Pitch, and Velocity
void MyHandleNoteOn(byte channel, byte pitch, byte velocity) { 
  //digitalWrite(LED,LOW);  //Turn LED on
  /*for(int i = velocity; i >= 0; i--){ // For testing purposes
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
    }
  */
  switch(mode){
    case 0:
      if(pitch == 48){
        fader(200,0,0);
        //*g = 200;
        //sameColorFade(255, 0, 0, 1000); // arg1 color, arg2 fadetime
      }
      if(pitch == 50){
        fader(0,200,0);
      }
      if(pitch == 52){
        fader(0,0,200);
      }
      break;
    case 1:
      if(pitch == 48){
        fader(velocity * 2,0,0);
        //*g = 200;
        //sameColorFade(255, 0, 0, 1000); // arg1 color, arg2 fadetime
      }
      if(pitch == 50){
        fader(0,velocity * 2,0);
      }
      if(pitch == 52){
        fader(0,0,velocity * 2);
      }
      if(pitch == 53){
        fadetime = 1;
      }
      if(pitch == 55){
        fadetime = 40;
      }
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
  //digitalWrite(LED,LOW);  //Turn LED off
}

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

  uint32_t c = strip.Color(red, green, blue);
  for(int i = 0; i < strip.numPixels(); i++){
  strip.setPixelColor(i, c);
  }
  strip.show();

  //Fade time
  red -= fadetime; 
  green -= fadetime;
  blue -= fadetime;
}

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


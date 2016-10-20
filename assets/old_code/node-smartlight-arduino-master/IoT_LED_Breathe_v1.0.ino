/*NYCDA & NextFab IoT Workshop
 * LED Breathe
 * by Matt Garfield
 * 8/28/2016
 * Version: MKR1000_NYCDA-LED_Breathe_v1.0_MG
 * 
 * Description:
 *    - Slowly fade the onboard LED and the RGB LED strip in and out
 *    - Ensures proper upload of code and wiring of circuit
*/ 

/* Arduino Output Pins
********************/
#define LED_PIN     6
#define RED_PIN     5
#define GREEN_PIN   4
#define BLUE_PIN    3

//Use color names as index values for color[]
#define RED_INDEX     1
#define GREEN_INDEX   2
#define BLUE_INDEX    3

//Hue Value Color Map from Adafruit
//Used for rainbow()
//https://blog.adafruit.com/2012/03/14/constant-brightness-hsb-to-rgb-algorithm/
#define RED_HUE     0
#define YELLOW_HUE  128
#define GREEN_HUE   256
#define TEAL_HUE    384
#define BLUE_HUE    512
#define PURPLE_HUE  640

/* Setup Routine
********************/
void setup() {
  
      /* Initialize Pin Modes
      ********************/
          pinMode(LED_PIN, OUTPUT);
          pinMode(RED_PIN, OUTPUT);
          pinMode(BLUE_PIN, OUTPUT);
          pinMode(GREEN_PIN, OUTPUT);
     
} //END Setup

/* Loop
********************/
void loop() {

  //breatheHSB(int hue, byte saturation, byte brightness, int wait (ms))
  breatheHSB (0, 0, 255, 3);
  
} //END Loop

/* SubRoutines
**********************/

/***************************************************************************
breatheHSB(int hue, byte saturation, byte brightness, int wait)
  * Runs a single fade-on / fade-out sequence
  * Requires hsbrgb()
  * 
  * hue = color to fade into; 0: full red, 256: full green, 512: full blue, 767: full red
  * saturation = intensity of hue component; 0 = full white, 255 = full hue
  * brightness = intensity of output light; 0 = off, 255 = full brightness
  * wait = number of milliseconds to wait between loops
*****************************************************************************/
//Initialize array to hold RED, GREEN, BLUE color values from hsb2rgb() routine
byte color[3];

void breatheHSB (int h, byte s, byte b, int wait){
      
      for(int i = 0; i <= b; i++){      
          hsb2rgb(h, s, i, color);
      
          //Set PWM values to Color Pins
          analogWrite(RED_PIN, color[RED_INDEX]);
          analogWrite(GREEN_PIN, color[GREEN_INDEX]);
          analogWrite(BLUE_PIN, color[BLUE_INDEX]);
          analogWrite(LED_PIN, i);
    
          delay(wait);    //sets speed of fade
      }

      for(int i = b; i >= 0; i--){      
          hsb2rgb(h, s, i, color);
      
          //Set PWM values to Color Pins
          analogWrite(RED_PIN, color[RED_INDEX]);
          analogWrite(GREEN_PIN, color[GREEN_INDEX]);
          analogWrite(BLUE_PIN, color[BLUE_INDEX]);
          analogWrite(LED_PIN, i);
    
          delay(wait);    //sets speed of fade
      }
  
} //END breatheHSB()

/******************************************************************************
 hsb2rgb()
 Convert Hue, Saturation, Brightness to Red, Green, Blue values
 
 https://blog.adafruit.com/2012/03/14/constant-brightness-hsb-to-rgb-algorithm/
 
 * accepts hue, saturation and brightness values and outputs three 8-bit color
 * values in an array (color[])
 *
 * saturation (sat) and brightness (bright) are 8-bit values.
 *
 * hue (index) is a value between 0 and 767. hue values out of range are
 * rendered as 0.
 *
 *****************************************************************************/
void hsb2rgb(uint16_t index, uint8_t sat, uint8_t bright, uint8_t color[3]){

      uint16_t r_temp, g_temp, b_temp;
      uint8_t index_mod;
      uint8_t inverse_sat = (sat ^ 255);
    
      index = index % 768;
      index_mod = index % 256;
    
      if (index < 256)
      {
        r_temp = index_mod ^ 255;
        g_temp = index_mod;
        b_temp = 0;
      }
    
      else if (index < 512)
      {
        r_temp = 0;
        g_temp = index_mod ^ 255;
        b_temp = index_mod;
      }
    
      else if ( index < 768)
      {
        r_temp = index_mod;
        g_temp = 0;
        b_temp = index_mod ^ 255;
      }
    
      else
      {
        r_temp = 0;
        g_temp = 0;
        b_temp = 0;
      }
    
      r_temp = ((r_temp * sat) / 255) + inverse_sat;
      g_temp = ((g_temp * sat) / 255) + inverse_sat;
      b_temp = ((b_temp * sat) / 255) + inverse_sat;
    
      r_temp = (r_temp * bright) / 255;
      g_temp = (g_temp * bright) / 255;
      b_temp = (b_temp * bright) / 255;
    
      color[RED_INDEX]  = (uint8_t)r_temp;
      color[GREEN_INDEX]  = (uint8_t)g_temp;
      color[BLUE_INDEX] = (uint8_t)b_temp;

} //END hsb2rgb()


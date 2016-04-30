#include <LiquidCrystal.h>
#include <Timer.h>

int exposurePin = 50;
int focusPin = 48;

int thirtySeconds = 30000;
int oneMinute = 60000;
unsigned long exposureTimeMS = 2000;// oneMinute + thirtySeconds;
unsigned long timeBetweenPhotoMS = 10000; // 3 * oneMinute;
int photo_count = 0;
int going = 0;
int repeat_count = 10;
int daytime_photo = 0;
Timer t1;
Timer t2;


// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int current_menu_state = 0;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {
  // put your setup code here, to run once:
     pinMode(exposurePin,INPUT);
     pinMode(focusPin,INPUT);

     lcd.begin(16,2);
}

void loop() {
  // put your main code here, to run repeatedly:
  t1.update();
  t2.update();

  int current_state = read_LCD_buttons(); 
  update_state(current_state);
}
void update_state(int state){
if (state!=5){
  lcd.clear();
}
  switch (state)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
     {
     increment_menu_state();
     display_current_menu_value();
     break;
     }
   case btnLEFT:
     {
     decrement_menu_state();
     display_current_menu_value();
     break;
     }
   case btnUP:
     {
     increment_current_selected_value();
     display_current_menu_value();
     break;
     }
   case btnDOWN:
     {
     decrement_current_selected_value();
     display_current_menu_value();
     break;
     }
   case btnSELECT:
     {
      if (going == 0){
        t1.every(exposureTimeMS + timeBetweenPhotoMS, stopTakingPhoto, repeat_count);
        photo_count = 0;
        going = 1;
      }

     break;
     }
     case btnNONE:
     {

     break;
     }
 }

}
void decrement_current_selected_value(){
    if (current_menu_state == 0){
        if (daytime_photo == 1){
        exposureTimeMS = exposureTimeMS - 10;
      }else{
         exposureTimeMS = exposureTimeMS - 1000;
      }
  }
  else if (current_menu_state == 1){
    timeBetweenPhotoMS = timeBetweenPhotoMS - 5000;
  }
  else if (current_menu_state == 2){
    repeat_count = repeat_count - 5;
  }
    else if (current_menu_state == 3){
    daytime_photo = ( daytime_photo + 1 ) % 2;
  }
}
void increment_current_selected_value(){
    if (current_menu_state == 0){
      if (daytime_photo == 1){
        exposureTimeMS = exposureTimeMS + 10;
      }else{
         exposureTimeMS = exposureTimeMS + 1000;
      }
  }
  else if (current_menu_state == 1){
    timeBetweenPhotoMS = timeBetweenPhotoMS + 5000;
  }
  else if (current_menu_state == 2){
    repeat_count = repeat_count + 5;
  }
  else if (current_menu_state == 3){
    daytime_photo = ( daytime_photo + 1 ) % 2;
  }
}

void display_current_menu_value(){
  char menu_display_string[40] = "";
  if (current_menu_state == 0){
    if (exposureTimeMS > 2000){
      snprintf(menu_display_string,40,"shutter %lu s", exposureTimeMS /1000 );
    }
    else
    {
      snprintf(menu_display_string,40,"shutter %lu ms", exposureTimeMS );
    }
  }
  else if (current_menu_state == 1){
    snprintf(menu_display_string,40,"gap %lu s", timeBetweenPhotoMS /1000 );
  }
  else if (current_menu_state == 2){
    snprintf(menu_display_string,40,"num %d", repeat_count );
  }
  else if (current_menu_state == 3){
    snprintf(menu_display_string,40,"daytime = %d", daytime_photo );
  }
  lcd.print(menu_display_string);
}
// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);
// read the value from the sensor 
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result

 delay(50);
 adc_key_in = analogRead(0);//debounce
 delay(150);//no double press
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;  
 return btnNONE;  // when all others fail, return this...
}

void increment_menu_state(){
  current_menu_state = (current_menu_state + 1) % 4;
}

void decrement_menu_state(){
  if (current_menu_state == 0){
    current_menu_state = 3;
  }else{
    current_menu_state = current_menu_state - 1;
  }
  
}

void takePhoto(){

    if (photo_count++ > repeat_count){
      going = 0;
      lcd.clear();
      lcd.print("done");
      pinMode(exposurePin,INPUT);
    }
    if (going == 1 ){
      digitalWrite(exposurePin,LOW);
      pinMode(exposurePin,OUTPUT);
      lcd.clear();
      lcd.print("taking photo");
    }

}
void stopTakingPhoto(){
    pinMode(exposurePin,INPUT);
    lcd.clear();
    int remaining = repeat_count - photo_count;
    char remaining_string[16];
    snprintf(remaining_string, 16, "%d remain", remaining);
    
    lcd.println("idle" );
    lcd.println(remaining_string);
    
    t2.after(timeBetweenPhotoMS, takePhoto);
}




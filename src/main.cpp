#include <Arduino.h>
#include <dht.h>
#include <Servo.h>

// Reset led and button states
int led1_state = 0;
int led2_state = 0;
int led3_state = 0;
int led4_state = 0;
int led5_state = 0;
int led6_state = 0;

int button1_state = 0;
int button2_state = 0;
int button3_state = 0;
int button4_state = 0;

// Declare pin numbers
int servo_pin = 10;

int led6_pin = 21;
int led5_pin = 22;
int led4_pin = 23;
int led3_pin = 24;
int led2_pin = 25;
int led1_pin = 26;

int button1_pin = 27;
int button2_pin = 0;
int button3_pin = 1;
int button4_pin = 5;
int dht_pin = 45;

// Declare trigger variables
float hum = 0;
float temp = 0;
float on_hum = 0;
float on_temp = 0;
float off_hum = 0;
float off_temp = 0;
unsigned long on_interval = 0;
unsigned long off_interval = 0;
int mode = 0;
int ac_state = 0;

// Acceptable trigger variable range
float hum_interval = 2;
float temp_interval = 0.5;

// For blinking LED
unsigned long previousMillis = 0;
int blink_count = 0;

// Declare Servo and Humidity sensor objects
Servo servo;
DHT dht(dht_pin, DHT11);

void setup() {
  Serial.begin(9600);
  dht.begin();

  // initialize LED digital pin as an output
  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);
  pinMode(led3_pin, OUTPUT);
  pinMode(led4_pin, OUTPUT);
  pinMode(led5_pin, OUTPUT);
  pinMode(led6_pin, OUTPUT);

  // initialize button digital pin as input
  pinMode(button1_pin, INPUT);
  pinMode(button2_pin, INPUT);
  pinMode(button3_pin, INPUT);
  pinMode(button4_pin, INPUT);

  // activate key pin pullup resistors
  digitalWrite(button1_pin, HIGH);
  digitalWrite(button2_pin, HIGH);
  digitalWrite(button3_pin, HIGH);
  digitalWrite(button4_pin, HIGH);

  // initialize servo pin
  servo.attach(servo_pin);
}

void print_line(String str_print_text)
{
  Serial.println(str_print_text);
}

void trigger_button()
{
  servo.write(60);
  delay(230);
  servo.write(90);
  delay(500);
  servo.write(110);
  delay(230);
  servo.write(90);
  delay(500);

  ac_state = !ac_state;
}

void led_toggle(int led1_flag, int led2_flag, int led3_flag, int led4_flag, int led5_flag, int led6_flag)
{
  digitalWrite(led1_pin, led1_flag);
  digitalWrite(led2_pin, led2_flag);
  digitalWrite(led3_pin, led3_flag);
  digitalWrite(led4_pin, led4_flag);
  digitalWrite(led5_pin, led5_flag);
  digitalWrite(led6_pin, led6_flag);
}

void turn_on_led_confirm()
{
  led_toggle(0,0,0,0,0,0);
  led_toggle(0,0,1,1,0,0);
  delay(500);
  led_toggle(0,1,1,1,1,0);
  delay(500);
  led_toggle(1,1,1,1,1,1);
}

void turn_off_led_confirm()
{
  led_toggle(1,1,1,1,1,1);
  delay(500);
  led_toggle(0,1,1,1,1,0);
  delay(500);
  led_toggle(0,0,1,1,0,0);
  delay(500);
  led_toggle(0,0,0,0,0,0);
}

void settings_confirm()
{
  led_toggle(1,1,1,1,1,1);
  delay(500);
  led_toggle(0,0,0,0,0,0);
}

void temp_sense()
{
  int number_of_samples = 6;
  int led_array[6] = {0,0,0,0,0,0};

  hum = 0;
  temp = 0;
  
  for (int i = 0; i<number_of_samples; i++)
  {
    hum = hum + dht.readHumidity();
    temp = temp + dht.readTemperature();
    led_array[i] = 1;
    
    delay(1000); //Delay 1 sec.
    led_toggle(led_array[5], led_array[4], led_array[3], led_array[2], led_array[1], led_array[0]);
  }
  
  // Find average of 6 samples
  hum = hum/number_of_samples;
  temp = temp/number_of_samples;
}

void reset_led_state()
{
  led1_state = 0;
  led2_state = 0;
  led3_state = 0;
  led4_state = 0;
  led5_state = 0;
  led6_state = 0;
}

void blink_led(unsigned long currentMillis, unsigned long interval, int led1_toggle, int led2_toggle, int led3_toggle, int led4_toggle, int led5_toggle, int led6_toggle )
{ 
    if (currentMillis - previousMillis >= interval) 
    {
      blink_count++;
      // save the last time you blinked the LED
      previousMillis = currentMillis;

      if (led1_toggle)
      {
        led1_state = !led1_state;
      }
      if (led2_toggle)
      {
        led2_state = !led2_state;
      }
      if (led3_toggle)
      {
        led3_state = !led3_state;
      }
      if (led4_toggle)
      {
        led4_state = !led4_state;
      }
      if (led5_toggle)
      {
        led5_state = !led5_state;
      }
      if (led6_toggle)
      {
        led6_state = !led6_state;
      }

      // Send control signal to led
      led_toggle(led1_state, led2_state, led3_state, led4_state, led5_state, led6_state);
  }
}

int obtain_mode()
{
  delay(500);
  int flag = 1;
  led1_state = 0;
  led2_state = 0;
  led3_state = 0;
  led4_state = 0;
  led5_state = 0;
  led6_state = 0;

  while (flag)
  {
    // Note: Remember to reset blink count
    blink_led(millis(), 500, 1, 1, 1, 1, 1, 1);

    if (!digitalRead(button1_pin))
    {
      reset_led_state();
      on_hum = 0;
      on_temp = 0;
      off_hum = 0;
      off_temp = 0;
      on_interval = 0;
      off_interval = 0;

      blink_count = 0;
      return 1;
    }
    if (!digitalRead(button2_pin))
    {
      reset_led_state();
      on_hum = 0;
      on_temp = 0;
      off_hum = 0;
      off_temp = 0;
      on_interval = 0;
      off_interval = 0;

      blink_count = 0;
      return 2;
    }
    if (!digitalRead(button3_pin))
    {
      reset_led_state();
      on_hum = 0;
      on_temp = 0;
      off_hum = 0;
      off_temp = 0;
      on_interval = 0;
      off_interval = 0;

      blink_count = 0;
      return 3;
    }
  }
  return 0;
}

void loop()
{
  // If mode is not selected
  if (mode == 0)
  {
    // Obtain mode
    mode = obtain_mode(); 
  }

  // Mode 1: Set on temp and Set Off temp
  else if (mode == 1)
  {
    // LED indicator for mode

    while (blink_count <= 5)
      blink_led(millis(), 500, 1, 0, 0, 0, 0, 0);
    reset_led_state();

    // Set Trigger Parameters for Turning on
    while (on_temp == 0 || on_hum == 0)
    {
      blink_led(millis(), 500, 1, 1, 0, 0, 0, 0);
      
      if (!digitalRead(button1_pin) && !digitalRead(button2_pin))
      {
        led_toggle(1,1,0,0,0,0);
        temp_sense();
        on_hum = hum;
        on_temp = temp;
        Serial.println("On Humidity: " + String(on_hum) + "\nOn Temp: " + String(on_temp) + "\n\n");
        trigger_button();
        reset_led_state();
      }
    }

    // Set Trigger Parameters for Turning off
    while (off_temp == 0 || off_hum == 0)
    {
      // Make led 1 and 2 stay on while blinking led 5 and led 6
      led1_state = 1;
      led2_state = 1;
      blink_led(millis(), 500, 0, 0, 0, 0, 1, 1);

      if (!digitalRead(button3_pin) && !digitalRead(button4_pin))
      {
        led_toggle(1,1,0,0,1,1);
        temp_sense();
        off_hum = hum;
        off_temp = temp;
        Serial.println("On Humidity: " + String(off_hum) + "\nOn Temp: " + String(off_temp) + "\n\n");
        trigger_button();
        reset_led_state();
      }

    }

    while(mode == 1)
    {
      // Trigger to exit mode to change mode
      if (!digitalRead(button1_pin) && !digitalRead(button4_pin))
      {
        blink_count=0;
        while (blink_count <= 20)
          blink_led(millis(), 100, 1, 1, 1, 1, 1, 1);
        mode = 0;
        break;
      }

      temp_sense();

      Serial.println("AC Status: " + String(ac_state) + "\nOn Humidity:      " + String(on_hum) + " On Temp:      " + String(on_temp) + "\nOff Humidity:     " + String(off_hum) + " Off Temp:     " + String(off_temp) + "\nCurrent Humidity: "+ String(hum) + " Current Temp: "+ String(temp) + "\n\n");
      
      if (ac_state == 1)
      {
        if ((abs(hum-off_hum) < hum_interval) || (abs(temp-off_temp) < temp_interval) || temp < off_temp || hum < off_hum)
        {
          trigger_button();
        }
      }
      else if (ac_state == 0)
      {
        if ((abs(hum-on_hum) < hum_interval) || (abs(temp-on_temp) < temp_interval) || temp > on_temp || hum > on_hum)
        {
          trigger_button();
        }
      }
    }
  }

  else if (mode == 2)
  {
    // LED indicator for mode  Note: Reset blink count
    while (blink_count <= 5)
      blink_led(millis(), 500, 0, 1, 0, 0, 0, 0);
    blink_count = 0;
    // Trigger to exit mode to change mode
    if (!digitalRead(button1_pin) && !digitalRead(button4_pin))
    {
      mode = 0;
    }
  }

  else if (mode == 3)
  {
    // LED indicator for mode  Note: Reset blink count
    while (blink_count <= 5)
      blink_led(millis(), 500, 0, 0, 1, 0, 0, 0);
    blink_count = 0;

    // Trigger to exit mode to change mode
    if (!digitalRead(button1_pin) && !digitalRead(button4_pin))
    {
      mode = 0;
    }
  }
}
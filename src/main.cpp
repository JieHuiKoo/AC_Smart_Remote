#include <Arduino.h>
#include <dht.h>
#include <Servo.h>

unsigned long previousMillis = 0;
int led1_state = 0;
int led2_state = 0;
int led3_state = 0;
int led4_state = 0;
int led5_state = 0;
int led6_state = 0;

Servo servo;
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
DHT dht(dht_pin, DHT11);
int hum = 0;
int temp = 0;

int on_hum = 0;
int on_temp = 0;
int off_hum = 0;
int off_temp = 0;
int on_interval = 0;
int off_interval = 0;

int button1_state = 0;
int button2_state = 0;
int button3_state = 0;
int button4_state = 0;

int mode = 0;

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
  //Read data and store it to variables hum and temp
    hum = dht.readHumidity();
    temp= dht.readTemperature();
    //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
    delay(2000); //Delay 2 sec.
    led_toggle(0,0,0,0,0,0);
    delay(100);
    led_toggle(1,1,1,1,1,1);
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

unsigned long blink_led(unsigned long currentMillis, unsigned long interval, int led1_toggle, int led2_toggle, int led3_toggle, int led4_toggle, int led5_toggle, int led6_toggle )
{    
    if (currentMillis - previousMillis >= interval) 
    {
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
    blink_led(millis(), 1000, 1, 1, 1, 1, 1, 1);

    if (!digitalRead(button1_pin))
    {
      reset_led_state();
      on_hum = 0;
      on_temp = 0;
      off_hum = 0;
      off_temp = 0;
      on_interval = 0;
      off_interval = 0;
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
      return 3;
    }
  }
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
    blink_led(millis(), 500, 1,0,0,0,0,0);

    // Trigger to exit mode to change mode
    if (!digitalRead(button1_pin) && !digitalRead(button4_pin))
    {
      mode = obtain_mode();
    }
  }

  else if (mode == 2)
  {
    // LED indicator for mode
    blink_led(millis(), 500, 0,1,0,0,0,0);

    // Trigger to exit mode to change mode
    if (!digitalRead(button1_pin) && !digitalRead(button4_pin))
    {
      mode = obtain_mode();
    }
  }

  else if (mode == 3)
  {
    // LED indicator for mode
    blink_led(millis(), 500, 0,0,1,0,0,0);

    // Trigger to exit mode to change mode
    if (!digitalRead(button1_pin) && !digitalRead(button4_pin))
    {
      mode = obtain_mode();
    }
  }
}





// //
// delay(100);
//   // Initialize leds to off
//   led_toggle(0,0,0,0,0,0);
  
//   if (!digitalRead(button1_pin))
//   {
//     digitalWrite(led1_pin, HIGH);
//     // print_line("1 press");
//   }
//   else if (!digitalRead(button2_pin))
//   {
//     digitalWrite(led2_pin, HIGH);
//     // print_line("2 press");
//   }
//   else if (!digitalRead(button3_pin))
//   {
//     digitalWrite(led3_pin, HIGH);
//     // print_line("3 press");
//   }
//   else if (!digitalRead(button4_pin))
//   {
//     digitalWrite(led4_pin, HIGH);
//     // print_line("4 press");
//   }
//   if (!digitalRead(button1_pin) && !digitalRead(button4_pin))
//   {
//     settings_confirm();
//     temp_sense();
//     led_toggle(1,1,1,1,1,1);
    
//     // print_line("1 and 4 press");
//   }
//   if (!digitalRead(button1_pin) && !digitalRead(button2_pin))
//   {
//     turn_on_led_confirm();
//     temp_sense();
//     trigger_button();
//     // print_line("1 and 2 press");
//   }
//   if (!digitalRead(button3_pin) && !digitalRead(button4_pin))
//   {
//     turn_off_led_confirm();
//     temp_sense();
//     trigger_button();
//     // print_line("3 and 4 press");
//   }
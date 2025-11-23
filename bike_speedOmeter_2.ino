#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

const int sensor_pin = 34;
const int button_pin = 15;// ปุ่ม reset
const int thres_hold = 1200; 

const float distance_meter = 1.40; // ระยะทางที่ล้อหมุนไปได้ 1 รอบ ในหน่วยเมตร

float distance_total = 0;
float speed_kmh = 0;
float max_speed = 0;

unsigned long t_first = 0;
unsigned long last_move_time = 0;

unsigned long total_run_time = 0;   
unsigned long last_update_time = 0;

bool first_detected = false;
int last_state = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(sensor_pin, INPUT);
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(2,OUTPUT);
  pinMode(4,OUTPUT);
  digitalWrite(2,LOW);
  digitalWrite(4,HIGH);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Speedometer Bike");
  lcd.setCursor(0,1);
  lcd.print("is Ready!!");
  delay(4000);
  lcd.clear();
}

void loop() {
  unsigned long now_ms = millis();
  int sensor_val = analogRead(sensor_pin);
  int state = (sensor_val > thres_hold) ? 1 : 0;
  
  if (digitalRead(button_pin) == HIGH) {
    resetAll();
    delay(300);
  }

  if (last_state == 0 && state == 1) {
    if (!first_detected) {
      t_first = micros();
      first_detected = true;
    }
    else {
      unsigned long t_last = micros();
      unsigned long diff = t_last - t_first;

      float speed_m_s = distance_meter * 1e6 / diff;
      speed_kmh = speed_m_s * 3.6;

      distance_total += distance_meter;
      last_move_time = now_ms;

      if (speed_kmh > max_speed) max_speed = speed_kmh;
      showDashboard();
      first_detected = false;
    }
    delay(8);
  }
  last_state = state;

  if (now_ms - last_move_time < 1500) {
 
    if (now_ms - last_update_time >= 1000) {
      total_run_time++;
      last_update_time = now_ms;
    }
  } 
  else {
    // ถ้ากรณีล้อหมุด ให้แสดงความเร็ว เป็น 0.0
    speed_kmh = 0;
    showDashboard();
  }

}
// ฟังก์ชั่น reset ทุกค่าให้เป็น 0
void resetAll() {
  distance_total = 0;
  max_speed = 0;
  speed_kmh = 0;
  total_run_time = 0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("RESET DONE");
  delay(700);
  lcd.clear();
}
// จอแสดงผล
void showDashboard() {

  int minutes = total_run_time / 60;
  int seconds = total_run_time % 60;

  lcd.setCursor(0,0);
  lcd.print(String(speed_kmh,1) + "kph " + String(max_speed,1)+"kph ");
  lcd.setCursor(0,1);
  lcd.print(String(distance_total/1000,2) + "km ");
  lcd.print((minutes < 10 ? "0":"") + String(minutes) + ":");
  lcd.print((seconds < 10 ? "0":"") + String(seconds));
}

// Индикатор концентрации CO2, сотоит из ардуинки nano, датчика и экранчика.`
// Умеет показывать текущее значение PPM, графики PPM за 6, 60 и 600 минут.
// При привышении 1200 PPM (отображается точками) включается 13й пин и встроееный led, 
// ниже 800 PPM выключается.
// Данные обновляются раз в 6 секунд.
// Подробности на других страницах скрипта.


// Датчик T6703
#include <Wire.h>
uint8_t rc;
int co2ppm = 0;
#define T67XX_I2C_ADDRESS                                       (0x15)
#define T67XX_I2C_REQUEST_SIZE                                  (0x05)
#define T67XX_I2C_RESPONSE_SIZE                                 (0x04)
#define T67XX_I2C_RESPONSE_DELAY                                (0x0A)
#define T67XX_I2C_FIELD_FUNCTION_CODE                           (0x00)
#define T67XX_I2C_FIELD_BYTES_COUNT                             (0x01)
#define T67XX_I2C_FIELD_DATA_MSB                                (0x02)
#define T67XX_I2C_FIELD_DATA_LSB                                (0x03)

// Oled экран 1306
#include "ssd1306.h"

bool timer1 = false, timer2 = false, timer3 = false;
unsigned long time1, time2, time3;

// Таймеры индикации 1го 2го и 3го блока данных
unsigned long dt1 = (unsigned long) (6. / 42 * 1000 * 60); // 6 минут на 43 (43-1, если с зазором) точки, в миллисекундах !не менее 3000 (ограничение датчика сo2)
unsigned long dt2 = (unsigned long) (60. / 42 * 1000 * 60); // 1 час
unsigned long dt3 = (unsigned long) (600. / 41 * 1000 * 60); // 10 часов

// unsigned long dt1 = (unsigned long) (4. / 43 * 1000 * 60); //// debug
// unsigned long dt2 = (unsigned long) (9. / 43 * 1000 * 60); ////
// unsigned long dt3 = (unsigned long) (13. / 42 * 1000 * 60); ////
int c1[43];
int c2[43];
int c3[42];


void setup() {
  Serial.begin(9600);
  Wire.begin(); // T6703
  setup_1306(); // 1306
  hardClear();

  printStringSmallFont(30, 24, "Привет, хуман...");
  delay(500);

  unsigned long current_time_0 = millis();
  time1 = current_time_0;
  time2 = current_time_0;
  time3 = current_time_0;

  for (int i = 0; i < 43; i++) {
    c1[i] = 0;
  }
  for (int i = 0; i < 43; i++) {
    c2[i] = 0;
  }
  for (int i = 0; i < 42; i++) {
    c3[i] = 0;
  }

  pinMode(13, OUTPUT); // LED_BUILTIN
  digitalWrite(13, LOW);

  Serial.println("https://github.com/Leo5700/co2Monitor");
}

void loop() {
  // Проверяем сработали ли таймеры
  timer1 = false;
  timer2 = false;
  timer3 = false;
  unsigned long current_time = millis();
  if (current_time - time1 >= dt1) {
    time1 = current_time;
    timer1 = true;
  }
  if (current_time - time2 >= dt2) {
    time2 = current_time;
    timer2 = true;
  }
  if (current_time - time3 >= dt3) {
    time3 = current_time;
    timer3 = true;
  }

  // По самому шустрому таймеру перерисовываем экран
  if (timer1) {
    hardClear();

    int cmin = 0; // Диапазон показаний датчика, которые будем вписывать в экран
    int cmax = 2000;

    for (int i = 0; i < 128; i += 3) { // разметка
      int x = i;
      int y = int(map(1200, cmin, cmax, 59, 0)); // 1200 PPM порог проветривания
      ssd1306_putPixel(x, y);
    }

    for (int j = 0; j < 10; j += 1) {
      ssd1306_putPixel(43, j); // второй блок
      ssd1306_putPixel(86, j); // третий блок
    }

    rc = getT67XXMetric(T67XX_I2C_ADDRESS, &co2ppm);
    if (rc) {
      for (int i = 42; i > 0; i--)
        c1[i] = c1[i - 1];
      c1[0] = co2ppm;
      Serial.println(co2ppm);
    } else {
      printStringSmallFont(48, 12, "Sensor failure");
      Serial.println("Sensor failure");
    }


    // Отрисовка графиков

    for (int i = 0; i < 43 - 1; i++) { // -1 пиксель на зазор м\графиками
      int x = i;
      int y = int(map(c1[i], cmin, cmax, 59, 0));
      if (y >= 0 && y <= 59) {
        // ssd1306_putPixel(x, y);
        ssd1306_setColor(255);
        ssd1306_fillRect(x, y, x, 59);
      }
    }
    for (int i = 0; i < 43 - 1; i++) {
      int x = i + 43;
      int y = int(map(c2[i], cmin, cmax, 59, 0));
      if (y >= 0 && y <= 59) {
        // sd1306_putPixel(x, y);
        ssd1306_setColor(255);
        ssd1306_fillRect(x, y, x, 59);
      }
    }
    for (int i = 0; i < 42; i++) {
      int x = i + 86;
      int y = int(map(c3[i], cmin, cmax, 59, 0));
      if (y >= 0 && y <= 59) {
        // ssd1306_putPixel(x, y);
        ssd1306_setColor(255);
        ssd1306_fillRect(x, y, x, 59);
      }
    }

    if (rc) {
      if (co2ppm <= 999) // трёхзначное число пишем немного с другим отступом, чем четырёхзначное
        printIntBigFont(47, 54, co2ppm);
      else
        printIntBigFont(40, 54, co2ppm);
    }

    // Зажигаем/гасим лампочку
    if (co2ppm > 1200)
      digitalWrite(LED_BUILTIN, HIGH);
    if (co2ppm < 800) // скрепя сердце примем нижний порог 800 PPM
      digitalWrite(LED_BUILTIN, LOW);
  }

  // Записываем показания во второй и третий блок по медленным таймерам
  if (timer2) {
    for (int i = 42; i > 0; i--)
      c2[i] = c2[i - 1];
    c2[0] = co2ppm;
  }

  if (timer3) {
    for (int i = 41; i > 0; i--)
      c3[i] = c3[i - 1];
    c3[0] = co2ppm;
  }

  delay(10);
}

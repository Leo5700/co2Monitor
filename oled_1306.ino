
// Экран SSD1306 https://a.aliexpress.com/_AkEVMG из магазина https://greatzt.aliexpress.ru/store/1916536
// Oled-дисплей 0,96 дюйма, IIC, последовательный белый OLED-дисплей,
// модуль 128X64 I2C SSD1306 12864 плата с ЖК-экраном GND VDD SCK SDA для Arduino
// Мне попался экран не 128х64 а 128х60, проверил выводом пикселя.
// Arduino Nano, распиновка:
// GND --- GND
// VDD --- D2
// SCK --- A5 (i2c)
// SDA --- A4 (i2c)
// Верх экрана -- со стороны пинов


// Ниже -- адово костыльные функции, т.к. встроенные работают архикриво

// #include "ssd1306.h" // (добавить в головной скрипт) либа лежит на https://github.com/lexus2k/ssd1306 (впрочем, она есть в Manage libraries...)
// https://lexus2k.github.io/ssd1306/modules.html здесь есть функции
// https://lexus2k.github.io/ssd1306/group___l_c_d___f_o_n_t_s.html про шрифты
// https://lexus2k.github.io/ssd1306/nano__gfx__types_8h.html#a4ceb6be9200d0bb016cbbb87dcf5ed88 начертания

void setup_1306() { // это для сетапа основного скрипта
  pinMode(2, OUTPUT); digitalWrite(2, HIGH); // питание 3V
  ssd1306_128x64_i2c_init();
  // ssd1306_setFreeFont(free_calibri11x12); // этих потом добавляем по месту
  // ssd1306_setFixedFont(courier_new_font11x16_digits); //
  // ssd1306_setSecondaryFont(free_calibri11x12_cyrillic); //
}


void hardClear() {
  ssd1306_setColor(0x00);
  for (int i = 0; i < 64; i++) {
    ssd1306_fillRect(0, i, 128, i + 8);
  }
}

void printIntBigFont(int x, int y, int num) {
  String num_s = String(num, DEC);
  ssd1306_setFixedFont(courier_new_font11x16_digits);
  ssd1306_printFixed(x, y, (uint8_t *)num_s.c_str(), STYLE_NORMAL);
}


void printIntSmallFont(int x, int y, int num) {
  String num_s = String(num, DEC);
  ssd1306_setFreeFont(free_calibri11x12);
  ssd1306_printFixed(x, y, (uint8_t *)num_s.c_str(), STYLE_NORMAL);
}


void printStringSmallFont(int x, int y, String s) {
  ssd1306_setFreeFont(free_calibri11x12);
  ssd1306_setSecondaryFont(free_calibri11x12_cyrillic);
  ssd1306_printFixed(x, y, (uint8_t *)s.c_str(), STYLE_NORMAL);
}

void printFloatSmallFont(int x, int y, float num) {
  String num_s = String(num, DEC);
  ssd1306_setFreeFont(free_calibri11x12);
  ssd1306_printFixed(x, y, (uint8_t *)num_s.c_str(), STYLE_NORMAL);
}

void printLongSmallFont(int x, int y, long num) {
  String num_s = String(num, DEC);
  ssd1306_setFreeFont(free_calibri11x12);
  ssd1306_printFixed(x, y, (uint8_t *)num_s.c_str(), STYLE_NORMAL);
}

// Строим график
// for (int i = 0; i < 128; i++) {
//   int y = int(sin(float(i) / 10.0 + k / 2.0) * 10.0 + 52);
//   ssd1306_putPixel(i, y);
// }

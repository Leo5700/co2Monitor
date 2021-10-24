// CO2 meter T6703
// Куплен на алике https://a.aliexpress.com/_AKztV7 в магазине Shop5617180 https://a.aliexpress.com/_9QTr4h
// По наводке https://habr.com/ru/company/lamptest/blog/536294/
// Подключение к Arduino Nano или Uno:
// Сенсор: SDA - A4, SCL - A5, V++ - 5V, GND - GND, CTRL - GND
// Смело подключаем и датчик и экран к пинам i2c A4 и A5, wire разберётся.
// Расположение контактов сенсора. Вид снизу. Используются 5 контактов:
// SDA, SCL, V++, GND, CTRL (CTRL должен быть соединен с GND).
// Максимальная частота показаний -- раз в 2 секунды. Лучше 5.
//  -----------------SDA
//  |               |SCL
//  |     T6703     |V++
//  |               |GND
//  |               |PWM
//  -----------------CTRL
// Вместе с ардуинкой, жрёт 0.030А в фоне и 0.150А в момент срабатывания
// датчика (датчик жрёт ток примерно 1 секунду)
// На датчике не предусмотрено узлов крепления, это подстава.
// Зато он самокалибрующийся, не ужна беготня на улицу.


//Эти команды пришлось зафигачить в основной скрипт
//#include <Wire.h>
//uint8_t rc;
//int co2ppm = 0;

//#define T67XX_I2C_ADDRESS                                       (0x15)
//#define T67XX_I2C_REQUEST_SIZE                                  (0x05)
//#define T67XX_I2C_RESPONSE_SIZE                                 (0x04)
//#define T67XX_I2C_RESPONSE_DELAY                                (0x0A)
//#define T67XX_I2C_FIELD_FUNCTION_CODE                           (0x00)
//#define T67XX_I2C_FIELD_BYTES_COUNT                             (0x01)
//#define T67XX_I2C_FIELD_DATA_MSB                                (0x02)
//#define T67XX_I2C_FIELD_DATA_LSB                                (0x03)

uint8_t getT67XXMetric(uint8_t _i2cAddress, uint16_t* _value) {
  //
  // Получаем PPM
  //
  uint8_t rc = false;
  uint8_t rawData[T67XX_I2C_REQUEST_SIZE] = {0x04, 0x13, 0x8B, 0x00, 0x01};

  Wire.beginTransmission(_i2cAddress);
  Wire.write(rawData, T67XX_I2C_REQUEST_SIZE);
  Wire.endTransmission();

  delay(T67XX_I2C_RESPONSE_DELAY);

  if (T67XX_I2C_RESPONSE_SIZE != Wire.requestFrom(_i2cAddress, T67XX_I2C_RESPONSE_SIZE)) {
    goto finish;
  }

  for (uint8_t i = 0x00; T67XX_I2C_RESPONSE_SIZE > i; i++) {
    rawData[i] = Wire.read();
  }

  if (0x04 != rawData[T67XX_I2C_FIELD_FUNCTION_CODE] || 0x02 != rawData[T67XX_I2C_FIELD_BYTES_COUNT]) {
    goto finish;
  }

  *_value = ((uint16_t) rawData[T67XX_I2C_FIELD_DATA_MSB] << 8) | rawData[T67XX_I2C_FIELD_DATA_LSB];

  rc = true;

finish:
  return rc;
}

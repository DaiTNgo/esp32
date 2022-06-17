#include <DHT.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <queue.h>
#include <task.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 6
#define DHTTYPE DHT11
#define gas_ain A0 // Aout ket noi Pin A0
#define gas_din 9  // Dout ket noi Pin 9
#define warnPin 10 // loa,den ket noi Pin 10

LiquidCrystal_I2C lcd(0x27, 16, 2);

DHT dht(DHTPIN, DHTTYPE);

byte degree[8] = {
  0B01110,
  0B01010,
  0B01110,
  0B00000,
  0B00000,
  0B00000,
  0B00000,
  0B00000
};

int button_value; // bien luu trang thai button
float ad_value;     // bien luu gia tri adc
float per;


SemaphoreHandle_t interruptSemaphore;
QueueHandle_t queueGas;
QueueHandle_t queueDht11_Humidity;
QueueHandle_t queueDht11_Temperature;
TaskHandle_t xReadSs;
TaskHandle_t xHandler;

void setup()
{
  Serial.begin(9600);
  Serial2.begin(115200);
  pinMode(gas_din, INPUT);  // set Input
  pinMode(gas_ain, INPUT);  // set Input
  pinMode(warnPin, OUTPUT); // set Output
  pinMode(2, INPUT_PULLUP);

  dht.begin(); // Khởi động cảm biến

  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Nhiet do: ");
  lcd.setCursor(0, 1);
  lcd.print("Do am: ");
  lcd.createChar(1, degree);

  queueGas = xQueueCreate(1, sizeof(int));
  queueDht11_Humidity = xQueueCreate(1, sizeof(float));
  queueDht11_Temperature = xQueueCreate(1, sizeof(float));

  interruptSemaphore = xSemaphoreCreateBinary();
  if (interruptSemaphore != NULL) {
    attachInterrupt(digitalPinToInterrupt(2), interruptHandler, LOW);
  }
  xTaskCreate(Gas_task, "Gas task", 1024, NULL, 7, NULL);
  xTaskCreate(Dht11_task, "DHT11 task", 1024, NULL, 6, NULL);
  xTaskCreate(Lcd_task, "Lcd task", 2048, NULL, 5, NULL);
  //  xTaskCreate(Lamp_task, "Lamp task", 120, NULL, 4, NULL);
  //  xTaskCreate(Button_task, "Button task", 128, NULL, 0, NULL );
  vTaskStartScheduler();
  for (;;) {}
}

void Dht11_task(void *pvParameter)
{
  while (1)
  {
    //    delay(2000);
    Serial.println("dht11 task\n");
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(temperature) || isnan(humidity))
    {
      Serial.println("dht11 read fail\n");
    }
    else
    {
      Serial.print("Nhiet do: ");
      Serial.println(humidity); // Xuất nhiệt độ
      Serial.print("Do am: ");
      Serial.println(temperature); // Xuất độ ẩm
      Serial.println();          // Xuống hàng
      xQueueSend(queueDht11_Humidity, (void *)&humidity, (TickType_t)0);
      xQueueSend(queueDht11_Temperature, (void *)&temperature, (TickType_t)0);
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void Lcd_task(void *arg)
{
  float gBuffer_h;
  float gBuffer_t;
  while (1)
  {
    Serial.println("lcd task\n");
    xQueueReceive(queueDht11_Humidity, &gBuffer_h, (TickType_t)0);
    xQueueReceive(queueDht11_Temperature, &gBuffer_t, (TickType_t)0);

    lcd.setCursor(10, 0);
    lcd.print(gBuffer_t);
    lcd.print(" ");
    lcd.write(1);
    lcd.print("C");
    lcd.setCursor(10, 1);
    lcd.print(gBuffer_h);
    lcd.print(" %");

    Serial.print("Nhiet do: ");
    Serial.println(gBuffer_t); // Xuất nhiệt độ
    Serial.print("Do am: ");
    Serial.println(gBuffer_h); // Xuất độ ẩm
    Serial.println();          // Xuống hàng

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void Gas_task(void *arg)
{
  while (1) {

    Serial.println("GAS task");

    ad_value = analogRead(gas_ain);       // doc gia tri adc
    per = map(ad_value, 0, 1023, 0, 100); // Chuyển nồng độ khí gas về đơn vị %.
    if (digitalRead(gas_din) == LOW)      // so sanh vs LOW, neu = thong bao co gas, den va loa bat
    {
      Serial.println("CO GAS");
      Serial.println(ad_value);
      digitalWrite(warnPin, HIGH);
    }

    // button_value = digitalRead(button); // doc trang thai button
    // if (button_value == HIGH)           // nhan button
    // {
    //   while (button_value == HIGH) // doi phim nha
    //   {
    //     button_value = digitalRead(button);
    //   }
    //   digitalWrite(warnPin, LOW);    // tat loa va den
    //   Serial.println("BINH THUONG"); // thong bao binh thuong
    // }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void Lamp_task(void *pvPara)
{

  float gBuffer_h;
  float gBuffer_t;
  while (1)
  {
    Serial.println("Lamp task");
    xQueueReceive(queueDht11_Humidity, &gBuffer_h, (TickType_t)0);
    xQueueReceive(queueDht11_Temperature, &gBuffer_t, (TickType_t)0);


    String chuoiSendWebJson = "{\"nhietdo\":\"" + String(gBuffer_t) + "\","
                              + "\"doam\":\"" + String(gBuffer_h) + "\","
                              + "\"gas\":\"" + String(per) + "\"}";
    Serial2.println(chuoiSendWebJson);
    Serial2.flush();
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void interruptHandler() {
  xSemaphoreGiveFromISR(interruptSemaphore, NULL);
}

void Button_task(void *pvParameters)
{
  (void) pvParameters;

  for (;;) {
    Serial.println("Button Task");
    if (xSemaphoreTake(interruptSemaphore, portMAX_DELAY) == pdPASS) {
      digitalWrite(warnPin, LOW);    // tat loa va den
      Serial.println("BINH THUONG"); // thong bao binh thuong
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void loop()
{
}

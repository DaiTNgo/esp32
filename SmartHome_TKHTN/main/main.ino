#include <DHT.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int DHTPIN = 2;      //Đọc dữ liệu từ DHT11 ở chân 2 trên mạch Arduino
const int DHTTYPE = DHT11; // Khai báo loại cảm biến, có 2 loại là DHT11 và DHT22

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

int ad_value;     // bien luu gia tri adc
int per;

#define gas_ain A0 // Aout ket noi Pin A0
#define gas_din 9  // Dout ket noi Pin 9
#define button 2   // button ket noi Pin 2
#define warnPin 10 // loa,den ket noi Pin 10
#define ledRed 8
#define ledGreen 7
#define gasSensor 6
#define buzzer 4

SemaphoreHandle_t interruptSemaphore;

QueueHandle_t queueGas;
QueueHandle_t queueDht11_Humidity;
QueueHandle_t queueDht11_Temperature;
TaskHandle_t xReadSs;
TaskHandle_t xHandler;

// xSemaphoreTake(xSemaphore, portMAX_DELAY);
void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600);
  pinMode(gas_din, INPUT);  // set Input
  pinMode(gas_ain, INPUT);  // set Input
  pinMode(warnPin, OUTPUT); // set Output
  pinMode(button, INPUT);   // set Input

  dht.begin(); // Khởi động cảm biến

  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Nhiet do: ");
  lcd.setCursor(0, 1);
  lcd.print("Do am: ");
  lcd.createChar(1, degree);

  pinMode(gasSensor, INPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(buzzer, OUTPUT);

  queueGas = xQueueCreate(1, sizeof(int));

  queueDht11_Humidity = xQueueCreate(1, sizeof(float));
  queueDht11_Temperature = xQueueCreate(1, sizeof(float));

  dht.begin();

  interruptSemaphore = xSemaphoreCreateBinary();
   if (interruptSemaphore != NULL) {
    attachInterrupt(digitalPinToInterrupt(2), interruptHandler, LOW);
  }
  xTaskCreate(Gas_task, "Gas task", 2048, NULL, 7, NULL);
  xTaskCreate(Dht11_task, "DHT11 task", 2048, NULL, 6, NULL);
  xTaskCreate(Lcd_task, "Lcd task", 2048, NULL, 5, NULL);
  xTaskCreate(Lamp_task, "Lamp task", 120, NULL, 4, NULL);
  xTaskCreate(Button_task,"Led",128,NULL,0,NULL );
  vTaskStartScheduler();
  for (;;) {}
}

void Dht11_task(void *pvParameter)
{
  while (1)
  {
    Serial.println("dht11 task\n");
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(temperature) || isnan(humidity))
    {
      Serial.println("dht11 read fail\n");
    }
    else
    {
      xQueueSend(queueDht11_Humidity, (void *)&humidity, (TickType_t)0);
      xQueueSend(queueDht11_Temperature, (void *)&temperature, (TickType_t)0);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
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
    lcd.print(round(gBuffer_t));
    lcd.print(" ");
    //      lcd.write(1);
    lcd.print("C");
    lcd.setCursor(10, 1);
    lcd.print(round(gBuffer_h));
    lcd.print(" %");

    Serial.print("Nhiet do: ");
    Serial.println(gBuffer_t); // Xuất nhiệt độ
    Serial.print("Do am: ");
    Serial.println(gBuffer_h); // Xuất độ ẩm
    Serial.println();          // Xuống hàng

    vTaskDelay(100 / portTICK_PERIOD_MS);
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
    vTaskDelay(50 / portTICK_PERIOD_MS);
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

  pinMode(LED_BUILTIN, OUTPUT);

  for (;;) {

    if (xSemaphoreTake(interruptSemaphore, portMAX_DELAY) == pdPASS) {
      // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    // if (button_value == HIGH)           // nhan button
    // {
    //   while (button_value == HIGH) // doi phim nha
    //   {
    //     button_value = digitalRead(button);
    //   }
    // }
      digitalWrite(warnPin, LOW);    // tat loa va den
      Serial.println("BINH THUONG"); // thong bao binh thuong
    }

    vTaskDelay(10);
  }
}

void loop()
{
}

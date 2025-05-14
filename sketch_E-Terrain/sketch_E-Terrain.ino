#include <Wire.h>
#include "Adafruit_SI1145.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define SOIL_SENSOR_PIN A0  
#define LED_PIN 13        
int dryValue = 800;  
int wetValue = 300;  
int moisturePercent = 0;

#define MQ135_PIN A1  
int airQuality = 0;

#define RAIN_SENSOR_PIN A2  
int rainValue = 0;

#define DHTPIN 2       
#define DHTTYPE DHT12  
DHT dht(DHTPIN, DHTTYPE);

Adafruit_SI1145 sensor = Adafruit_SI1145();

void setup() {
    Serial.begin(9600);
    
    pinMode(SOIL_SENSOR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(RAIN_SENSOR_PIN, INPUT);
    
    dht.begin();
    
    if (!sensor.begin()) {
        Serial.println("⚠️ Si1145 UV Sensor Not Found! Check Wiring.");
        while (1);
    }

    Serial.println("✅ Sensors Initialized!");
    delay(2000);
}

void loop() {
    Serial.println("\n===== 🌍 SMART FARMING SENSOR READINGS =====");

    int soilValue = analogRead(SOIL_SENSOR_PIN); 
    moisturePercent = map(soilValue, dryValue, wetValue, 0, 100);
    moisturePercent = constrain(moisturePercent, 0, 100);

    Serial.print("🌿 Soil Moisture: ");
    Serial.print(moisturePercent);
    Serial.println("%");

    if (moisturePercent < 30) {
        Serial.println("🚨 Dry Soil! Water Needed!");
        digitalWrite(LED_PIN, HIGH);
    } else {
        digitalWrite(LED_PIN, LOW);
    }

    airQuality = analogRead(MQ135_PIN);
    Serial.print("🌫 Air Quality Index: ");
    Serial.print(airQuality);

    float co2Level = map(airQuality, 200, 900, 400, 5000);
    co2Level = constrain(co2Level, 400, 5000);
    Serial.print(" | CO2 Level: ");
    Serial.print(co2Level);
    Serial.println(" ppm");

    String soilFertility = getSoilFertility(moisturePercent, airQuality);
    Serial.print("🌾 Soil Fertility Status: ");
    Serial.println(soilFertility);

    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temp) || isnan(humidity)) {
        Serial.println("⚠ Failed to read from DHT12 sensor!");
    } else {
        Serial.print("🌡 Temperature: ");
        Serial.print(temp);
        Serial.print(" °C  |  💧 Humidity: ");
        Serial.print(humidity);
        Serial.println(" %");

        float heatIndex = computeHeatIndex(temp, humidity);
        Serial.print("🔥 Feels Like: ");
        Serial.print(heatIndex);
        Serial.println(" °C");

        float dewPoint = computeDewPoint(temp, humidity);
        Serial.print("💧 Dew Point: ");
        Serial.print(dewPoint);
        Serial.println(" °C");

        String comfortLevel = getComfortLevel(temp, humidity);
        Serial.print("😊 Comfort Level: ");
        Serial.println(comfortLevel);
    }

    rainValue = analogRead(RAIN_SENSOR_PIN);
    Serial.print("🌧 Rain Sensor Value: ");
    Serial.println(rainValue);
    if (rainValue < 500) {
        Serial.println("🚨 Heavy Rain Detected!");
    }

    Serial.print("☀ Visible Light: "); Serial.println(sensor.readVisible());
    Serial.print("🌙 IR Light: "); Serial.println(sensor.readIR());
    Serial.print("🛡 UV Index: "); Serial.println(sensor.readUV() / 100.0);  

    Serial.println("====================================");
    delay(2000);
}

float computeHeatIndex(float temp, float humidity) {
    return -8.784695 + 1.61139411 * temp + 2.338549 * humidity - 0.14611605 * temp * humidity 
           - 0.012308094 * pow(temp, 2) - 0.01642482 * pow(humidity, 2) 
           + 0.002211732 * pow(temp, 2) * humidity + 0.00072546 * temp * pow(humidity, 2) 
           - 0.000003582 * pow(temp, 2) * pow(humidity, 2);
}

float computeDewPoint(float temp, float humidity) {
    float a = 17.27;
    float b = 237.7;
    float alpha = ((a * temp) / (b + temp)) + log(humidity / 100.0);
    return (b * alpha) / (a - alpha);
}

String getComfortLevel(float temp, float humidity) {
    if (temp > 30 && humidity > 70) return "Too Hot & Humid 🥵";
    if (temp > 30) return "Too Hot 🔥";
    if (temp < 10) return "Too Cold ❄";
    if (humidity > 70) return "High Humidity 😓";
    if (humidity < 30) return "Low Humidity 🏜";
    return "Comfortable 😊";
}

String getSoilFertility(int moisture, int airQuality) {
    if (moisture > 60 && airQuality < 400) return "Highly Fertile 🌱";
    if (moisture > 40 && airQuality < 600) return "Moderately Fertile 🌾";
    return "Poor Fertility 🚨";
}
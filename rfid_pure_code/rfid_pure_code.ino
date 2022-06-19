#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 21
#define RST_PIN 2
MFRC522 mfrc522(SS_PIN, RST_PIN);
int green = 14;
int buzzer = 12;
int red = 13;
bool rfid_tag_present_prev = false;
bool rfid_tag_present = false;
int _rfid_error_counter = 0;
bool _tag_found = false;

void setup()
{
    pinMode(red, OUTPUT);
    pinMode(green, OUTPUT);
    pinMode(buzzer, OUTPUT);
    Serial2.begin(9600, SERIAL_8N1, 27, 26);
    Serial.begin(115200);
    SPI.begin();
    mfrc522.PCD_Init();
    Serial.println("trust the power of thought");
    delay(100);
}

String content = "";
void loop()
{
Serial.println("#include<QTcpSocket>");
    rfid_tag_present_prev = rfid_tag_present;
    _rfid_error_counter += 1;
    if (_rfid_error_counter > 2)
    {
        _tag_found = false;
    }
    byte bufferATQA[2];
    byte bufferSize = sizeof(bufferATQA);
    mfrc522.PCD_WriteRegister(mfrc522.TxModeReg, 0x00);
    mfrc522.PCD_WriteRegister(mfrc522.RxModeReg, 0x00);
    mfrc522.PCD_WriteRegister(mfrc522.ModWidthReg, 0x26);
    MFRC522::StatusCode result = mfrc522.PICC_RequestA(bufferATQA, &bufferSize);
    if (result == mfrc522.STATUS_OK)
    {
        if (!mfrc522.PICC_ReadCardSerial())
        {
            return;
        }
        _rfid_error_counter = 0;
        _tag_found = true;
        for (byte i = 0; i < mfrc522.uid.size; i++)
        {
            content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
            content.concat(String(mfrc522.uid.uidByte[i], HEX));
        }
        content.toUpperCase();
    }

    rfid_tag_present = _tag_found;
    if (rfid_tag_present)
    {
        if (!rfid_tag_present_prev)
        {
            Serial.println("Tag Found:" + content);
            if (content == "CC4B6623" || content == "7C7CA922" || content == "A343A894" || content == "83768194" || content == "A376EF94")
            {
                digitalWrite(green, 1);
                digitalWrite(red, 0);
                digitalWrite(buzzer, 0);
                Serial.println("Valid cards");
            }
            else
            {
                digitalWrite(green, 0);
                digitalWrite(red, 1);
                digitalWrite(buzzer, 1);
                SendMessage();
                delay(500);
                Serial.println("Invalid card");
                Serial.println("sms sent");
            }
        }
    }
    else
    {
        if (rfid_tag_present_prev)
        {
            Serial.println("No card");
            Serial.println("");
            content = "";
        }
        else
        {
            SendMessage();
            digitalWrite(green, 0);
            digitalWrite(red, 1);
            digitalWrite(buzzer, 1);
            delay(500);
            digitalWrite(buzzer, 0);
            delay(500);
        }
    }
}
void SendMessage()

{

    Serial2.println("AT+CMGF=1");                   // Sets the GSM Module in Text Mode
    delay(1000);                                     // Delay of 1000 milli seconds or 1 second
    Serial2.println("AT+CMGS=\"+255754650078\"\r"); // Replace x with mobile number
    delay(1000);
    Serial2.println("Machine is not secured"); // The SMS text you want to send
    delay(100);
    Serial2.println((char)26); // ASCII code of CTRL+Z
    delay(1000);
}

#include <DFPlayer_Mini_Mp3.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); //rx, tx
int a,b =1;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
const byte R_MESIN = 8;
const byte R_KONTAKTOR = 9;
int count_salah = 0;
int count_benar = 0;
byte status_jari;
byte hold_starter = 0;
#define READY 0
#define BENAR 1
#define SALAH 2
#define TUNGGU 3
#define AKTIF_FINGER 0
#define NON_AKTIF_FINGER 1
const int LIMIT_WAKTU = 20;
byte state = 0;
int i;
void setup()
{
 Serial.begin (9600);
 pinMode(R_KONTAKTOR, OUTPUT);
 digitalWrite(R_KONTAKTOR, HIGH);
 pinMode(R_MESIN, OUTPUT);
 digitalWrite(R_MESIN, OUTPUT);
 a=1;
 Serial.begin(9600);
 while (!Serial);
 delay(100);
 Serial.println("\n\nGoleti Sensor...");
 finger.begin(57600);

 if (finger.verifyPassword()) {
 Serial.println("Sensor Ketemu...!");
 } else {Serial.println("Sensor ora ketemu bos... :(");
 while (1) { delay(1); }
 }
 finger.getTemplateCount();
 Serial.print("Jumlah sidik jari ");
Serial.print(finger.templateCount); Serial.println(" driji");
 Serial.println("Ngenteni sidik jari sing pas...");
}
void loop()
{
 //getFingerprintIDez();
 switch(state){
 case AKTIF_FINGER:
 getFingerprintID();
 //getFingerprintIDez();
 control();
 delay(500);
 break;
 case NON_AKTIF_FINGER:

 break;
 }

}
uint8_t getFingerprintID() {
 uint8_t p = finger.getImage();
 switch (p) {
 case FINGERPRINT_OK:
 Serial.println("Image taken");
 break;
 case FINGERPRINT_NOFINGER:
 return p;
 case FINGERPRINT_PACKETRECIEVEERR:
 Serial.println("Communication error");
 return p;
 case FINGERPRINT_IMAGEFAIL:
 Serial.println("Imaging error");
 return p;
 default:
 Serial.println("Unknown error");
 return p;
 }
 p = finger.image2Tz();
 switch (p) {
case FINGERPRINT_OK:
 Serial.println("Image converted");
 break;
 case FINGERPRINT_IMAGEMESS:
 Serial.println("Image too messy");
 return p;
 case FINGERPRINT_PACKETRECIEVEERR:
 Serial.println("Communication error");
 return p;
 case FINGERPRINT_FEATUREFAIL:
 Serial.println("Could not find fingerprint features");
 return p;
 case FINGERPRINT_INVALIDIMAGE:
 Serial.println("Could not find fingerprint features");
 return p;
 default:
 Serial.println("Unknown error");
 return p;
 }

 p = finger.fingerFastSearch();
 if (p == FINGERPRINT_OK) {
 Serial.println("Found a print match!");
 Serial.println("Benar........");
 count_benar ++;
 if ( count_benar == 4) count_benar = 0;
 status_jari = BENAR;

 } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
 Serial.println("Communication error");
 return p;
 } else if (p == FINGERPRINT_NOTFOUND) {
 delay(10);
 Serial.println("Did not find a match");
 Serial.println("Salah.........");
 count_salah ++;
 if ( count_salah == 4) count_salah = 0;
 status_jari = SALAH;

 return p;
 } else {
 Serial.println("Unknown error");
 return p;
 }

 Serial.print("Driji Ketemu ID #"); Serial.print(finger.fingerID);
 Serial.print(" Pase ");
Serial.println(finger.confidence);
return finger.fingerID;
}
int getFingerprintIDez() {
 uint8_t p = finger.getImage();
 if (p != FINGERPRINT_OK) return -1;

 p = finger.image2Tz();
 if (p != FINGERPRINT_OK) return -1;
 p = finger.fingerFastSearch();
 if (p != FINGERPRINT_OK) return -1;

 Serial.print("Driji ketemu ID #"); Serial.print(finger.fingerID);
 Serial.print(" cocoke ");
Serial.println(finger.confidence);
 return finger.fingerID;
}
void control(){
 switch(status_jari){
 case READY:
 getFingerprintID();
 control();
 delay(50);
 break;
 case BENAR:
 switch(count_benar){
 case 1:
 state = AKTIF_FINGER;

 delay(1500);
 hold_starter = 0;
 b=1;
 Serial.println("Mesin Urip");
 digitalWrite(R_MESIN, LOW);
 break;

 case 2:
 delay(2000);
 hold_starter = 0;
 a=1;
 Serial.println("Kontaktor Urip");
 digitalWrite(R_KONTAKTOR, LOW);
 delay(1000);
 digitalWrite(R_KONTAKTOR, HIGH);
 Serial.println("Kontaktor mati");
 break;

case 3:
 delay(2000);
 hold_starter = 0;
 a=1;
 digitalWrite(R_KONTAKTOR, HIGH);
 Serial.println("Kontaktor mati");
 break;
 
 default:
 digitalWrite(R_MESIN, HIGH);
 Serial.println("Mesin Mati");
 }
 break;
 case SALAH:
 switch(count_salah){
 case 1:
 Serial.println("Finger print Salah 1x");
 break;
 case 2:
 Serial.println("Finger print Salah 2x");
 break;
 case 3:
 Serial.println("Finger print Salah 3x");
 status_jari = TUNGGU;
 break;
 }
 break;
 case TUNGGU:
 state = NON_AKTIF_FINGER;
 delay(10);
 for( i = 0; i < LIMIT_WAKTU; i++){

 Serial.print("Tunggu finger aktif: ");
 Serial.println(i);
 delay(1000);
 }
 state = AKTIF_FINGER;
 count_salah = 0; status_jari = 0;
 break;
 }

}

#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SPI.h>
#include <RH_RF95.h>
TinyGPS gps;
RH_RF95 rf95;
SoftwareSerial ss(3, 4); 

static void smartdelay(unsigned long ms);

int lampu=A0;

char IDPerangkat[10]="*******"; //Sesuaikan IDPerangkat LoRa 

String datastring1="";
String datastring2="";
String datastring3="";
uint8_t datasend[50];    

char bujur[50]={"\0"}; 
char lintang[20]={"\0"}; 
char ketinggian[20]={"\0"}; 
void setup()
{
  pinMode(lampu, OUTPUT);
  
  Serial.begin(9600);  
  ss.begin(9600);       
  while (!Serial);

   if (!rf95.init()) {  
    Serial.println("Jalankan LoRa Node");
    while (1);
  }
  
  rf95.setFrequency(925);
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125E3);
  rf95.setCodingRate4(5);
  rf95.setTxPower(20,false);
  
  Serial.println("Siap mengirim data");
}

void loop()
{
  float flat, flon,falt;
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
  falt=gps.f_altitude();  //get altitude       
  flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6;//save six decimal places 
  flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6;
  falt == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : falt, 2;//save two decimal places
  datastring1 +=dtostrf(flat, 0, 6, lintang); 
  datastring2 +=dtostrf(flon, 0, 6, bujur);
  datastring3 +=dtostrf(falt, 0, 2, ketinggian);
  if(flon!=1000.000000)
  {
  strcat(bujur,",");
  strcat(bujur,lintang); 
  strcat(bujur,","); 
  strcat(bujur,ketinggian);
  strcat(bujur,","); 
  strcat(bujur,DeviceID);
  strcat(bujur,",");
  strcpy((char *)datasend,bujur);
  Serial.println((char *)datasend);
  
  rf95.send(datasend, sizeof(datasend));  
  rf95.waitPacketSent();
  receivepacket();
  }
  smartdelay(1000);
}

void receivepacket(){
    uint8_t indatabuf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(indatabuf);
    
    if (rf95.waitAvailableTimeout(3000))
     {    
       if (rf95.recv(indatabuf, &len))
         {
         Serial.println((char*)indatabuf);        
         digitalWrite(lampu, HIGH);   
         }
         else 
         {
          digitalWrite(lampu, LOW);    
          Serial.println("gagal menerima data");
         }
    }
    else
    {
      digitalWrite(lampu, LOW);    
    }
}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
    {
      gps.encode(ss.read());
    }
  } while (millis() - start < ms);
}

#include <Console.h> 
#include <Process.h>
#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;

char IDPerangkat[20]="\0"; 
char bujur[20]="\0";          
char lintang[20]="\0";           
char ketinggian[20]="\0";           

void getTimeStamp();     
void receivepacket();    
void run_send_gps_data();

void setup() { 
  Bridge.begin(115200);  
  Console.begin();
  while (!Console);
  
   if (!rf95.init()) {
    Console.println("Lora gagal berkomunikasi");
    while (1);
  }

  rf95.setFrequency(868);
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125E3);
  rf95.setCodingRate4(5);
  rf95.setTxPower(20,false);
  
  Console.println("Siap menerima data");
}
void loop(){
 receivepacket();
}

void getTimeStamp() {
  Process time;
  time.begin("date");
  time.addParameter("+%D-%T");  
  time.run();  

  while(time.available()>0) {
    char c = time.read();
    Console.print(c);
  }
}

void run_send_gps_data() {
  Process pcs;    
  pcs.begin("send_gps_data.sh");
  pcs.addParameter("-d");
  pcs.addParameter(IDPerangkat);
  pcs.addParameter("-l"); 
  pcs.addParameter(bujur);
  pcs.addParameter("-n"); 
  pcs.addParameter(lintang);
  pcs.addParameter("-a"); 
  pcs.addParameter(ketinggian);
  pcs.run();  
}

//Receiver LoRa bus tracker IPB
void receivepacket() {
   if (rf95.available())
  {
    Console.print("Dapatkan data baru: ");

    int i = 0,j=0,code[4];
    int m1=0,m2=0,m3=0,m4=0;   
    uint8_t buf[50];
    char message[50]="\0";
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len)){
      strcpy(message,(char *)buf);
      while(i<50)
      {
      if(message[i]==',')
      {
        code[j]=i;
        j++;
        }
        i++;
      }
    }
    for(int k=0;k<code[0];k++)
    {
      bujur[m1]=message[k];
      m1++;
    }
     for(int k=code[0]+1;k<code[1];k++)
    {
      lintang[m2]=message[k];
      m2++;
    }
     for(int k=code[1]+1;k<code[2];k++)
    {
      ketinggian[m3]=message[k];
      m3++;
    }
    for(int k=code[2]+1;k<code[3];k++)
    {
      IDPerangkat[m4]=message[k];
      m4++;
    }
    run_send_gps_data();
    Console.print((char*)buf);
    Console.print("  with RSSI: ");
    Console.print(rf95.lastRssi(), DEC);
    Console.print("  ");getTimeStamp();
    
    uint8_t data[] = "Gateway menerima data";
    rf95.send(data, sizeof(data));
    rf95.waitPacketSent();
  }
}

#include <SoftwareSerial.h>
#include <TinyGPS.h>

TinyGPS gps;

#define car_id "Dhaka Metro Ga-12-157667" //change it as manual
 
SoftwareSerial serialSIM808(2, 3);
SoftwareSerial serialGPS(4, 5);

char lati[16]= "23.780456";
char loni[16]= "90.420654";

char fuel[3];

char state;

void setup() {
  pinMode(A0,INPUT);
  Serial.begin(9600);
  while(!Serial);
   
  serialSIM808.begin(9600);
  delay(1000);
  
  Serial.println("SIM808 Initialized");
  serialSIM808.write("AT+CREG?\r\n");
  delay(150);
  gsm_connect_gprs();

  serialGPS.begin(4800);
  Serial.println("GPS Initialized");
}
 
void loop() {
  getFuel();
  getGPS();

  if(Serial.available()){
    state = Serial.read();
  }

  switch(state){
    case 's':
    gsm_connect_gprs();
    gsm_send_data(lati,loni,fuel);
    Serial.println();
    delay(2000);
    state = 'x';
    break;
    default:
    break;
  }
}

void gsm_connect_gprs(){
  serialSIM808.write("AT+CGATT=1\r\n"); 
  delay(2000);
  serialSIM808.write("AT+SAPBR=1,1\r\n"); 
  delay(2000);
  Serial.println("Data Connection on");
}

void gsm_disconnect_gprs(){
  serialSIM808.write("AT+CGATT=0\r\n");
  delay(2000);
  Serial.println("Data Connection Off");
}

void gsm_send_data(char * lati, char * loni, char * f)
{
    Serial.println("Sending data.");    
    serialSIM808.write("AT+HTTPINIT\r\n");  
    delay(1000);
    serialSIM808.write("AT+HTTPPARA=\"URL\",\"http://fusion.krittimmanush.com/api/fuel.php?act=insert&q1=");
    Serial.print("http://fusion.krittimmanush.com/api/fuel.php?act=insert&q1=");
    serialSIM808.write(car_id);   
    Serial.print(car_id);
    delay(50);
    serialSIM808.write("&q2="); 
    Serial.print("&q2=");
    delay(50);
    serialSIM808.write(lati);   
    Serial.print(lati);
    delay(50);
    serialSIM808.write("&q3="); 
    Serial.print("&q3=");
    delay(50);
    serialSIM808.write(loni);   
    Serial.println(loni);
    delay(50);
    serialSIM808.write("&q4="); 
    Serial.print("&q4=");
    delay(50);
    serialSIM808.write(f);   
    Serial.println(f);
    delay(50);
    serialSIM808.write("\"\r\n");   
    delay(2000);
    serialSIM808.write("AT+HTTPPARA=\"CID\",1\r\n"); 
    delay(2000);
    serialSIM808.write("AT+HTTPACTION=0\r\n");
    delay(3000);    
    serialSIM808.write("AT+HTTPTERM\r\n");
    delay(3000);    
    Serial.println("Data sent complete");
}


void getFuel(){
  int x = analogRead(A0);
  int y = map(x, 30, 200, 100, 0);
  if (y < 0) {
    y = 0;
  }
  if (y > 100) {
    y = 100;
  }
  itoa(y, fuel, 10);
}

void getGPS(){
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;
  
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (serialGPS.available())
    {
      char c = serialGPS.read();
      if (gps.encode(c))
        newData = true;
    }
  }

  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    itoa(flat, lati, 10);
    itoa(flon, loni, 10);
  }
}


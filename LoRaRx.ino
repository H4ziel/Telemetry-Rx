/*==================================================================================================================================================================

Telemetry System - v.01 main.ino

Author: Joao Ricardo Chaves and Marcelo Haziel
Date: 2024, March.

====================================================================================================================================================================*/

//==========================================================================
//--Biblioteca--
#include "LoraConfigRx.h"
#include <Arduino.h>

//==========================================================================
//--Task Prototipos--
void menuDisp(void *p);
//void DataExcel(void *p);
void SensorData(void *p);

//===========================================================================================================
//---Functions---
void menugyro();
void SetupLoRa();

//===========================================================================================================
//---Interruption Definition---
void IRAM_ATTR readBotton();

//==========================================================================
//--Main Function--
void setup() 
{
  Serial.begin(9600);
  Heltec.begin(DisplayEstado, LoRaEstado, SerialEstado, AmplificadorDePotencia, BAND);
  SetupLoRa(); 
  Wire.begin(SDA_PIN, SCL_PIN);
  pinMode(BottonEnter,INPUT_PULLUP);                                              // Utilização de resistor de pullup Interno, verificar se o GPIO 
  pinMode(BottonUp,INPUT_PULLUP);                                                 // possui o a possibilidade de pullup ou pulldown, nem todos existem.
  pinMode(BottonDown,INPUT_PULLUP);
  pinMode(BottonExit,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BottonEnter),readBotton,FALLING);         // Inicialização da interrupção interna, no modo FALLING(Mudança de HIGH p/
  attachInterrupt(digitalPinToInterrupt(BottonUp),readBotton,FALLING);            // LOW)
  attachInterrupt(digitalPinToInterrupt(BottonDown),readBotton,FALLING);
  attachInterrupt(digitalPinToInterrupt(BottonExit),readBotton,FALLING);
  lcd.init();                                                                     // INICIA A COMUNICAÇÃO COM O DISPLAY
  lcd.backlight();                                                               // LIGA A ILUMINAÇÃO DO DISPLAY
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Telemetry System");
  lcd.setCursor(0,1);
  lcd.print("Abutres - v.01");
  delay(2000);
  xTaskCreate(menuDisp,"menuDisp",7000,NULL,4,NULL);                              // Inicialização da task menuDisp, utiliza o nucleo 1 como padrão       
  xTaskCreate(DataExcel,"DataExcel",1000,NULL,2,NULL);
  xTaskCreate(SensorData,"SensorData",1000,NULL,3,NULL);
  disableCore0WDT();                                                              // Desabilita o Watchdog Timer do nucleo 0
  disableCore1WDT();                                                              // Desabilita o Watchdog Timer do nucleo 1
}//end setup

void loop() 
{
  //==================================
  //---
}// end function loop

//==========================================================================
//--Functions/Tasks/Interruptions--

//==========================================================================
//--Function Setup Lora--
void SetupLoRa()
{
  SPI.begin(LORA_SCK_PIN, LORA_MISO_PIN, LORA_MOSI_PIN, LORA_SS_PIN);
  LoRa.setPins(LORA_SS_PIN, LORA_RST_PIN, LORA_DI00_PIN);
  if (!LoRa.begin(BAND, AmplificadorDePotencia)){
  }//end if
  LoRa.setSpreadingFactor(10); //define o SF
  LoRa.setSignalBandwidth(250E3); //define a BW
  LoRa.setCodingRate4(5); //define o CR
  LoRa.setPreambleLength(6); // define o comprimento do Preambulo
  LoRa.setSyncWord(0x12); // define a palavra de sincronização
  LoRa.crc(); // ativa a Checagem de Redundância Cilíndrica
}//end SetupLora

//===========================================================================================================
//---SensorData---
void SensorData(void *p)
{
  while(1)
  {
    int packetSize = LoRa.parsePacket();

    if (packetSize) // se o pacote n tiver vazio
    {
      while (LoRa.available())
      {
        Dados = LoRa.readString();

        //pegando os index dos caracteres especiais para posteriormente pegar as variáveis.
        int index1 = Dados.indexOf('#');
        int index2 = Dados.indexOf('@');
        int index3 = Dados.indexOf('%');
        int index4 = Dados.indexOf('&');
        int index5 = Dados.indexOf('D');
        int index6 = Dados.indexOf('E');
        int index7 = Dados.indexOf('A');
        int index8 = Dados.indexOf('B');

        //adquirindo os dados dos sensores de acordo com os index's da string de recebimento
        temp = Dados.substring(0, index1);
        AngleRoll = Dados.substring(index1 + 1, index2);
        AnglePitch = Dados.substring(index2 + 1, index3);
        altitude = Dados.substring(index3 + 1, index4);
        velocidade = Dados.substring(index4 + 1, index5);
        pressao = Dados.substring(index5 + 1, index6);
        sat = Dados.substring(index6 + 1, index7);
        Lat = Dados.substring(index7 + 1, index8);
        Lng = Dados.substring(index8+1, Dados.length());
      }//end while aninhado
      //RSSI e SNR da comunicação
      SNR_dB = LoRa.packetSnr();
      RSSI_dBm = LoRa.packetRssi();
    }//end if
  }//end while
}//end SensorData

//===========================================================================================================
//---DataExcel---

void DataExcel(void *p)
{
  //Coleta para Excel
  while(1)
  {
    Serial.print(SNR_dB);
    Serial.print(",");
    Serial.print(RSSI_dBm);
    Serial.print(",");
    Serial.print(AngleRoll);
    Serial.print(",");
    Serial.print(AnglePitch);
    Serial.print(",");
    Serial.print(temp);
    Serial.print(",");
    Serial.print(velocidade);
    Serial.print(",");
    Serial.print(altitude);
    Serial.print(",");
    Serial.print(pressao);
    Serial.print(",");
    Serial.print(Lat);
    Serial.print(",");
    Serial.print(Lng);
    Serial.println();
  
    delay(1000);
  }//end While
}//end DataExcel



//===========================================================================================================
//---menuDisp---
void menuDisp(void *p)
{
  int cont=0;
  while(1)
  { 
    //Serial.print("Cont: ");
    //Serial.println(cont);
    //Serial.print("Menu[cont]: ");
    //Serial.println(Menu[cont]);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(">");
    lcd.print(Menu[cont]);
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.print(Menu[(cont+1)<6?(cont+1):0]);
    while(!DownPressed && !UpPressed && !EnterPressed && !ExitPressed) 
    {
      vTaskDelay(200/portTICK_PERIOD_MS); // Pequeno atraso para evitar consumo excessivo de recursos
    }//end while

    if(DownPressed)
    {
      cont = (cont+1)<6?(cont+1):0;
      DownPressed = false;
    }//end if DownPressed
    else if(UpPressed)
    {
      cont = (cont-1)>=0 ? (cont-1):5;
      UpPressed = false;
    }//end UpPressed
    else if(EnterPressed)
    {
      switch(cont)
      {
        case 0:
          while(ExitPressed != true)
          {
            //Serial.print("case 0");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("SNR:"+ String(SNR_dB));
            lcd.setCursor(0,1);
            lcd.print("RSSI:"+ String(RSSI_dBm));
            vTaskDelay(1000/portTICK_PERIOD_MS);
          }//end while
          break;        
        case 1:
          while(ExitPressed != true)
          {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Temperatura:");
            lcd.setCursor(0,1);
            lcd.print(String(temp));
            lcd.write(B11011111);
            lcd.print("C");
            vTaskDelay(1000/portTICK_PERIOD_MS);
          }//end while
          break;
        case 2:
          menuMPU6050();
          break;
        case 3:
          while(ExitPressed != true)
          {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Altitude:");
            lcd.setCursor(0,1);
            lcd.print(String(altitude));            
            vTaskDelay(500/portTICK_PERIOD_MS);
          }//end while
          break;
        case 4:
          while(ExitPressed != true)
          {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Velocidade:");
            lcd.setCursor(0,1);
            lcd.print(String(velocidade));
            lcd.print(" Km/h");
            vTaskDelay(500/portTICK_PERIOD_MS);
          }//end while
          break;
        case 5:
          while(ExitPressed != true)
          {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Pressao:");
            lcd.setCursor(0,1);
            lcd.print(String(pressao));
            vTaskDelay(500/portTICK_PERIOD_MS);
          }//end while
          break;
        default:
          cont = 0;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Comando Invalido");
          vTaskDelay(500/portTICK_PERIOD_MS);
          break;
      }//end switch
      EnterPressed = false;
    }//end EnterPressed
    else if(ExitPressed)
    {
      if(cont >= 0 && cont <= 5 )
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(">");
        lcd.print(Menu[0]);
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.print(Menu[1]);
      }//end if
        cont = 0;
        ExitPressed = false;
    }//end ExitPressed
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }//end while
}//end menuDisp

//===========================================================================================================
//---readBotton---
void IRAM_ATTR readBotton()
{
  static unsigned long lastMillis = 0;
  unsigned long newMillis = xTaskGetTickCount();
  if(newMillis - lastMillis < 300)
  {
  }//end if
  else if(digitalRead(BottonUp)==LOW)
  {
    UpPressed = true;
    lastMillis = newMillis;
  }//end else if aninhado
  else if(digitalRead(BottonDown)==LOW)
  {
    DownPressed = true;
    lastMillis = newMillis;
  }//end else if aninhado
  else if(digitalRead(BottonEnter)==LOW)
  {
    EnterPressed = true;
    lastMillis = newMillis;
  }//end else if aninhado
  else if(digitalRead(BottonExit)==LOW)
  {
    ExitPressed = true;
    lastMillis  = newMillis;
  }//end else if aninhado
}//end readBotton

//===========================================================================================================
//---MenuGyro---
void menuMPU6050()
{
  int contmenu=0;
  while(ExitPressed != true)
  {
    while(!DownPressed && !UpPressed && !EnterPressed && !ExitPressed) 
    {
      vTaskDelay(100/portTICK_PERIOD_MS); // Pequeno atraso para evitar consumo excessivo de recursos
    }//end while

    MPUValues[0] = AngleRoll;
    MPUValues[1] = AnglePitch;
/*
    Serial.println("============================");
    Serial.print("GyroX:");
    Serial.println(gyroValues[0]);
    Serial.print("GyroY:");
    Serial.println(gyroValues[1]);
    Serial.print("GyroZ:");
    Serial.println(gyroValues[2]);
    Serial.print("AngleX:");
    Serial.println(gyroValues[3]);
    Serial.print("AngleY:");
    Serial.println(gyroValues[4]);
    Serial.print("AngleZ:");
    Serial.println(gyroValues[5]);
    Serial.println("============================");
*/
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(">");
    lcd.print(MenuMPU6050[contmenu]+":" + MPUValues[contmenu]);
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.print(MenuMPU6050[(contmenu+1)<2 ? (contmenu+1):0]+":"+ MPUValues[(contmenu+1)<2 ? (contmenu+1):0]);

    vTaskDelay(200 / portTICK_PERIOD_MS);
    if(DownPressed)
    {
      contmenu = (contmenu+1)<2 ? (contmenu+1) : 0;
      DownPressed = false;
    }//end downPressed
    else if(UpPressed)
    {
      contmenu = (contmenu - 1) >= 0 ? (contmenu - 1) : 1;
      UpPressed = false;
    }//end UpPressed
  }//end While
}//end MenuGyro

// ============================================================================
// --- End of Program ---

/*==================================================================================================================================================================

Telemetry System - v.01 LoraConfig.h
MODULE: WiFi LoRa 32 (V2)
MCU: ESP32
CHIP: LoRa SX1278
Frequency: 863~928 Hz

Author: Joao Ricardo Chaves and Marcelo Haziel
Date: 2024, March.

====================================================================================================================================================================*/

#ifndef LoRa_ConfigRx_
#define LoRa_ConfigRx_

//==========================================================================
//--Bibliotecas--
#include <heltec.h>
#include <LiquidCrystal_I2C.h>

//==========================================================================
//--Variaveis Globais--

//==========================================================================
//--Variaveis Display e botões--
const String Menu[]       = {"LoRa","Temperatura","MPU6050","Altitude","Velocidade","Pressao"};
const String MenuMPU6050[]   = {"AngleRoll", "AnglePitch"};

volatile bool EnterPressed = false;                                                   // Variaveis do tipo Volatile pois são modificadas na interrupção
volatile bool DownPressed  = false;
volatile bool UpPressed    = false;
volatile bool ExitPressed  = false;

String MPUValues[2];

//==========================================================================
//--Variáveis dos dados do sensor mpu6050
String AnglePitch;
String AngleRoll;
String satelites;

//==========================================================================
//--Variáveis dos dados do sensor bmp180
String temp;
String pressao;

//==========================================================================
//--Variável que armazena o pacote LoRa
String Dados;

//==========================================================================
//--Variáveis do gps module neo 6m
String velocidade;
String altitude;
String altura;
String Lat;
String Lng;
String sat;

float altitudeRef = 679.5; // Constante da altitude de referencia para calculo de altura;

//==========================================================================
//--RSSI e SNR da comunicação LoRa
float SNR_dB;
int RSSI_dBm;

//==========================================================================
//pinos do chip LoRa SX1276 (comunicação spi)
#define LORA_SCK_PIN 5
#define LORA_MISO_PIN 19
#define LORA_MOSI_PIN 27
#define LORA_SS_PIN 18
#define LORA_RST_PIN 15
#define LORA_DI00_PIN 26

//==========================================================================
//--definição do rádio LoRa
#define DisplayEstado false           //define se o display estará ativo ou não ativo
#define LoRaEstado true              //
#define SerialEstado true            // Baud rate padrão de 1152000
#define AmplificadorDePotencia true  //define se o amplificador de potência PABOOST estará ativo ou não
#define BAND 915E6

//==========================================================================
//--LCD 16x2 i2C configuration--
#define endereco    0x27 // Endereços comuns: 0x27, 0x3F
#define colunas     16
#define linhas      2

LiquidCrystal_I2C lcd(endereco, colunas, linhas);

//==========================================================================
//--Button configuration--
#define BottonEnter 23
#define BottonUp    13
#define BottonDown  17
#define BottonExit  2

#define SDA_PIN     4
#define SCL_PIN     15

#endif
// ============================================================================
// --- End of Program ---
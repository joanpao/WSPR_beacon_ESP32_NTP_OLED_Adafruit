/***********************************************************************************
* Use serial console Output for debugging and development
*  
* Change if needed
************************************************************************************/

// Comment out if you do not want Serial console output
// Comment out DEVMODE in a production environment as it will degrade performance!
#define DEVMODE

// Your own HAM call. Change it
// Modifico por mi indicativo
#define MYCALL "EA5JTT"

// The power of your transmission in dBm. 
// for the si5351 this should be set to 10 (=10 milliwatts) 7dBm
// Es la potencia que entrega Si5351
#define DBMPOWER 7


// If you have a valid GPS lock, the software will make this HIGH twice for 20ms
// You could connect a LED to it.
// Set LED_PIN to 0 if you do not want to use a pin
// Pongo el 25 que es el que normalmente usamos 
#define LED_PIN 25       

// Frequency of the SI5351 Oscillator in Hertz
// for example #define SI5351FREQ 26000000 if you have a 26MHz XO
// Use 0 if you have a 25MHz Oscillator
// segun los datos de Aliexpress funciona a 25 MHz asíe s que pongo 0
#define SI5351FREQ 0


/***********************************************************************************
* Todos los chips  Si5351 preceisan calibrar la frecuencia y para ello hay que calcular el valor de ajuste:
* - mediante el progrma de calibraciónd e ejemplo
* - mediante un frecuencimetro
* - mediante un receptor 
*
* Se calibre con centesimas de Hz
* por algun motivo el ajuste de 2660 Hz para 18MHZ se convierte en 1160 ¿?
************************************************************************************/


char MaidenHead[7] = "IM99tl";

const char* ssid = "TP-Link_7254";
const char* password = "63341379";

/***********************************************************************************
* FRECUENCIAS WSPR 
* Frecuencia en  Hz con - correcccion si #define SI5351_CORRECTION 0
*            0ULL,         No transmite
* 60m  5286600ULL, no se usa pues no esta consensuada a nivel mundial por asignacioens dispares de esta banda
* 6m  50294500ULL, precisa filtro especifico
* LAs bandas son de 200 Hz por lo que se pone la frecuecnia central y el margen es de +/- 100 Hz
************************************************************************************/
/* INTERBATIONAL WSPR BEACON PROJECT https://github.com/HB9VQQ/WSPRBeacon */
// PARA PLACA CLK2
// OBSERVE COMO EL AJUSTE ES PROGRESIVO
// 
//  144898230ULL    // 2m           144489200ULL => 144080170 409030

static const unsigned long long freqArray[11] = {   
     1836480ULL,   // 160m 0,20,40   1836600ULL =>   1836610  120
     3569870ULL,   // 80m 2,22,42    3570100ULL =>   3570788  230 OK
     5288350ULL,   // 60m 4,24,44    5288700ULL =>   5289030  330 OK
     7039620ULL,   // 40m 6,26,46    7040100ULL =>   7040540  440 OK
    10139560ULL,   // 30m 8,28,48   10140200ULL =>  10139560  690 OK
    14096140ULL,   // 20m 10,30,50  14097100ULL =>  14098060  960 OK
    18104895ULL,   // 18m 12,32,52  18106100ULL =>  18104895 1205 OK
    21094650ULL,   // 15m 14,34,54  21096100ULL =>  21097550 1450 OK
    24924400ULL,   // 12m 16,36,56  24926100ULL =>  24927800 1700 OK
    28124150ULL,   // 10m 18,38,58  28126100ULL =>  28128050 1950 OK
    50291020ULL    // 6m            50294500ULL =>  50297140 3440
    };


/***********************************************************************************
* GPS SETTINGS
*  
* Change if needed
************************************************************************************/
//Para Lilygo  T-Beam V1.2 la velocidad es 9600 y los pines 12-TX 34-RX  
static const int Rx = 34, Tx = 12; 
static const uint32_t GPSBaud = 9600;  
                         
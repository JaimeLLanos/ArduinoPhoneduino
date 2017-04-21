#include <SPI.h>
#include <Wire.h>      
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Adafruit_STMPE610.h>
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);


// Para la pantalla TFT
#define TFT_DC 9
#define TFT_CS 10

// Aqui se carga la pantalla TFT en el arduino
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// Se crea una variable para la FONA y se carga en el arduino
#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// Estas variables definen las coordenadas minimas y maximas de la pantalla TFT
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

//******************* Detalles de la Interfaz de Usuario
#define BUTTON_X 40
#define BUTTON_Y 100
#define BUTTON_W 60
#define BUTTON_H 30
#define BUTTON_SPACING_X 20
#define BUTTON_SPACING_Y 20
#define BUTTON_TEXTSIZE 2

// Textfield vacio, donde escribimos los numeros
#define TEXT_X 10
#define TEXT_Y 10
#define TEXT_W 220
#define TEXT_H 50
#define TEXT_TSIZE 3
#define TEXT_TCOLOR ILI9341_MAGENTA

// los datos (telefono) que guardamos en el textfield
#define TEXT_LEN 12
char textfield[TEXT_LEN+1] = "";
uint8_t textfield_i=0;

// Linea de estado para la que FONA esta funcionando
#define STATUS_X 10
#define STATUS_Y 65

// Creacion de los 15 botones de la interfaz
char buttonlabels[15][5] = {"Send", "Clr", "End", "1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "0", "#" };
uint16_t buttoncolors[15] = {ILI9341_DARKGREEN, ILI9341_DARKGREY, ILI9341_RED, 
                             ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, 
                             ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, 
                             ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, 
                             ILI9341_ORANGE, ILI9341_BLUE, ILI9341_ORANGE};
Adafruit_GFX_Button buttons[15];

// Imprime algo en la barra de estado con flashstring
void status(const __FlashStringHelper *msg) {
  tft.fillRect(STATUS_X, STATUS_Y, 240, 8, ILI9341_BLACK);
  tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.print(msg);
}
// o charstring
void status(char *msg) {
  tft.fillRect(STATUS_X, STATUS_Y, 240, 8, ILI9341_BLACK);
  tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.print(msg);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Arduin-o-Phone!"); 
  
  // Limpiar la pantalla
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  
  // Si no se encuentra la pantalla
  if (!ts.begin()) {
    Serial.println("No se ha podido inciar el controlador de la pantalla tactil");
    while (1);
  }
  Serial.println("Pantalla tactil iniciada");

  // Crear botones
  for (uint8_t row=0; row<5; row++) {
    for (uint8_t col=0; col<3; col++) {
      buttons[col + row*3].initButton(&tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
                 BUTTON_Y+row*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
                  BUTTON_W, BUTTON_H, ILI9341_WHITE, buttoncolors[col+row*3], ILI9341_WHITE,
                  buttonlabels[col + row*3], BUTTON_TEXTSIZE); 
      buttons[col + row*3].drawButton();
    }
  }
  
  // Crear textfield
  tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, ILI9341_WHITE);
  
  status(F("Detectando la FONA..."));
  // Comprobar si esta la FONA
  fonaSS.begin(4800);

  // Comprobar si responde la FONA
  if (! fona.begin(fonaSS)) {
    status(F("No se ha podido encontrar la FONA :("));
    while (1);
  }
  status(F("FONA encontrada!"));
  
  // Comprobar que nos conectamos a la red
  while (fona.getNetworkStatus() == 1) {
    status(F("Buscando red..."));
    delay(100);
  }
  status(F("Conectado a la red!"));
 
  // Para establecer el audio externo del movil
  fona.setAudio(FONA_EXTAUDIO);
}


void loop(void) {
  TS_Point p;
  
  if (ts.bufferSize()) {
    p = ts.getPoint(); 
  } else {
    // De esta forma se rastrea el tacto en la pantalla
    p.x = p.y = p.z = -1;
  }
  
  // Calibrar la interfaz al tamaño de la pantalla
  if (p.z != -1) {
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    Serial.print("("); Serial.print(p.x); Serial.print(", "); 
    Serial.print(p.y); Serial.print(", "); 
    Serial.print(p.z); Serial.println(") ");
  }
  
  // Se comprueba boton a boton si este ha sido presionado
  for (uint8_t b=0; b<15; b++) {
    if (buttons[b].contains(p.x, p.y)) {
      buttons[b].press(true); \\ Te dice si ha presionado el boton
    } else {
      buttons[b].press(false); \\ En caso de no presionar el boton
    }
  }

  // Ahora preguntamos a los botones si ha cambiado su estado
  for (uint8_t b=0; b<15; b++) {
    if (buttons[b].justReleased()) {
      buttons[b].drawButton();
    }
    
    if (buttons[b].justPressed()) {
        buttons[b].drawButton(true);
        
        // Si se añade un boton de almohadilla numerica, mostrar # en pantalla
        if (b >= 3) {
          if (textfield_i < TEXT_LEN) {
            textfield[textfield_i] = buttonlabels[b][0];
            textfield_i++;
	    textfield[textfield_i] = 0;
            
            fona.playDTMF(buttonlabels[b][0]);
          }
        }

        // Boton clr, para borrar digitos del textfield
        if (b == 1) {
          
          textfield[textfield_i] = 0;
          if (textfield > 0) {
            textfield_i--;
            textfield[textfield_i] = ' ';
          }
        }

        // Actualizar el textfield actual
        Serial.println(textfield);
        tft.setCursor(TEXT_X + 2, TEXT_Y+10);
        tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
        tft.setTextSize(TEXT_TSIZE);
        tft.print(textfield);

        // Para colgar
        if (b == 2) {
          status(F("Hanging up"));
          fona.hangUp();
        }
        
        // Para llamar
        if (b == 0) {
          status(F("Calling"));
          Serial.print("Calling "); Serial.print(textfield);
          
          fona.callPhone(textfield);
        }
        
      delay(100);
    }
  }
}

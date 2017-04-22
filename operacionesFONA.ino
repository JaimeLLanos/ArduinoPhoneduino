#include "Adafruit_FONA.h"

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4


char replybuffer[255];


#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;



Adafruit_FONA fona = Adafruit_FONA(FONA_RST);


uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

uint8_t type;

void setup() {
  while (!Serial);

  Serial.begin(115200);
  Serial.println(F("Otras Operaciones de la FONA"));
  Serial.println(F("Iniciando...."));

  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("No se ha podido detectar la FONA"));
    while (1);
  }
  type = fona.type();
  Serial.println(F("FONA OK"));
  Serial.print(F("Encontrada!! "));
  switch (type) {
    case FONA800H:
      Serial.println(F("FONA 800H")); break;
    default: 
      Serial.println(F("?")); break;
  }
  
  //Imprime el modulo IMEI
  char imei[16] = {0}; 
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("Modulo IMEI: "); 
    Serial.println(imei);
  }



  printMenu();
}

void printMenu(void) {
  Serial.println(F("--------------------------------------------"));
  Serial.println(F("[?] Imprime este menu"));
  Serial.println(F("[a] Lectura de ADC 2.8V"));
  Serial.println(F("[b] Leer los V de la bateria y el porcentaje de carga"));
  Serial.println(F("[C] Lectura del Indicador de fuerza de la señal recibida"));
  Serial.println(F("[U] En el caso de que la SIM tenga pin, seleccione esta opcion para desbloquear"));
  Serial.println(F("[n] Conectarse a una red"));
  Serial.println(F("[v] Establecer el volumen del audio"));
  Serial.println(F("[V] Obtener Volumen de Audio"));
  Serial.println(F("[H] Establecer audio para la salida jack(Tipo auriculares o altavoz externo)"));
  Serial.println(F("[e] Establecer audio externo(Activa altavoz soldado"));
  Serial.println(F("[T] Reproduce los tonos del audio"));
  //Serial.println(F("[P] PWM/Buzzer out (FONA800 & 808)"));
  
  Serial.println(F("[f] Sintoniza Radio FM"));
  Serial.println(F("[F] Apaga radio"));
  Serial.println(F("[m] Establecer Audio Radio FM"));
  Serial.println(F("[M] Obtener Volumen Radio FM"));
  Serial.println(F("[q] Obtener Potencia de la señal de la banda sintonizada"));

  
  Serial.println(F("[c] Hacer Llamada"));
  Serial.println(F("[A] Obetener estatus de la llamada"));
  Serial.println(F("[h] Colgar"));
  Serial.println(F("[p] Coger llamada"));


  Serial.println(F("[N] Numero de SMS"));
  Serial.println(F("[r] Leer SMS #"));
  Serial.println(F("[R] Leer todos los SMS"));
  Serial.println(F("[d] Borrar SMS #"));
  Serial.println(F("[s] Mandar SMS"));
  Serial.println(F("[u] Mandar SMS(Tipo Multimedia)"));

  Serial.println(F("[G] Activar GPRS"));
  Serial.println(F("[g] Descativar GPRS"));
  Serial.println(F("[w] Posibilidad de leer pagina web(No implementadol)"));
  Serial.println(F("---------------------------------------"));
  Serial.println(F(""));
  


}
void loop() {
  Serial.print(F("FONA> "));
  while (! Serial.available() ) {
    if (fona.available()) {
      Serial.write(fona.read());
    }
  }

  char comando = Serial.read();
  Serial.println(comando);


  switch (comando) {
    case '?': {
        printMenu();
        break;
      }

    case 'a': {
        uint16_t adc;
        if (! fona.getADCVoltage(&adc)) {
          Serial.println(F("Fallo al leer ADC"));
        } else {
          Serial.print(F("ADC = "));
          Serial.print(adc);
          Serial.println(F(" mV"));
        }
        break;
      }

    case 'b': {
        uint16_t vbat;
        if (! fona.getBattVoltage(&vbat)) {
          Serial.println(F("Fallo al leer la bateria"));
        } else {
          Serial.print(F("VBat = "));
          Serial.print(vbat);
          Serial.println(F(" mV"));
        }


        if (! fona.getBattPercent(&vbat)) {
          Serial.println(F("Fallo al leer la bateria"));
        } else {
          Serial.print(F("Porcentaje = "));
          Serial.print(vbat);
          Serial.println(F("%"));
        }

        break;
      }

    case 'U': {
        char PIN[5];
        flushSerial();
        Serial.println(F("Introduce un codigo(PIN) de 4 digitos"));
        readline(PIN, 3);
        Serial.println(PIN);
        Serial.print(F("Desbloqueando tarjeta SIM...: "));
        if (! fona.unlockSIM(PIN)) {
          Serial.println(F("Fallo"));
        } else {
          Serial.println(F("Desbloqueada!"));
        }
        break;
      }

      case 'C': {
        fona.getSIMCCID(replybuffer);  
        Serial.print(F("CCID SIM = ")); 
        Serial.println(replybuffer);
        break;
      }

    case 'i': {
        // read the RSSI
        uint8_t n = fona.getRSSI();
        int8_t r;

        Serial.print(F("RSSI = ")); 
        Serial.print(n); 
        Serial.print(": ");
        if (n == 0) r = -115;
        if (n == 1) r = -111;
        if (n == 31) r = -52;
        if ((n >= 2) && (n <= 30)) {
          r = map(n, 2, 30, -110, -54);
        }
        Serial.print(r);
        Serial.println(F(" dBm"));

        break;
      }

    case 'n': {
        uint8_t n = fona.getNetworkStatus();
        Serial.print(F("Estado de la Red "));
        Serial.print(n);
        Serial.print(F(": "));
        if (n == 0) Serial.println(F("No registrado"));
        if (n == 1) Serial.println(F("Registrado"));
        if (n == 2) Serial.println(F("No Registrado (buscando...)"));
        if (n == 3) Serial.println(F("Denegado"));
        if (n == 4) Serial.println(F("Desconocido"));
        if (n == 5) Serial.println(F("Registrado con roaming"));
        break;
      }

 
    case 'v': {
        flushSerial();
        if ( (type == FONA3G_A) || (type == FONA3G_E) ) {
          Serial.print(F("Set Vol [0-8] "));
        } else {
          Serial.print(F("Set Vol % [0-100] "));
        }
        uint8_t vol = readnumber();
        Serial.println();
        if (! fona.setVolume(vol)) {
          Serial.println(F("Fallo"));
        } else {
          Serial.println(F("Establecido!"));
        }
        break;
      }

    case 'V': {
        uint8_t v = fona.getVolume();
        Serial.print(v);
        if ( (type == FONA3G_A) || (type == FONA3G_E) ) {
          Serial.println(" / 8");
        } else {
          Serial.println("%");
        }
        break;
      }

    case 'H': {
        if (! fona.setAudio(FONA_HEADSETAUDIO)) {
          Serial.println(F("Fallo"));
        } else {
          Serial.println(F("Establecido!"));
        }
        fona.setMicVolume(FONA_HEADSETAUDIO, 15);
        break;
      }
    case 'e': {
        if (! fona.setAudio(FONA_EXTAUDIO)) {
          Serial.println(F("Fallo"));
        } else {
          Serial.println(F("Establecido!"));
        }

        fona.setMicVolume(FONA_EXTAUDIO, 10);
        break;
      }

    case 'T': {
        flushSerial();
        Serial.print(F("Reproducir Tono #"));
        uint8_t kittone = readnumber();
        Serial.println();
        if (! fona.playToolkitTone(kittone, 1000)) {
          Serial.println(F("Fallo"));
        } else {
          Serial.println(F("Establecido!"));
        }
        break;
      }

    case 'f': {
        flushSerial();
        Serial.print(F("FM Freq (ej 89.9 FM: introduzca ->899): "));
        uint16_t station = readnumber();
        Serial.println();
        if (fona.FMradio(true, FONA_HEADSETAUDIO)) {
          Serial.println(F("Abierta"));
        }
        if (! fona.tuneFMradio(station)) {
          Serial.println(F("Fallo"));
        } else {
          Serial.println(F("Sintonizada"));
        }
        break;
      }
      
    case 'F': {
        if (! fona.FMradio(false)) {
          Serial.println(F("Fallo"));
        } else {
          Serial.println(F("Apagada!"));
        }
        break;
      }
    case 'm': {
        flushSerial();
        Serial.print(F("Set FM Vol [0-6]:"));
        uint8_t vol = readnumber();
        Serial.println();
        if (!fona.setFMVolume(vol)) {
          Serial.println(F("Fallo"));
        } else {
          Serial.println(F("Establecido!"));
        }
        break;
      }
    case 'M': {
        uint8_t fmvol = fona.getFMVolume();
        if (fmvol < 0) {
          Serial.println(F("Fallo"));
        } else {
          Serial.print(F("Volumen FM: "));
          Serial.println(fmvol, DEC);
        }
        break;
      }
    case 'q': {
        flushSerial();
        Serial.print(F("FM Freq (ej 89.9 FM: introduzca ==899): "));
        uint16_t station = readnumber();
        Serial.println();
        int8_t level = fona.getFMSignalLevel(station);
        if (level < 0) {
          Serial.println(F("Fallo! asegurese de que la emisora esta sintonizada"));
        } else {
          Serial.print(F("Nivel de la señal (dB): "));
          Serial.println(level, DEC);
        }
        break;
      }
    case 'c': {
        char number[30];
        flushSerial();
        Serial.print(F("Llamar #"));
        readline(number, 30);
        Serial.println();
        Serial.print(F("Llamando... ")); 
        Serial.println(number);
        if (!fona.callPhone(number)) {
          Serial.println(F("Fallo"));
        } else {
          Serial.println(F("Llamada Enviada!"));
        }
        break;
      }
    case 'A': {
        int8_t callstat = fona.getCallStatus();
        switch (callstat) {
          case 0: Serial.println(F("Listo")); break;
          case 1: Serial.println(F("No fue posible obtener un estado")); break;
          case 3: Serial.println(F("Llamada (entrando)")); break;
          case 4: Serial.println(F("Timbre/en curse(saliente)")); break;
          default: Serial.println(F("Estado Desconocido")); break;
        }
        break;
      }
      
    case 'h': {
        if (! fona.hangUp()) {
          Serial.println(F("Fallo"));
        } else {
          Serial.println(F("Colgada/Finalizada!"));
        }
        break;
      }

    case 'p': {
        if (! fona.pickUp()) {
          Serial.println(F("Fallo"));
        } else {
          Serial.println(F("Llamada cogida!"));
        }
        break;
      }

    case 'N': {
        int8_t smsnum = fona.getNumSMS();
        if (smsnum < 0) {
          Serial.println(F("No se puedo leer # SMS"));
        } else {
          Serial.print(smsnum);
          Serial.println(F(" SMS's la memoria SIM!"));
        }
        break;
      }
    case 'r': {
        flushSerial();
        Serial.print(F("Leer #"));
        uint8_t smsn = readnumber();
        Serial.print(F("\n\rLeyendo SMS #"));
        Serial.println(smsn);

        if (! fona.getSMSSender(smsn, replybuffer, 250)) {
          Serial.println("Fallo!");
          break;
        }
        Serial.print(F("De: "));
        Serial.println(replybuffer);

        uint16_t smslen;
        if (! fona.readSMS(smsn, replybuffer, 250, &smslen)) { 
          Serial.println("Fallo!");
          break;
        }
        Serial.print(F("***** SMS #")); 
        Serial.print(smsn);
        Serial.print(" (");
        Serial.print(smslen);
        Serial.println(F(") bytes *****"));
        Serial.println(replybuffer);
        Serial.println(F("*****"));

        break;
      }
    case 'R': {
        int8_t smsnum = fona.getNumSMS();
        uint16_t smslen;
        int8_t smsn;

        if ( (type == FONA3G_A) || (type == FONA3G_E) ) {
          smsn = 0; 
          smsnum--;
        } else {
          smsn = 1; 
        }

        for ( ; smsn <= smsnum; smsn++) {
          Serial.print(F("\n\rLeeyendo SMS #"));
          Serial.println(smsn);
          if (!fona.readSMS(smsn, replybuffer, 250, &smslen)) { 
            Serial.println(F("Fallo!"));
            break;
          }
          //Si la longitud es cero,es un caso especial donde el numero del inidice el mayor
          //asique incrementar el maximo que estabamos buscando
          if (smslen == 0) {
            Serial.println(F("[ranura vacia]"));
            smsnum++;
            continue;
          }

          Serial.print(F("***** SMS #"));
          Serial.print(smsn);
          Serial.print(" (");
          Serial.print(smslen); 
          Serial.println(F(") bytes *****"));
          Serial.println(replybuffer);
          Serial.println(F("*****"));
        }
        break;
      }

    case 'd': {
        flushSerial();
        Serial.print(F("Borrar #"));
        uint8_t smsn = readnumber();

        Serial.print(F("\n\rBorrando SMS #")); 
        Serial.println(smsn);
        if (fona.deleteSMS(smsn)) {
          Serial.println(F("Borrado!"));
        } else {
          Serial.println(F("No se pudo borrar"));
        }
        break;
      }

    case 's': {
        char sendto[21], message[141];
        flushSerial();
        Serial.print(F("Mandar a #"));
        readline(sendto, 20);
        Serial.println(sendto);
        Serial.print(F("Escribe en una sola linea (200 caracteres max): "));
        readline(message, 200);
        Serial.println(message);
        if (!fona.sendSMS(sendto, message)) {
          Serial.println(F("Fallo"));
        } else {
          Serial.println(F("Enviado!"));
        }

        break;
      }

    case 'u': {
      char message[141];
      flushSerial();
      Serial.print(F("Escribe en una sola linea (200 caracteres max): "));
      readline(message, 200);
      Serial.println(message);
      uint16_t ussdlen;
      if (!fona.sendUSSD(message, replybuffer, 250, &ussdlen)) { 
        Serial.println(F("Fallo"));
      } else {
        Serial.println(F("Enviado!"));
        Serial.print(F("***** USSD Respuesta"));
        Serial.print(" ("); 
        Serial.print(ussdlen); 
        Serial.println(F(") bytes *****"));
        Serial.println(replybuffer);
        Serial.println(F("*****"));
      }
    }

    case 'g': {
        if (!fona.enableGPRS(false))
          Serial.println(F("Fallo al apagar"));
        break;
      }
    case 'G': {
        if (!fona.enableGPRS(true))
          Serial.println(F("Fallo al encender"));
        break;
      }

   
    case 'w': {
      Serial.println(F("No implementado todavia"));
    }
      /*
        uint16_t statuscode;
        int16_t length;
        char url[80];

        flushSerial();
        Serial.println(F("NOTE: En desarrollo! Utiliza pequeñas paginas web para leer!"));
        Serial.println(F("URL to read (e.g. www.elmundotoday.com):"));
        Serial.print(F("http://")); 
        readline(url, 79);
        Serial.println(url);

        Serial.println(F("****"));
        if (!fona.HTTP_GET_start(url, &statuscode, (uint16_t *)&length)) {
          Serial.println("Fallo!");
          break;
        }
        while (length > 0) {
          while (fona.available()) {
            char c = fona.read();

           
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
            loop_until_bit_is_set(UCSR0A, UDRE0); 
            UDR0 = c;
#else
            Serial.write(c);
#endif
            length--;
            if (! length) break;
          }
        }
        Serial.println(F("\n****"));
        fona.HTTP_GET_end();
        break;
      }
      */

    default: {
        Serial.println(F("Comando desconocido"));
        printMenu();
        break;
      }
  }

  flushSerial();
  while (fona.available()) {
    Serial.write(fona.read());
  }

}

void flushSerial() {
  while (Serial.available())
    Serial.read();
}

char readBlocking() {
  while (!Serial.available());
  return Serial.read();
}
uint16_t readnumber() {
  uint16_t x = 0;
  char c;
  while (! isdigit(c = readBlocking())) {
    //Serial.print(c);

  }
  Serial.print(c);
  x = c - '0';
  while (isdigit(c = readBlocking())) {
    Serial.print(c);
    x *= 10;
    x += c - '0';
  }
  return x;
}

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout) {
  uint16_t buffidx = 0;
  boolean timeoutvalid = true;
  if (timeout == 0) timeoutvalid = false;

  while (true) {
    if (buffidx > maxbuff) {
      //Serial.println(F("SPACE"));
      break;
    }

    while (Serial.available()) {
      char c =  Serial.read();

        //Serial.print(c, HEX); Serial.print("#"); Serial.println(c);

      if (c == '\r') continue;
      if (c == 0xA) {
        if (buffidx == 0)   // the first 0x0A is ignored
          continue;

        timeout = 0;     // the second 0x0A is the end of the line
        timeoutvalid = true;
        break;
      }
      buff[buffidx] = c;
      buffidx++;
    }

    if (timeoutvalid && timeout == 0) {
       //Serial.println(F("TIMEOUT"));
      break;
    }
    delay(1);
  }
  buff[buffidx] = 0; //null term
  return buffidx;
}

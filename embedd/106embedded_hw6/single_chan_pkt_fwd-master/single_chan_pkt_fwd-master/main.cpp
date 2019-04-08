/*******************************************************************************
 *
 * Copyright (c) 2015 Thomas Telkamp
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *******************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <cstdlib>
#include <sys/time.h>
#include <cstring>
#include <sys/ioctl.h>
#include <net/if.h>
#include "base64.h"
#include <wiringPi.h>
#include <wiringPiSPI.h>
/*
Include 912loraaes
*/
#include <iostream>
using namespace std;
#include <fstream>
#include <cryptopp/aes.h>
using CryptoPP::AES;
#include <cryptopp/modes.h>
using CryptoPP::ECB_Mode;
#include <cryptopp/filters.h>
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;
#include <string>
using std::string;
#include "cryptopp/hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;
#include <cryptopp/algparam.h>
#include <cstdlib>
using std::exit;

#include <cryptopp/base64.h>
using CryptoPP::Base64Decoder;
using CryptoPP::Base64URLDecoder;
using CryptoPP::Name::Pad;
using CryptoPP::Name::InsertLineBreaks;
#include "cryptopp/cryptlib.h"
using CryptoPP::Exception;
#include <stdint.h>
// typedef int64_t word64;
/*end*/

typedef bool boolean;
typedef unsigned char byte;

static const int CHANNEL = 0;

byte currentMode = 0x81;

char message[256];
char b64[256];

bool sx1272 = true;

byte receivedbytes;

struct sockaddr_in si_other;
int s, slen=sizeof(si_other);
struct ifreq ifr;

uint32_t cp_nb_rx_rcv;
uint32_t cp_nb_rx_ok;
uint32_t cp_nb_rx_bad;
uint32_t cp_nb_rx_nocrc;
uint32_t cp_up_pkt_fwd;

enum sf_t { SF7=7, SF8, SF9, SF10, SF11, SF12 };

/*******************************************************************************
 *
 * Configure these values!
 *
 *******************************************************************************/

// SX1272 - Raspberry connections
int ssPin = 6;
int dio0  = 7;
int RST   = 0;

// Set spreading factor (SF7 - SF12)
sf_t sf = SF7;

// Set center frequency
uint32_t  freq = 868100000; // in Mhz! (868.1)

// Set location
float lat=0.0;
float lon=0.0;
int   alt=0;

/* Informal status fields */
static char platform[24]    = "Single Channel Gateway";  /* platform definition */
static char email[40]       = "";                        /* used for contact email */
static char description[64] = "";                        /* used for free form description */

// define servers
// TODO: use host names and dns
#define SERVER1 "54.72.145.119"    // The Things Network: croft.thethings.girovito.nl
//#define SERVER2 "192.168.1.10"      // local
#define PORT 1700                   // The port on which to send data

// #############################################
// #############################################

#define REG_FIFO                    0x00
#define REG_FIFO_ADDR_PTR           0x0D
#define REG_FIFO_TX_BASE_AD         0x0E
#define REG_FIFO_RX_BASE_AD         0x0F
#define REG_RX_NB_BYTES             0x13
#define REG_OPMODE                  0x01
#define REG_FIFO_RX_CURRENT_ADDR    0x10
#define REG_IRQ_FLAGS               0x12
#define REG_DIO_MAPPING_1           0x40
#define REG_DIO_MAPPING_2           0x41
#define REG_MODEM_CONFIG            0x1D
#define REG_MODEM_CONFIG2           0x1E
#define REG_MODEM_CONFIG3           0x26
#define REG_SYMB_TIMEOUT_LSB  		0x1F
#define REG_PKT_SNR_VALUE			0x19
#define REG_PAYLOAD_LENGTH          0x22
#define REG_IRQ_FLAGS_MASK          0x11
#define REG_MAX_PAYLOAD_LENGTH 		0x23
#define REG_HOP_PERIOD              0x24
#define REG_SYNC_WORD				0x39
#define REG_VERSION	  				0x42

#define SX72_MODE_RX_CONTINUOS      0x85
#define SX72_MODE_TX                0x83
#define SX72_MODE_SLEEP             0x80
#define SX72_MODE_STANDBY           0x81

#define PAYLOAD_LENGTH              0x40

// LOW NOISE AMPLIFIER
#define REG_LNA                     0x0C
#define LNA_MAX_GAIN                0x23
#define LNA_OFF_GAIN                0x00
#define LNA_LOW_GAIN		    	0x20

// CONF REG
#define REG1                        0x0A
#define REG2                        0x84

#define SX72_MC2_FSK                0x00
#define SX72_MC2_SF7                0x70
#define SX72_MC2_SF8                0x80
#define SX72_MC2_SF9                0x90
#define SX72_MC2_SF10               0xA0
#define SX72_MC2_SF11               0xB0
#define SX72_MC2_SF12               0xC0

#define SX72_MC1_LOW_DATA_RATE_OPTIMIZE  0x01 // mandated for SF11 and SF12

// FRF
#define        REG_FRF_MSB              0x06
#define        REG_FRF_MID              0x07
#define        REG_FRF_LSB              0x08

#define        FRF_MSB                  0xD9 // 868.1 Mhz
#define        FRF_MID                  0x06
#define        FRF_LSB                  0x66

#define BUFLEN 2048  //Max length of buffer

#define PROTOCOL_VERSION  1
#define PKT_PUSH_DATA 0
#define PKT_PUSH_ACK  1
#define PKT_PULL_DATA 2
#define PKT_PULL_RESP 3
#define PKT_PULL_ACK  4

#define TX_BUFF_SIZE  2048
#define STATUS_SIZE	  1024

void die(const char *s)
{
    perror(s);
    exit(1);
}

void selectreceiver()
{
    digitalWrite(ssPin, LOW);
}

void unselectreceiver()
{
    digitalWrite(ssPin, HIGH);
}

byte readRegister(byte addr)
{
    unsigned char spibuf[2];

    selectreceiver();
    spibuf[0] = addr & 0x7F;
    spibuf[1] = 0x00;
    wiringPiSPIDataRW(CHANNEL, spibuf, 2);
    unselectreceiver();

    return spibuf[1];
}

void writeRegister(byte addr, byte value)
{
    unsigned char spibuf[2];

    spibuf[0] = addr | 0x80;
    spibuf[1] = value;
    selectreceiver();
    wiringPiSPIDataRW(CHANNEL, spibuf, 2);

    unselectreceiver();
}


boolean receivePkt(char *payload)
{

    // clear rxDone
    writeRegister(REG_IRQ_FLAGS, 0x40);

    int irqflags = readRegister(REG_IRQ_FLAGS);

    cp_nb_rx_rcv++;

    //  payload crc: 0x20
    if((irqflags & 0x20) == 0x20)
    {
        printf("CRC error\n");
        writeRegister(REG_IRQ_FLAGS, 0x20);
        return false;
    } else {

        cp_nb_rx_ok++;

        byte currentAddr = readRegister(REG_FIFO_RX_CURRENT_ADDR);
        byte receivedCount = readRegister(REG_RX_NB_BYTES);
        receivedbytes = receivedCount;

        writeRegister(REG_FIFO_ADDR_PTR, currentAddr);

        for(int i = 0; i < receivedCount; i++)
        {
            payload[i] = (char)readRegister(REG_FIFO);
        }
    }
    return true;
}

void SetupLoRa()
{

    digitalWrite(RST, HIGH);
    delay(100);
    digitalWrite(RST, LOW);
    delay(100);

    byte version = readRegister(REG_VERSION);

    if (version == 0x22) {
        // sx1272
        printf("SX1272 detected, starting.\n");
        sx1272 = true;
    } else {
        // sx1276?
        digitalWrite(RST, LOW);
        delay(100);
        digitalWrite(RST, HIGH);
        delay(100);
        version = readRegister(REG_VERSION);
        if (version == 0x12) {
            // sx1276
            printf("SX1276 detected, starting.\n");
            sx1272 = false;
        } else {
            printf("Unrecognized transceiver.\n");
            //printf("Version: 0x%x\n",version);
            exit(1);
        }
    }

    writeRegister(REG_OPMODE, SX72_MODE_SLEEP);

    // set frequency
    uint64_t frf = ((uint64_t)freq << 19) / 32000000;
    writeRegister(REG_FRF_MSB, (uint8_t)(frf>>16) );
    writeRegister(REG_FRF_MID, (uint8_t)(frf>> 8) );
    writeRegister(REG_FRF_LSB, (uint8_t)(frf>> 0) );

    writeRegister(REG_SYNC_WORD, 0x34); // LoRaWAN public sync word

    if (sx1272) {
        if (sf == SF11 || sf == SF12) {
            writeRegister(REG_MODEM_CONFIG,0x0B);
        } else {
            writeRegister(REG_MODEM_CONFIG,0x0A);
        }
        writeRegister(REG_MODEM_CONFIG2,(sf<<4) | 0x04);
    } else {
        if (sf == SF11 || sf == SF12) {
            writeRegister(REG_MODEM_CONFIG3,0x0C);
        } else {
            writeRegister(REG_MODEM_CONFIG3,0x04);
        }
        writeRegister(REG_MODEM_CONFIG,0x72);
        writeRegister(REG_MODEM_CONFIG2,(sf<<4) | 0x04);
    }

    if (sf == SF10 || sf == SF11 || sf == SF12) {
        writeRegister(REG_SYMB_TIMEOUT_LSB,0x05);
    } else {
        writeRegister(REG_SYMB_TIMEOUT_LSB,0x08);
    }
    writeRegister(REG_MAX_PAYLOAD_LENGTH,0x80);
    writeRegister(REG_PAYLOAD_LENGTH,PAYLOAD_LENGTH);
    writeRegister(REG_HOP_PERIOD,0xFF);
    writeRegister(REG_FIFO_ADDR_PTR, readRegister(REG_FIFO_RX_BASE_AD));

    // Set Continous Receive Mode
    writeRegister(REG_LNA, LNA_MAX_GAIN);  // max lna gain
    writeRegister(REG_OPMODE, SX72_MODE_RX_CONTINUOS);

}

void sendudp(char *msg, int length) {

//send the update
#ifdef SERVER1
    inet_aton(SERVER1 , &si_other.sin_addr);
    if (sendto(s, (char *)msg, length, 0 , (struct sockaddr *) &si_other, slen)==-1)
    {
        die("sendto()");
    }
#endif

#ifdef SERVER2
    inet_aton(SERVER2 , &si_other.sin_addr);
    if (sendto(s, (char *)msg, length , 0 , (struct sockaddr *) &si_other, slen)==-1)
    {
        die("sendto()");
    }
#endif
}

void sendstat() {

    static char status_report[STATUS_SIZE]; /* status report as a JSON object */
    char stat_timestamp[24];
    time_t t;

    int stat_index=0;

    /* pre-fill the data buffer with fixed fields */
    status_report[0] = PROTOCOL_VERSION;
    status_report[3] = PKT_PUSH_DATA;

    status_report[4] = (unsigned char)ifr.ifr_hwaddr.sa_data[0];
    status_report[5] = (unsigned char)ifr.ifr_hwaddr.sa_data[1];
    status_report[6] = (unsigned char)ifr.ifr_hwaddr.sa_data[2];
    status_report[7] = 0xFF;
    status_report[8] = 0xFF;
    status_report[9] = (unsigned char)ifr.ifr_hwaddr.sa_data[3];
    status_report[10] = (unsigned char)ifr.ifr_hwaddr.sa_data[4];
    status_report[11] = (unsigned char)ifr.ifr_hwaddr.sa_data[5];

    /* start composing datagram with the header */
    uint8_t token_h = (uint8_t)rand(); /* random token */
    uint8_t token_l = (uint8_t)rand(); /* random token */
    status_report[1] = token_h;
    status_report[2] = token_l;
    stat_index = 12; /* 12-byte header */

    /* get timestamp for statistics */
    t = time(NULL);
    strftime(stat_timestamp, sizeof stat_timestamp, "%F %T %Z", gmtime(&t));

    int j = snprintf((char *)(status_report + stat_index), STATUS_SIZE-stat_index, "{\"stat\":{\"time\":\"%s\",\"lati\":%.5f,\"long\":%.5f,\"alti\":%i,\"rxnb\":%u,\"rxok\":%u,\"rxfw\":%u,\"ackr\":%.1f,\"dwnb\":%u,\"txnb\":%u,\"pfrm\":\"%s\",\"mail\":\"%s\",\"desc\":\"%s\"}}", stat_timestamp, lat, lon, (int)alt, cp_nb_rx_rcv, cp_nb_rx_ok, cp_up_pkt_fwd, (float)0, 0, 0,platform,email,description);
    stat_index += j;
    status_report[stat_index] = 0; /* add string terminator, for safety */

    printf("stat update: %s\n", (char *)(status_report+12)); /* DEBUG: display JSON stat */

    //send the update
    // sendudp(status_report, stat_index);

}
string m_parser(string data,int first,int end)
{
  string str = "";

  for(int i = first;i < end;i++)
  {
    str+=data[i];

  }

  return str;
}
string flip(string data,int first,int end)
{
  string str = "";

  for(int i = first;i >end;i--)
  {
    str+=data[i-1];

  }

  return str;
}

void printhexdata(string name,string data){
   printf("\n");
  for(int i =0 ; i < name.size();i++)
  {
    printf("%c",name[i]);
  }
  printf("\n");
  for(int i =0 ; i < data.size();i++)
  {
    printf("%02x",data[i]);
  }
}

string getdecode(string input){
  string decoded;
  StringSource(input, true,
    new Base64Decoder(
        new StringSink(decoded))); // StringSource
  // printhexdata("decoded",decoded);
  int phypayloadlen = decoded.size();
  // printf("\nsize: %d\n",phypayloadlen);
  string MHDR  = m_parser(decoded,0,1);
  // printhexdata("MHDR",MHDR);
  string MACPayload  = m_parser(decoded,1,phypayloadlen-4);
  // printhexdata("MACPayload",MACPayload);
  string MIC  = m_parser(decoded,phypayloadlen-4,phypayloadlen);
  // printhexdata("MIC",MIC);
  string FCtrl = m_parser(MACPayload,4,5);
  // printhexdata("FCtrl",FCtrl);
  char char_array[FCtrl.length()];
  strcpy(char_array, FCtrl.c_str());
  // printf("\n");
  // printf("%02x",char_array[0]);
  char_array[0] = char_array[0] & 0x0f;
  // printf("\n%02x",char_array[0]);
  int FOptsLen = char_array[0];
  // printf("\FOptsLen: %d",FOptsLen);
  int FHDRlength = 7 + FOptsLen;
  string FHDR = m_parser(MACPayload,0,FHDRlength);
  // printhexdata("FHDR",FHDR);
  string DevAddr = flip(FHDR,4,0);
  printhexdata("DevAddr",DevAddr);
  printf("\n");
  string FCntarray = flip(FHDR,7,5);
  char* charFCnt_array = new char[FCntarray.length()+1];
  memcpy(charFCnt_array, FCntarray.c_str(), FCntarray.length()+1);
  int fcnt = 0;
  fcnt+= (charFCnt_array[0] * 256);
  fcnt+= charFCnt_array[1];
  string FPort = m_parser(MACPayload,FHDRlength,FHDRlength+1);
  char* FPort_array = new char[FPort.length()+1];
  memcpy(FPort_array, FPort.c_str(), FPort.length()+1);
  int FPortfcnt = FPort_array[0];
  // printhexdata("FPort",FPort);
  string FRMPayload = m_parser(MACPayload,FHDRlength+1,MACPayload.size());
  // printhexdata("FRMPayload",FRMPayload);
  string plainText;
  string cipherText;
  string cipher, encoded, recovered;
  int sequence_counter = fcnt;
  byte newkey[] = { 0x11,0xB5,0x16,0xC4,0x6D,0x2A,0x78,0x33,0x02,0x38,0x46,0xD4,0x59,0xC1,0xEC,0xA1};
  byte appkey[] = { 0x87,0x30,0xE5,0x28,0xAD,0x05,0x54,0x2E,0x44,0x6D,0xFF,0x4E,0xA3,0x4A,0x07,0xBF};
  if(FPortfcnt==0)
  {
    for(int i =0 ;i < 16;i++)
    {
      appkey[i] = newkey[i];
    }
  }
  byte abblock [] =
    {     0x01,                             // 0 always 0x01
          0x00,                             // 1 always 0x00
          0x00,                             // 2 always 0x00
          0x00,                             // 3 always 0x00
          0x00,                             // 4 always 0x00
          0,                        // 5 dir, 0 for uplink, 1 for downlink
          DevAddr[3],                      // 6 devaddr, lsb
          DevAddr[2],                      // 7 devaddr
          DevAddr[1],                      // 8 devaddr
          DevAddr[0],                      // 9 devaddr, msb
          sequence_counter & 0xff,          // 10 sequence counter (FCntUp) lsb
          (sequence_counter >> 8) & 0xff,   // 11 sequence counter
          (sequence_counter >> 16) & 0xff,  // 12 sequence counter
          (sequence_counter >> 24) & 0xff,  // 13 sequence counter (FCntUp) msb
          0x00,                             // 14 always 0x01
          0x00 } ;
  int bufferIndex = 0;
  int datapayloadsize = FRMPayload.size();
  int ctr = 1;
  string moutput = "";
  while(datapayloadsize>=16)
  {
    abblock[15] = ctr & 0xFF;
    ctr+=1;
    std::string s1(reinterpret_cast<char*>(abblock), sizeof(abblock));  //api need string parameter
    try
  	{
  		ECB_Mode< AES >::Encryption e;
  		e.SetKey(appkey, sizeof(appkey));
  		StringSource(s1, true,
  			new StreamTransformationFilter(e,
  				new StringSink(cipher)
  			) // StreamTransformationFilter
  		); // StringSource
  	}
  	catch(const CryptoPP::Exception& e)
  	{
  		exit(1);
  	}
    for(int i = 0; i < 16;i++)
    {
      moutput += FRMPayload[bufferIndex + i] ^ cipher[i];
    }
   bufferIndex +=16;
   datapayloadsize -=16;
  }
  if(datapayloadsize > 0)
  {
    abblock[15] = ctr & 0xFF;
    std::string s1(reinterpret_cast<char*>(abblock), sizeof(abblock));  //api need string parameter
    try
  	{
  		ECB_Mode< AES >::Encryption e;
  		e.SetKey(appkey, sizeof(appkey));
  		StringSource(s1, true,
  			new StreamTransformationFilter(e,
  				new StringSink(cipher)
  			) // StreamTransformationFilter
  		); // StringSource
  	}
  	catch(const CryptoPP::Exception& e)
  	{
  		exit(1);
  	}
    for(int i = 0; i < datapayloadsize;i++)
    {
      if(FRMPayload[bufferIndex + i] ^ cipher[i] == '\0')
      {
        break;
      }
      moutput += FRMPayload[bufferIndex + i] ^ cipher[i];
    }
  }

   return moutput;
}

void receivepacket() {

    long int SNR;
    int rssicorr;

    if(digitalRead(dio0) == 1)
    {
        if(receivePkt(message)) {
            byte value = readRegister(REG_PKT_SNR_VALUE);
            if( value & 0x80 ) // The SNR sign bit is 1
            {
                // Invert and divide by 4
                value = ( ( ~value + 1 ) & 0xFF ) >> 2;
                SNR = -value;
            }
            else
            {
                // Divide by 4
                SNR = ( value & 0xFF ) >> 2;
            }

            if (sx1272) {
                rssicorr = 139;
            } else {
                rssicorr = 157;
            }

            printf("Packet RSSI: %d, ",readRegister(0x1A)-rssicorr);
            printf("RSSI: %d, ",readRegister(0x1B)-rssicorr);
            printf("SNR: %li, ",SNR);
            printf("Length: %i",(int)receivedbytes);
            printf("\n");

            int j;
            j = bin_to_b64((uint8_t *)message, receivedbytes, (char *)(b64), 341);
            //fwrite(b64, sizeof(char), j, stdout);

            char buff_up[TX_BUFF_SIZE]; /* buffer to compose the upstream packet */
            int buff_index=0;

            /* gateway <-> MAC protocol variables */
            //static uint32_t net_mac_h; /* Most Significant Nibble, network order */
            //static uint32_t net_mac_l; /* Least Significant Nibble, network order */

            /* pre-fill the data buffer with fixed fields */
            buff_up[0] = PROTOCOL_VERSION;
            buff_up[3] = PKT_PUSH_DATA;

            /* process some of the configuration variables */
            //net_mac_h = htonl((uint32_t)(0xFFFFFFFF & (lgwm>>32)));
            //net_mac_l = htonl((uint32_t)(0xFFFFFFFF &  lgwm  ));
            //*(uint32_t *)(buff_up + 4) = net_mac_h;
            //*(uint32_t *)(buff_up + 8) = net_mac_l;

            buff_up[4] = (unsigned char)ifr.ifr_hwaddr.sa_data[0];
            buff_up[5] = (unsigned char)ifr.ifr_hwaddr.sa_data[1];
            buff_up[6] = (unsigned char)ifr.ifr_hwaddr.sa_data[2];
            buff_up[7] = 0xFF;
            buff_up[8] = 0xFF;
            buff_up[9] = (unsigned char)ifr.ifr_hwaddr.sa_data[3];
            buff_up[10] = (unsigned char)ifr.ifr_hwaddr.sa_data[4];
            buff_up[11] = (unsigned char)ifr.ifr_hwaddr.sa_data[5];

            /* start composing datagram with the header */
            uint8_t token_h = (uint8_t)rand(); /* random token */
            uint8_t token_l = (uint8_t)rand(); /* random token */
            buff_up[1] = token_h;
            buff_up[2] = token_l;
            buff_index = 12; /* 12-byte header */

            // TODO: tmst can jump is time is (re)set, not good.
            struct timeval now;
            gettimeofday(&now, NULL);
            uint32_t tmst = (uint32_t)(now.tv_sec*1000000 + now.tv_usec);

            /* start of JSON structure */
            memcpy((void *)(buff_up + buff_index), (void *)"{\"rxpk\":[", 9);
            buff_index += 9;
            buff_up[buff_index] = '{';
            ++buff_index;
            j = snprintf((char *)(buff_up + buff_index), TX_BUFF_SIZE-buff_index, "\"tmst\":%u", tmst);
            buff_index += j;
            j = snprintf((char *)(buff_up + buff_index), TX_BUFF_SIZE-buff_index, ",\"chan\":%1u,\"rfch\":%1u,\"freq\":%.6lf", 0, 0, (double)freq/1000000);
            buff_index += j;
            memcpy((void *)(buff_up + buff_index), (void *)",\"stat\":1", 9);
            buff_index += 9;
            memcpy((void *)(buff_up + buff_index), (void *)",\"modu\":\"LORA\"", 14);
            buff_index += 14;
            /* Lora datarate & bandwidth, 16-19 useful chars */
            switch (sf) {
            case SF7:
                memcpy((void *)(buff_up + buff_index), (void *)",\"datr\":\"SF7", 12);
                buff_index += 12;
                break;
            case SF8:
                memcpy((void *)(buff_up + buff_index), (void *)",\"datr\":\"SF8", 12);
                buff_index += 12;
                break;
            case SF9:
                memcpy((void *)(buff_up + buff_index), (void *)",\"datr\":\"SF9", 12);
                buff_index += 12;
                break;
            case SF10:
                memcpy((void *)(buff_up + buff_index), (void *)",\"datr\":\"SF10", 13);
                buff_index += 13;
                break;
            case SF11:
                memcpy((void *)(buff_up + buff_index), (void *)",\"datr\":\"SF11", 13);
                buff_index += 13;
                break;
            case SF12:
                memcpy((void *)(buff_up + buff_index), (void *)",\"datr\":\"SF12", 13);
                buff_index += 13;
                break;
            default:
                memcpy((void *)(buff_up + buff_index), (void *)",\"datr\":\"SF?", 12);
                buff_index += 12;
            }
            memcpy((void *)(buff_up + buff_index), (void *)"BW125\"", 6);
            buff_index += 6;
            memcpy((void *)(buff_up + buff_index), (void *)",\"codr\":\"4/5\"", 13);
            buff_index += 13;
            j = snprintf((char *)(buff_up + buff_index), TX_BUFF_SIZE-buff_index, ",\"lsnr\":%li", SNR);
            buff_index += j;
            j = snprintf((char *)(buff_up + buff_index), TX_BUFF_SIZE-buff_index, ",\"rssi\":%d,\"size\":%u", readRegister(0x1A)-rssicorr, receivedbytes);
            buff_index += j;
            memcpy( (void *)(buff_up + buff_index), (void *)",\"data\":\"", 9);
            buff_index += 9;
            int datastart = buff_index;
            j = bin_to_b64((uint8_t *)message, receivedbytes, (char *)(buff_up + buff_index), 341);
            buff_index += j;
            int dataend = buff_index;
            buff_up[buff_index] = '"';
            ++buff_index;

            /* End of packet serialization */
            buff_up[buff_index] = '}';
            ++buff_index;
            buff_up[buff_index] = ']';
            ++buff_index;
            /* end of JSON datagram payload */
            buff_up[buff_index] = '}';
            ++buff_index;
            buff_up[buff_index] = 0; /* add string terminator, for safety */

            printf("rxpk update: %s\n", (char *)(buff_up + 12)); /* DEBUG: display JSON payload */
            printf("lora data\n");
            char charlora[dataend-datastart];
            for(int i = datastart;i < dataend;i++)
            {
              charlora[i-datastart] = buff_up[i];
              printf("%c", buff_up[i]);
            }

            std::string s1(reinterpret_cast<char*>(charlora), sizeof(charlora));  //api need string parameter
            string decodedata = getdecode(s1);
            printf("Decode Data\n");
            cout << decodedata << endl;
            // for(int i = 0;i<decodedata.size();i++)
            // {
            //   if(decodedata[i]=='\0')
            //   {
            //
            //   }
            //   else{
            //     printf("%c",decodedata[i]);
            //   }
            //
            // }
            // printf("\n");
            //sendudp(buff_up, buff_index);

            fflush(stdout);

        } // received a message

    } // dio0=1
}

int main () {

    struct timeval nowtime;
    uint32_t lasttime;

    wiringPiSetup () ;
    pinMode(ssPin, OUTPUT);
    pinMode(dio0, INPUT);
    pinMode(RST, OUTPUT);

    //int fd =
    wiringPiSPISetup(CHANNEL, 500000);
    //cout << "Init result: " << fd << endl;

    SetupLoRa();

    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);  // can we rely on eth0?
    ioctl(s, SIOCGIFHWADDR, &ifr);

    /* display result */
    printf("Gateway ID: %.2x:%.2x:%.2x:ff:ff:%.2x:%.2x:%.2x\n",
           (unsigned char)ifr.ifr_hwaddr.sa_data[0],
           (unsigned char)ifr.ifr_hwaddr.sa_data[1],
           (unsigned char)ifr.ifr_hwaddr.sa_data[2],
           (unsigned char)ifr.ifr_hwaddr.sa_data[3],
           (unsigned char)ifr.ifr_hwaddr.sa_data[4],
           (unsigned char)ifr.ifr_hwaddr.sa_data[5]);

    printf("Listening at SF%i on %.6lf Mhz.\n", sf,(double)freq/1000000);
    printf("------------------\n");

    while(1) {

        receivepacket();

        gettimeofday(&nowtime, NULL);
        uint32_t nowseconds = (uint32_t)(nowtime.tv_sec);
        if (nowseconds - lasttime >= 30) {
            lasttime = nowseconds;
            sendstat();
            cp_nb_rx_rcv = 0;
            cp_nb_rx_ok = 0;
            cp_up_pkt_fwd = 0;
        }
        delay(1);
    }

    return (0);

}

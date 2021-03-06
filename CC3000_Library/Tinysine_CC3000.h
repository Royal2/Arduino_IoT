
#ifndef Tinysine_CC3000_H
#define Tinysine_CC3000_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include "utility/cc3000_common.h"
#include "utility/wlan.h"
#include "utility/netapp.h"
#include "ccspi.h"

#define WLAN_CONNECT_TIMEOUT 10000  // how long to wait, in milliseconds
#define RXBUFFERSIZE  64 // how much to buffer on the incoming side
#define TXBUFFERSIZE  32 // how much to buffer on the outgoing side

#define WIFI_ENABLE 1
#define WIFI_DISABLE 0
#define WIFI_STATUS_CONNECTED 1

typedef struct Result_Struct
{
	uint32_t	num_networks;
	uint32_t 	scan_status;
	uint8_t 	rssiByte;
	uint8_t 	Sec_ssidLen;
	uint16_t 	time;
	uint8_t 	ssid_name[32];
	uint8_t 	bssid[6];
} ResultStruct_t;  	/**!ResultStruct_t : data struct to store SSID scan results */

/* Enum for wlan_ioctl_statusget results */
typedef enum 
{
  STATUS_DISCONNECTED = 0,
  STATUS_SCANNING     = 1,
  STATUS_CONNECTING   = 2,
  STATUS_CONNECTED    = 3
} status_t;

class Tinysine_CC3000;

class Tinysine_CC3000_Client : public Print {
 public:
  Tinysine_CC3000_Client(uint16_t s);
  Tinysine_CC3000_Client(void);
  
  bool connected(void);
  size_t write(uint8_t c);

  size_t fastrprint(const char *str);
  size_t fastrprintln(const char *str);
  size_t fastrprint(const __FlashStringHelper *ifsh);
  size_t fastrprintln(const __FlashStringHelper *ifsh);

  int16_t write(const void *buf, uint16_t len, uint32_t flags = 0);
  int16_t read(void *buf, uint16_t len, uint32_t flags = 0);
  uint8_t read(void);
  int32_t close(void);
  uint8_t available(void);

  uint8_t _rx_buf[RXBUFFERSIZE], _rx_buf_idx;
  char    _tx_buf[TXBUFFERSIZE];
  int16_t bufsiz;

 private:
  int16_t _socket;

};

class Tinysine_CC3000 {
  public:
  Tinysine_CC3000(uint8_t csPin, uint8_t irqPin, uint8_t vbatPin, uint8_t spispeed = SPI_CLOCK_DIV128);
    bool     begin(uint8_t patchReq = 0);
    void     reboot(uint8_t patchReq = 0);
    void     stop(void);
    bool     disconnect(void);
    bool     deleteProfiles(void);
    void     printHex(const byte * data, const uint32_t numBytes);
    void     printHexChar(const byte * data, const uint32_t numBytes);
    void     printIPdots(uint32_t ip);
    void     printIPdotsRev(uint32_t ip);
    uint32_t IP2U32(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
    bool     getMacAddress(uint8_t address[6]);
    bool     setMacAddress(uint8_t address[6]);

    bool     connectToAP(const char *ssid, const char *key, uint8_t secmode);
    bool     connectSecure(const char *ssid, const char *key, int32_t secMode);
    bool     connectOpen(const char *ssid); 
    bool     checkConnected(void);
    bool     checkDHCP(void);
    bool     getIPAddress(uint32_t *retip, uint32_t *netmask, uint32_t *gateway, uint32_t *dhcpserv, uint32_t *dnsserv);

    bool     checkSmartConfigFinished(void);

    Tinysine_CC3000_Client connectTCP(uint32_t destIP, uint16_t destPort);
    Tinysine_CC3000_Client connectUDP(uint32_t destIP, uint16_t destPort);
     
    #ifndef CC3000_TINY_DRIVER
    bool     getFirmwareVersion(uint8_t *major, uint8_t *minor);
    status_t getStatus(void);
    uint16_t startSSIDscan(void);
    void     stopSSIDscan();
    uint8_t  getNextSSID(uint8_t *rssi, uint8_t *secMode, char *ssidname);

    bool     listSSIDResults(void);
    bool     startSmartConfig(bool enableAES);

    bool     getIPConfig(tNetappIpconfigRetArgs *ipConfig);


    uint16_t ping(uint32_t ip, uint8_t attempts=3,  uint16_t timeout=500, uint8_t size=32);
    uint16_t getHostByName(char *hostname, uint32_t *ip);
    #endif

    /* Functions that aren't available with the tiny driver */
    #ifndef CC3000_TINY_DRIVER
    bool     scanSSIDs(uint32_t time);
    #endif

    void setPrinter(Print*);

  private:
    bool _initialised;

};

extern Print* CC3KPrinter;

#endif

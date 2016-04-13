#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <pthread.h> /* Threads */

#include "esp8266.h"

ESP8266_t radio;
int fd; /* File descriptor for the port */

uint8_t ESP8266_LL_USARTInit(uint32_t baudrate)
{
    /* Init USART */
    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY );

    if( fd == 1 )
        perror("Open port: Unable to open /dev/ttyUSB0 -" );
    else
    {
        fcntl( fd, F_SETFL, 0 );
        printf( "Initialized at baud: %d\n", baudrate );
    }

    /* Return 0 = Successful */
    return 0;
}

uint8_t ESP8266_LL_USARTSend(uint8_t* _data, uint16_t count)
{
    int n = 0;
    /* Send data via USART */
    n = write( fd, _data, count );

    if( n < 0 )
        printf("write() of %d bytes failed\n", count);

    /* Return 0 = Successful */
    return 0;
}

/* USART receive interrupt handler */
void *USART_RX_INTERRUPT_HANDLER_FUNCTION_NAME( void *values )
{
    uint8_t ch;

    fcntl(fd, F_SETFL, 0);
    /* Get character from USART */
    while(1)
    {
        read(fd,&ch,1);
        ESP8266_DataReceived(&ch, 1);
        printf("%c", ch);
        usleep(100);
    }

    /* Send received character to ESP stack */

}

void *timer_ISR( void *values )
{
    while(1)
    {
        ESP8266_TimeUpdate(&radio, 1);
        usleep(1);
    }
}


int main(int argc, char *argv[])
{

    pthread_t t1, t2;

    pthread_create(&t1, NULL, timer_ISR, NULL);
    pthread_create(&t2, NULL, USART_RX_INTERRUPT_HANDLER_FUNCTION_NAME, NULL);


    printf("Initializing Radio\n");

    /* Initialize ESP8266 */
    while (ESP8266_Init(&radio, 9600) != ESP_OK)
        printf("Error trying to initialize ESP8266 module\r\n");

    /* Set mode to STA+AP */
    while(ESP8266_SetMode(&radio, ESP8266_Mode_STA_AP) != ESP_OK);

    /* Enable server on port 80 */
    while (ESP8266_ServerEnable(&radio, 80) != ESP_OK);

    /* Module is connected OK */
    printf("Initialization finished!\r\n");

    /* Disconnect from wifi if connected */
    ESP8266_WifiDisconnect(&radio);

    /* Wait till finishes */
    ESP8266_WaitReady(&radio);

#if ESP8266_USE_APSEARCH
    /* Get a list of all stations */
    ESP8266_ListWifiStations(&radio);

    /* Wait till finishes */
    ESP8266_WaitReady(&radio);
#endif

    /* Connect to wifi and save settings */
    ESP8266_WifiConnect(&radio, "YOUR SSID", "YOUR PASSWORD");

    /* Wait till finish */
    ESP8266_WaitReady(&radio);

    /* Get connected devices */
    ESP8266_WifiGetConnected(&radio);

    /* Init GPIO pin for making connections */
    /* Make pin input with pullup */
    //DDRB &= ~(1 << PINB6);
    //PORTB |= 1 << PINB6;

    /* Replace with your application code */
    while (1)
    {
        /* Update ESP stack periodically */
        ESP8266_Update(&radio);

        /* Check if pin is low, make a new connection */
        //if ((PINB & (1 << PINB6)) == 0) {
        /* Wait till pin is low */
        //while ((PINB & (1 << PINB6)) == 0);

        /* Make a new connection */
        while(ESP8266_StartClientConnection(&radio, "stm32f4disco", "stm32f4-discovery.com", 80, NULL) != ESP_OK);
    }
}

/************************************/
/*           ESP CALLBACKS          */
/************************************/
/* Called when ready string detected */
void ESP8266_Callback_DeviceReady(ESP8266_t* ESP8266)
{
    printf("Device is ready\r\n");
}

/* Called when watchdog reset on ESP8266 is detected */
void ESP8266_Callback_WatchdogReset(ESP8266_t* ESP8266)
{
    printf("Watchdog reset detected!\r\n");
}

/* Called when we are disconnected from WIFI */
void ESP8266_Callback_WifiDisconnected(ESP8266_t* ESP8266)
{
    printf("Wifi is disconnected!\r\n");
}

void ESP8266_Callback_WifiConnected(ESP8266_t* ESP8266)
{
    printf("Wifi is connected!\r\n");
}

void ESP8266_Callback_WifiConnectFailed(ESP8266_t* ESP8266)
{
    printf("Connection to wifi network has failed. Reason %d\r\n", ESP8266->WifiConnectError);
}

void ESP8266_Callback_WifiGotIP(ESP8266_t* ESP8266)
{
    printf("Wifi got an IP address\r\n");

    /* Read that IP from module */
    printf("Grabbing IP status: %d\r\n", ESP8266_GetSTAIP(ESP8266));
}

void ESP8266_Callback_WifiIPSet(ESP8266_t* ESP8266)
{
    /* We have STA IP set (IP set by router we are connected to) */
    printf("We have valid IP address: %d.%d.%d.%d\r\n", ESP8266->STAIP[0], ESP8266->STAIP[1], ESP8266->STAIP[2], ESP8266->STAIP[3]);
}

void ESP8266_Callback_DHCPTimeout(ESP8266_t* ESP8266)
{
    printf("DHCP timeout!\r\n");
}

void ESP8266_Callback_WifiDetected(ESP8266_t* ESP8266, ESP8266_APs_t* ESP8266_AP)
{
    uint8_t i = 0;

    /* Print number of detected stations */
    printf("We have detected %d AP stations\r\n", ESP8266_AP->Count);

    /* Print each AP */
    for (i = 0; i < ESP8266_AP->Count; i++)
    {
        /* Print SSID for each AP */
        printf("%2d: %s\r\n", i, ESP8266_AP->AP[i].SSID);
    }
}


/************************************/
/*         CLIENT CALLBACKS         */
/************************************/
void ESP8266_Callback_ClientConnectionConnected(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection)
{
    /* We are connected to external server */
    printf("Client connected to server! Connection number: %s\r\n", Connection->Name);

    /* We are connected to server, request to sent header data to server */
    ESP8266_RequestSendData(ESP8266, Connection);
}

/* Called when client connection fails to server */
void ESP8266_Callback_ClientConnectionError(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection)
{
    /* Fail with connection to server */
    printf("An error occurred when trying to connect on connection: %d\r\n", Connection->Number);
}

/* Called when data are ready to be sent to server */
uint16_t ESP8266_Callback_ClientConnectionSendData(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection, char* Buffer, uint16_t max_buffer_size)
{
    /* Format data to sent to server */
    sprintf(Buffer, "GET / HTTP/1.1\r\n");
    strcat(Buffer, "Host: stm32f4-discovery.com\r\n");
    strcat(Buffer, "Connection: close\r\n");
    strcat(Buffer, "\r\n");

    /* Return length of buffer */
    return strlen(Buffer);
}

/* Called when data are send successfully */
void ESP8266_Callback_ClientConnectionDataSent(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection)
{
    printf("Data successfully sent as client!\r\n");
}

/* Called when error returned trying to sent data */
void ESP8266_Callback_ClientConnectionDataSentError(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection)
{
    printf("Error while sending data on connection %d!\r\n", Connection->Number);
}

void ESP8266_Callback_ClientConnectionDataReceived(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection, char* Buffer)
{
    /* Data received from server back to client */
    printf("Data received from server on connection: %s; Number of bytes received: %lu; %lu / %lu;\r\n",
    Connection->Name,
    Connection->BytesReceived,
    Connection->TotalBytesReceived,
    Connection->ContentLength
    );

    /* Print message when first packet */
    if (Connection->FirstPacket)
    {

        /* Print first message */
        printf("This is first packet received. Content length on this connection is: %lu\r\n", Connection->ContentLength);
    }
}

/* Called when connection is closed */
void ESP8266_Callback_ClientConnectionClosed(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection)
{
    printf("Client connection closed, connection: %d; Total bytes received: %lu;\
            Content-Length header: %lu\r\n", Connection->Number,
            Connection->TotalBytesReceived,
            Connection->ContentLength
    );
}

/* Called when timeout is reached on connection to server */
void ESP8266_Callback_ClientConnectionTimeout(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection)
{
    printf("Timeout reached on connection: %d\r\n", Connection->Number);
}

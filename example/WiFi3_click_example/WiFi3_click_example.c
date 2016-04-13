/*WiFi3 click board demonstration example

WiFi3 click board
MCU:PIC18F45K22
WiFi module: ESP8266MOD

Tools: EasyPIC v7, mikro compiler's USART and UDP terminals


This simple exmple demonstrates how to configure ESP8266MOD wifi module as a server by connecting
the module to an access point and then receiving data (text) sent from UDP terminal. All of the 
AT commands used for configuring the wifi module and its appropriate responses issued by the wifi 
module too, are written on the USART terminal screen in respect of their ordering. After the wifi
module is configured/connected to the known access point (user should know the access point's SSID/
pasword - set them in wifi3_parameters header file) the user can test the module by sending data from
UDP terminal (type the module's IP address and port number on the UDP terminal fields and then connect
to the module). The wifi module IP address is shown on the USART terminal during the module's init/config 
period of time.
ESP8266MOD wifi module is interfaced with MCU via UART1's RX/TX lines, also the CH_PD (power down)
line should be at high logic level at power-on of the module, and all the time of the module working.
MCU uses its UART2 module for communicating with USART terminal.

Settings: SW1.8, SW2.8 in ON position, connect RS-232 serial cable or USB B side cable to RS-232/USB UART
port on the development system board, set the USB/UART jumpers in the appropriate position

*/


//constant declarations
#define    CH_PD       RA2_bit
#define    GPIO15      RE1_bit

unsigned short RXbuffer[128];           //adjust RX buffer size length as long as you want data (chars) to receive (sent from UDP terminal) in this example
unsigned short RXbyte_counter;
unsigned short AT_flag;
unsigned short Fail_to_connect_flag;
unsigned short ATnext_cmd;
unsigned short ATcmd_index;

unsigned short i;

//include header files
#include  "wifi3_parameters.h"
#include  "wifi3_at_commands.h"

void Interrupt() {
     //PORTB = 0x22;  //DEBUG
     //CONNECTING TIMEOUT - ERROR
     //if (TMR0IF_bit == 1) {
     //   TMR0IF_bit = 0;
        //...
     //}
     //UART Receiver Interrupt occureed
     if (UART1_Data_Ready() == 1) RXbuffer[RXbyte_counter] = UART1_Read();
     //else
      //   return;
      RXbyte_counter++;
     //end of RX message response interpreed depending on the current AT command executing!!!  ATnext_cmd is not necessary now!
     //if (RXbuffer[RXbyte_counter] == LF & RXbuffer[RXbyte_counter - 1] == CR) {
     //   AT_flag == 1;   //the end of message (from wife module via UART lines) detected
     //   ATnext_cmd++;   //pointer to the AT command routine to be executed as next one in the initialization row

     AT_end_msg();

}


void WiFi3_Init() {
     //parameters init
     AT_flag = 0;
     ATnext_cmd = 0;
     RXbyte_counter = 0;

     UART1_Init(9600);     //init UART1 module - communication with wifi module
     UART2_Init(9600);     //init UART2 module - communication with USART terminal
     Delay_ms(1000);
     CH_PD = 1;           //power down line at high level
     GPIO15 = 0;          //bind GPIO15 pin at low level on starting the module (already low via resistor)
     UART2_Write_Text("Please wait...");
     CRLF_Write();
     Delay_ms(5000);         //wait five seconds on power-on befer

     //interrupt init
     RC1IE_bit = 1;
     RC2IE_bit = 1;
     PEIE_bit = 1;
     GIE_bit = 1;
     //init timer  - ERROR on the screen if not connected to the access point
}

void WiFi3_Start_msg() {
     UART2_Write_Text("Start Initialisation...");
     CRLF_Write();
     CRLF_Write();
     delay_ms(2000);
}

void WiFi3_AP_connect() {
     WiFi3_Restart_cmd();                //RESET module
     Delay_ms(1000);
     WiFi3_Mode_Test_cmd();              //Test wifi modes
     Delay_ms(1000);
     WiFi3_Mode_Read_cmd();              //Read wifi mode
     Delay_ms(1000);
     WiFi3_Mode_Write_cmd(1);            //Set wifi mode (1-STA,2-AP or 3-BOTH)
     Delay_ms(1000);
     WiFi3_Connections_cmd(1);           //set multiple connection (0-single, 1-multiple)
     Delay_ms(1000);
     WiFi3_Set_Server_cmd(1);            //set as server: mode 0 to close; mode 1 to open
     Delay_ms(2000);
     UART2_Write_Text("Connecting to access point...");
     CRLF_Write();
     Delay_ms(800);
     WiFi3_Join_AP_cmd();                //Join Access Point - set SSID and pasword in "wifi3_parameters.h"
     if (Fail_to_connect_flag == 1) {
        CRLF_Write();
        CRLF_Write();
        UART2_Write_Text("NOT connected:");
        CRLF_Write();
        UART2_Write_Text("1.Wrong SSID or pasword");
        CRLF_Write();
        UART2_Write_Text("2.WiFi mode not adjusted for this type of connection");
        CRLF_Write();
        UART2_Write_Text("3.Another reasons - restart the module");
        CRLF_Write();
        CRLF_Write();
        while (1);
     }
     Delay_ms(500);
     WiFi3_Get_IP_cmd();                 //check assigned IP address
     UART2_Write_Text("Connected!");     //connected
     CRLF_Write();
     CRLF_Write();
     delay_ms(1000);
     //server ready for data receiving
     UART2_Write_Text("Server ready to receive data (send text from UDP terminal):");
     CRLF_Write();
     CRLF_Write();
     while (1) {
           WiFi3_Data_Receiving();             //receive data (text) sent by UDP terminal via wifi signal; then show received data on USART terminal
     }

}



void main() {

     //MCU init
     ANSELA = 0;
     ANSELB = 0;
     ANSELC = 0;
     ANSELD = 0;
     ANSELE = 0;
     TRISA = 0;
     TRISB = 0;
     TRISD = 0;
     TRISE = 0;


     PORTA = 0;
     PORTB = 0;
     PORTD = 0;
     PORTE = 0;

     //Initialization
     WiFi3_Init();

     WiFi3_Start_msg();
     WiFi3_AP_connect();    //connect to Access Point



}
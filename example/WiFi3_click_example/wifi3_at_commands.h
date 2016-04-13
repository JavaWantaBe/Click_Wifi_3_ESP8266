//AT commands processing routines and data receiving routine


//AT commands index constants
#define      AT_RST             1
#define      AT_CWMODE_test     2
#define      AT_CWMODE_read     3
#define      AT_CWMODE_write    4
#define      AT_CWJAP           5
#define      AT_CIPMUX          6
#define      AT_CIPSERVER       7
#define      AT_CIFSR           8
#define      WIFI_DATA          9

////////declaration of routines////////////////////////////////////////////////////
void WiFi3_Restart_cmd(void);
void AT_Restart_resp_end(void);

void WiFi3_Mode_Test_cmd(void0);
void AT_Mode_Test_resp_end(void);

void WiFi3_Mode_Read_cmd(void);
void AT_Mode_Read_resp_end(void);

void WiFi3_Mode_Write_cmd(unsigned short);
void AT_Mode_Write_resp_end(void);

void WiFi3_Join_Ap_cmd(void);
void AT_Join_AP_resp_end(void);

void WiFi3_Connections_cmd(unsigned short);
void AT_Connections_resp_end(void);

void WiFi3_Set_Server_cmd(unsigned short);
void AT_Set_Server_resp_end(void);

void WiFi3_Get_IP_cmd(void);
void AT_Get_IP_resp_end(void);

void WiFi3_Data_Receiving(void);
void AT_Data_Receiving_resp_end(void);
///////////////////////////////////////////////////////////////////////////////////

//write <CR><LF>
void AT_CRLF_Write() {
     UART1_Write(0x0D);           //CR
     UART1_Write(0x0A);           //LF
}

//write <CR><LF> via UART2
void CRLF_Write() {
     UART2_Write(0x0D);
     UART2_Write(0x0A);
}

unsigned char *AT_RST_msg = "AT+RST";
unsigned char *AT_JAP_msg = "AT+CWJAP=";
unsigned char *AT_MODEtest = "AT+CWMODE=?";
unsigned char *AT_MODEread = "AT+CWMODE?";
unsigned char *AT_MODEwrite_1 = "AT+CWMODE=1";
unsigned char *AT_MODEwrite_2 = "AT+CWMODE=2";
unsigned char *AT_MODEwrite_3 = "AT+CWMODE=3";
unsigned char *AT_MUX_msg_1 = "AT+CIPMUX=1";
unsigned char *AT_MUX_msg_2 = "AT+CIPMUX=2";
unsigned char *AT_SERVER_msg_1 = "AT+CIPSERVER=1";
unsigned char *AT_SERVER_msg_2 = "AT+CIPSERVER=2";
unsigned char *AT_GETIP_msg = "AT+CIFSR";



//INTERRUPT ROUTINE - the end of AT COMMAND RESPONSE detection "ROUTER" ROUTINE///////////////
void AT_end_msg() {
     switch (ATcmd_index) {
            case AT_RST: AT_Restart_resp_end(); break;
            case AT_CWMODE_test: AT_Mode_Test_resp_end(); break;
            case AT_CWMODE_read: AT_Mode_Read_resp_end(); break;
            case AT_CWMODE_write: AT_Mode_Write_resp_end(); break;
            case AT_CWJAP: AT_Join_AP_resp_end(); break;
            case AT_CIPMUX: AT_Connections_resp_end(); break;
            case AT_CIPSERVER: AT_Set_Server_resp_end(); break;
            case AT_CIFSR: AT_Get_IP_resp_end(); break;
            case WIFI_DATA: AT_Data_Receiving_resp_end();
     }
}
//////////////////////////////////////////////////////////////////////////////


///AT RESTART COMMAND ROUTINES///////////////////////////////////////////////////////////////////////////////


void WiFi3_Restart_cmd() {
     ATcmd_index = AT_RST;
     //transmit TX
     UART1_Write_Text(AT_RST_msg);
     AT_CRLF_Write();
     //GIE_bit = 0;
     PORTB = 0x33; //DEBUG
     //receive msg
     while (1) {
           PORTB = 0x77; //DEBUG
           if (AT_flag == 1) {
              GIE_bit = 0;
              break;  //the end of message detected
           }
     }
     //interpret received message (RXbuffer array) if necessary for this AT command
     PORTB = 0x11;       //DEBUG
     for (i = 0; i < RXbyte_counter; i++) {
         UART2_Write(RXbuffer[i]);
     }
}

//restart AT response end
void AT_Restart_resp_end() {
     PORTB = 0xF0;        //DEBUG
     //AT_flag = 1;
     if ((RXbuffer[RXbyte_counter - 2] == 0x0D) && (RXbuffer[RXbyte_counter - 1] == 0x0A)) {
        if (RXbuffer[RXbyte_counter - 3] == 'y')
           if (RXbuffer[RXbyte_counter - 4] == 'd')
              if (RXbuffer[RXbyte_counter - 5] == 'a')
                 if (RXbuffer[RXbyte_counter - 6] == 'e')
                    if (RXbuffer[RXbyte_counter - 7] == 'r') AT_flag = 1;
        
        //if (RXbuffer[RXbyte_counter - 3] == 'K')
           //if (RXbuffer[RXbyte_counter - 4] == 'O') AT_flag = 1;
        //AT_flag = 1;
     }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//AT WIFI MODE TEST COMMAND//////////////////////////////////////////////////////////////////////////////////

void WiFi3_Mode_Test_cmd() {
     //init flags and counter
     AT_flag = 0;
     GIE_bit = 1;
     RXbyte_counter = 0;
     ATcmd_index = AT_CWMODE_test;          //AT command "end of message" index
     //transmit AT command
     UART1_Write_Text(AT_MODEtest);
     AT_CRLF_Write();
     //RX interrupt loop
     while (1) {
           if (AT_flag == 1) {
              GIE_bit = 0;
              break;
           }
     }
     //interpret received message (RXbuffer array) if necessary for this AT command
     PORTB = 0x12;       //DEBUG
     for (i = 0; i < RXbyte_counter; i++) {
         UART2_Write(RXbuffer[i]);
     }
}

//Wifi mode test AT response end
void AT_Mode_Test_resp_end() {
     if ((RXbuffer[RXbyte_counter - 2] == 0x0D) && (RXbuffer[RXbyte_counter - 1] == 0x0A)) {
        //if (RXbuffer[RXbyte_counter - 3] == 0x0A)
        //   if (RXbuffer[RXbyte_counter - 4] == 0x0D) AT_flag =1;
        if (RXbuffer[RXbyte_counter - 3] == 'K')
           if (RXbuffer[RXbyte_counter - 4] == 'O') AT_flag = 1;
        //AT_flag = 1;
     }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////

//AT WIFI MODE READ COMMAND ROUTINES////////////////////////////////////////////////////////////////////

void WiFi3_Mode_Read_cmd() {
     //init flags and counter
     AT_flag = 0;
     GIE_bit = 1;
     RXbyte_counter = 0;
     ATcmd_index = AT_CWMODE_read;          //AT command "end of response" index
     //transmit AT command
     UART1_Write_Text(AT_MODEread);
     AT_CRLF_Write();
     //RX interrupt loop
     while (1) {
           if (AT_flag == 1) {
              GIE_bit = 0;
              break;
           }
     }
     //interpret received message (RXbuffer array) if necessary for this AT command
     PORTB = 0x12;       //DEBUG
     for (i = 0; i < RXbyte_counter; i++) {
         UART2_Write(RXbuffer[i]);
     }
}

//Wifi mode AT end read
void AT_Mode_Read_resp_end() {
     if ((RXbuffer[RXbyte_counter - 2] == 0x0D) && (RXbuffer[RXbyte_counter - 1] == 0x0A)) {
        if (RXbuffer[RXbyte_counter - 3] == 'K')
           if (RXbuffer[RXbyte_counter - 4] == 'O') AT_flag = 1;
     }

}

/////////////////////////////////////////////////////////////////////////////////////////////////

//AT WIFI MODE WRITE COMMAND ROUTINES////////////////////////////////////////////////////////////
void WiFi3_Mode_Write_cmd(unsigned short _set_value) {
     //init parameters
     AT_flag = 0;
     GIE_bit = 1;
     RXbyte_counter = 0;
     ATcmd_index = AT_CWMODE_write;      //AT command "end of response" index
     switch (_set_value) {
            case 1: UART1_Write_Text(AT_MODEwrite_1); break;
            case 2: UART1_Write_Text(AT_MODEwrite_2); break;
            case 3: UART1_Write_Text(AT_MODEwrite_3); break;
     }
     AT_CRLF_Write();
     //RX interrupt loop
     while (1) {
           //PORTB = 0XF3;
           if (AT_flag == 1) {
              GIE_bit = 0;
              break;
           }
     }
     //interpret received message (RXbuffer array) if necessary for this AT command
     PORTB = 0x13;       //DEBUG
     for (i = 0; i < RXbyte_counter; i++) {
         UART2_Write(RXbuffer[i]);
     }
}

//wifi mode AT write end of response check routine
void AT_Mode_Write_resp_end() {
     PORTB = 0XF4;            //DEBUG
     if ((RXbuffer[RXbyte_counter - 2] == 0x0D) && (RXbuffer[RXbyte_counter - 1] == 0x0A)) {
        if (RXbuffer[RXbyte_counter - 3] == 'K')
           if (RXbuffer[RXbyte_counter - 4] == 'O') {
              AT_flag = 1;
              return;
           }
     }
     //or if no change (parameter already set)
     if ((RXbuffer[RXbyte_counter - 2] == 0x0D) && (RXbuffer[RXbyte_counter - 1] == 0x0A)) {
        if (RXbuffer[RXbyte_counter - 3] == 'e')
           if (RXbuffer[RXbyte_counter - 4] == 'g')
              if (RXbuffer[RXbyte_counter - 5] == 'n')
                 if (RXbuffer[RXbyte_counter - 6] == 'a')
                    if (RXbuffer[RXbyte_counter - 7] == 'h')
                       if (RXbuffer[RXbyte_counter - 8] == 'c') AT_flag = 1;
     }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

//AT JOIN ACCESS POINT ROUTINES/////////////////////////////////////////////////////////////////////////
void WiFi3_Join_Ap_cmd() {
     //init parameters
     AT_flag = 0;
     Fail_to_connect_flag = 0;
     GIE_bit = 1;
     RXbyte_counter = 0;
     ATcmd_index = AT_CWJAP;      //AT command "end of response" index
     //write in this format AT+CWJAP="SSID","pasword" <CR><LF>
     //SSID and pasword in "wifi3_parameters.h" header file
     UART1_Write_Text(AT_JAP_msg);     //write AT+CWJAP=
     UART1_Write('"');                 //write "
     UART1_Write_Text(SSID);           //write <SSID>
     UART1_Write('"');                 //write "
     UART1_Write(',');                 //write ,
     UART1_Write('"');                 //write "
     UART1_Write_Text(pasword);        //write <pasword>
     UART1_Write('"');                 //write "
     
     AT_CRLF_Write();
     //RX interrupt loop
     while (1) {
           if (AT_flag == 1 || Fail_to_connect_flag == 1) {
              GIE_bit = 0;
              break;
           }
     }
     //interpret received message (RXbuffer array) if necessary for this AT command
     PORTB = 0x16;       //DEBUG
     for (i = 0; i < RXbyte_counter; i++) {
         UART2_Write(RXbuffer[i]);
     }
}

//wifi mode AT write end of response check routine
void AT_Join_AP_resp_end() {
     if ((RXbuffer[RXbyte_counter - 2] == 0x0D) && (RXbuffer[RXbyte_counter - 1] == 0x0A)) {
        if (RXbuffer[RXbyte_counter - 3] == 'K')
           if (RXbuffer[RXbyte_counter - 4] == 'O') {
              AT_flag = 1;
              return;
           }
           //WRONG SSID or pasword or no wifi signal
        if (RXbuffer[RXbyte_counter - 3] == 'L')
           if (RXbuffer[RXbyte_counter - 4] == 'I')
              if (RXbuffer[RXbyte_counter - 5] == 'A')
                 if (RXbuffer[RXbyte_counter - 6] == 'F')
                    Fail_to_connect_flag = 1;
        //WRONG WIFI MODE
        if (RXbuffer[RXbyte_counter - 3] == 'R')
           if (RXbuffer[RXbyte_counter - 4] == 'O')
              if (RXbuffer[RXbyte_counter - 5] == 'R')
                 if (RXbuffer[RXbyte_counter - 6] == 'R')
                    if (RXbuffer[RXbyte_counter - 7] == 'E')
                       Fail_to_connect_flag = 1;
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

//AT Connections ROUTINES///////////////////////////////////////////////////////////////////////////////
void WiFi3_Connections_cmd(unsigned short _set_value) {
     //init parameters
     AT_flag = 0;
     GIE_bit = 1;
     RXbyte_counter = 0;
     ATcmd_index = AT_CIPMUX;
     switch (_set_value) {
            case 1: UART1_Write_Text(AT_MUX_msg_1); break;
            case 2: UART1_Write_Text(AT_MUX_msg_2); break;
     }
     AT_CRLF_Write();
     //RX interrupt loop
     while (1) {
           if (AT_flag == 1) {
              GIE_bit = 0;
              break;
           }
     }
     //interpret received message (RXbuffer array) if necessary for this AT command
     PORTB = 0x14;       //DEBUG
     for (i = 0; i < RXbyte_counter; i++) {
         UART2_Write(RXbuffer[i]);
     }
}

void AT_Connections_resp_end() {
     if ((RXbuffer[RXbyte_counter - 2] == 0x0D) && (RXbuffer[RXbyte_counter - 1] == 0x0A)) {
        if (RXbuffer[RXbyte_counter - 3] == 'K')
           if (RXbuffer[RXbyte_counter - 4] == 'O') AT_flag = 1;
     }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//AT SET as Server routines//////////////////////////////////////////////////////////////////////////////////
void WiFi3_Set_Server_cmd(unsigned short _set_value) {
     //init parameters
     AT_flag = 0;
     GIE_bit = 1;
     RXbyte_counter = 0;
     ATcmd_index = AT_CIPSERVER;
     //write in this format: AT+CIPSERVER=<mode>,<port><CR><LF>....... set port value in "wifi3_parameters.h" file
     switch (_set_value) {
            case 1: UART1_Write_Text(AT_SERVER_msg_1); break;
            case 2: UART1_Write_Text(AT_SERVER_msg_2); break;
     }
     UART1_Write(',');                                 //write ,
     UART1_Write_Text(port);                           //write <port> number
     AT_CRLF_Write();
     //RX interrupt loop
     while (1) {
           if (AT_flag == 1) {
              GIE_bit = 0;
              break;
           }
     }
     //interpret received message (RXbuffer array) if necessary for this AT command
     PORTB = 0x15;       //DEBUG
     for (i = 0; i < RXbyte_counter; i++) {
         UART2_Write(RXbuffer[i]);
     }
}

void AT_Set_Server_resp_end() {
     if ((RXbuffer[RXbyte_counter - 2] == 0x0D) && (RXbuffer[RXbyte_counter - 1] == 0x0A)) {
        if (RXbuffer[RXbyte_counter - 3] == 'K')
           if (RXbuffer[RXbyte_counter - 4] == 'O') AT_flag = 1;
     }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WiFi3_Get_IP_cmd() {
     //init flags and counter
     AT_flag = 0;
     GIE_bit = 1;
     RXbyte_counter = 0;
     ATcmd_index = AT_CIFSR;          //AT command "end of response" index
     //transmit AT command
     UART1_Write_Text(AT_GETIP_msg);
     AT_CRLF_Write();
     //RX interrupt loop
     while (1) {
           if (AT_flag == 1) {
              GIE_bit = 0;
              break;
           }
     }
     //interpret received message (RXbuffer array) if necessary for this AT command
     PORTB = 0x17;       //DEBUG
     for (i = 0; i < RXbyte_counter; i++) {
         UART2_Write(RXbuffer[i]);
     }
}

void AT_Get_IP_resp_end() {
     if ((RXbuffer[RXbyte_counter - 2] == 0x0D) && (RXbuffer[RXbyte_counter - 1] == 0x0A)) {
        if (RXbuffer[RXbyte_counter - 3] == 'K')
           if (RXbuffer[RXbyte_counter - 4] == 'O') AT_flag = 1;
     }
}


//data receiving routines //////////////////////////////////////////////////////////////////////
void WiFi3_Data_Receiving() {
     //init parameters
     AT_flag = 0;
     RXbyte_counter = 0;
     GIE_bit = 1;
     //RX interrupt loop
     while (1) {
           if (AT_flag == 1) {
              GIE_bit = 0;
              break;
           }
     }
     //interpret received message (RXbuffer array) if necessary for this AT command
     PORTB = 0x18;       //DEBUG
     for (i = 0; i < RXbyte_counter; i++) {
         UART2_Write(RXbuffer[i]);
     }
}

void AT_Data_Receiving_resp_end() {
     if ((RXbuffer[RXbyte_counter - 2] == 0x0D) && (RXbuffer[RXbyte_counter - 1] == 0x0A)) {
        if (RXbuffer[RXbyte_counter - 3] == 'K')
           if (RXbuffer[RXbyte_counter - 4] == 'O') AT_flag = 1;
     }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
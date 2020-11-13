// Authors:

// Brennan Basinger
// bdb264

// Sam Boggs
// sjb578

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <netinet/in.h>
#include <time.h>
#include <stdio.h>


#include <stdlib.h>
#include <cstring>
#include <cstdlib>
#include<iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <fstream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include "packet.h"
#include <unistd.h>
#include "packet.cpp"
#include <string.h>

#define MAX 80
#define packetLen 37



using namespace std;
//

void error(const char *msg)
{
    perror(msg);
    exit(1);
}


int main(int argc, char *argv[]) {
    //int sockfd = 0;
    //int portno;


    char buffer[512];

    //struct sockaddr_in serv_addr;



    //1
    struct sockaddr_in client_address;
    socklen_t clientLength = sizeof(client_address);


    int recMsgSize;
    int packet_received = -1;
    int packetLength = 30;
    int packetType;
    //int s = 0;
    //int offset;

    //int packetSequenceNum = 0;
    int packetSequenceNum;


    // 2
    //SETUP DATAGRAM SOCKET FOR RECEIVING
    int ESSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(ESSocket < 0){
        cout << "Error: failed to open datagram socket.\n";
    }

    // 3
    // set up the sockaddr_in structure for receiving
    struct sockaddr_in ES;
    socklen_t ES_length = sizeof(ES);
    bzero(&ES, sizeof(ES));
    ES.sin_family = AF_INET;
    ES.sin_addr.s_addr = htonl(INADDR_ANY);
    char * end;
    int sr_rec_port = strtol(argv[2], &end, 10);  // server's receiving port and convert to int
    ES.sin_port = htons(sr_rec_port);             // set to emulator's receiving port


    //4
    // do the binding
    if (bind(ESSocket, (struct sockaddr *)&ES, ES_length) == -1)
        cout << "Error in binding.\n";

    // ******************************************************************
    // ******************************************************************



    //5
    struct hostent *em_host;            // pointer to a structure of type hostent
    em_host = gethostbyname(argv[1]);   // host name for emulator
    if(em_host == NULL){
        cout << "Failed to obtain emulator.\n";
        exit(EXIT_FAILURE);
    }



    //6
    int SESocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(SESocket < 0){
        cout << "Error in trying to open datagram socket.\n";
        exit(EXIT_FAILURE);
    }


    // 7
    // setup sockaddr_in struct
    struct sockaddr_in SE;
    memset((char *) &SE, 0, sizeof(SE));
    SE.sin_family = AF_INET;
    bcopy((char *)em_host->h_addr, (char*)&SE.sin_addr.s_addr, em_host->h_length);
    int em_rec_port = strtol(argv[3], &end, 10);
    SE.sin_port = htons(em_rec_port);

    cout << "Hello There " << endl;





    ofstream log("arrival.log", ios_base::out | ios_base::trunc);

    //portno = atoi(argv[1]);

    /*
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        cout << "Failed to create socket" << endl;
    }
     */


    /*
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
     */


    /*
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
        cout << "Failed to Bind" << endl;
    }
     */


    ofstream akLog("clientack.log", ios_base::out | ios_base::trunc);

    cout << "Hello " << endl;

    int bytesReceived = 0;
    char fileContents[512];
    memset(fileContents, 0, sizeof(fileContents));
    fileContents[511] = '\0';

    char payloadA[512];
    memset(payloadA, 0, 512);
    char serialized[37];
    memset(serialized, 0, 37);
    char ackPacketMsg[] = "ACK PACKET: MESSAGE RECEIVED!";

    packet *received_packet = new packet(0, 0, 0, payloadA);

    cout << "Entering While Loop" << endl;

    FILE *fp = fopen("output.txt", "a");
    while (1) {
        cout << "Inside While Loop " << endl;


        recvfrom(ESSocket, serialized, sizeof(serialized), 0, (struct sockaddr *) &ES, &ES_length);
        //if (recMsgSize < 0)
        //{
            //cout << "Error: Failed To Receive Message!" << endl << endl;
        //}



        received_packet->deserialize(serialized);
        received_packet->printContents();

        log << received_packet->getSeqNum() << endl;
        cout << endl << endl;

        cout << "PRINTING TYPE: " << received_packet->getType() << endl << endl;

        cout << endl;
        //cout << "Packet Data: " << endl << received_packet->getData() << endl;
        //cout << "Packet Length " << received_packet->getLength() << endl;
        cout << endl;


        cout << "SENDING ACK!" << endl << endl;

        sendto(SESocket, ackPacketMsg, 64, 0, (struct sockaddr *) &SE, sizeof(struct sockaddr_in));
        
        cout << "LINE 222\n";



        packetType = received_packet->getType();
        packetSequenceNum = received_packet->getSeqNum();
        cout << endl << endl << endl;
        cout << "Packet Sequence Number: " << packetSequenceNum << endl << endl << endl;
        log << packetSequenceNum << endl;
        //if (packetType == 2)
            //break;
        //memcpy( destination, source, num)
        //memcpy(fileContents + bytesReceived, received_packet->getData(), 30);
        strncpy(fileContents + bytesReceived, received_packet->getData(), 30);
        bytesReceived += strlen(received_packet->getData());

        cout << "GOING INTO IF STATEMENT" << endl;

        if (packetSequenceNum == packet_received + 1)
        {
            packet_received++;
            
            if(sendto(SESocket, ackPacketMsg, 64, 0, (struct sockaddr *) &SE, clientLength) <0) {
                cout << " FAILED TO SEND \n";
            }
            cout << "Actual packet acknowledgement 245\n";
        }
        // After the server has received all data packets and an EOT from the client,
        // it should send an EOT
        // packet with the type field set to 2, and then exit.
        // EOT end of transmission type field == 2
        // ACK packet == 0
        // Data Packet == 1

        //FILE* fp = fopen( "Output.txt", "wa");
  
        fwrite(fileContents, sizeof(char), bytesReceived, fp);
        //fclose(fp);
        //DELETE THIS LATER
        if (packetSequenceNum >= 4)
            break;





    }



    cout << "End Of Transmission Packet Received..." << endl;
    cout << "Attempting to write to file.." << endl;
    cout << "Closing File Object" << endl;
    cout << "Closing Socket..." << endl;
    //close();
    cout << "Gracefully Shutting Down..." << endl;


    return 0;


}













/*

    int nBytes;
    char udpBuffer[1024];
    //char udpSendBuffer[4];
    //char *hello = "Hello There";

    struct sockaddr_in servaddr, cliaddr;

    //cout << "Creating Socket File Descriptor" << endl;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
    {
        cout << "Failed to create socket" << endl;
        return 0;
    }

    //cout << "Successfully Created Socket" << endl;

    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 

    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(portno);

    bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));


    //cout << "Successfully Binded to " << portNumber << endl;

    socklen_t l = sizeof(cliaddr);
    //return 0;
 
        //cout << "Opening and Writing to File: dataReceived.txt" << endl;
                
        FILE* fp = fopen( "Output.txt", "wb");
        b = recvfrom(sockfd, udpBuffer, 1024, 0,(struct sockaddr*)&cliaddr, &l);
        
        cout << "Writing to File" << endl;
        //tot += b;
        fwrite(udpBuffer, sizeof(char), b, fp);


            
        

        cout << "Received Bytes: " << b << endl;

        if (b < 0)
        {
            perror("Receiving");
        }

        fclose(fp);
        
        close(sockfd);
        
        return 0;
*/



















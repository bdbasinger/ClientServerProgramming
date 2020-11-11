// Authors:

// Brennan Basinger
// bdb264

// Sam Boggs
// netid

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


void error(const char *msg)
{
    perror(msg);
    exit(1);
}


int main(int argc, char *argv[]) {
    int sockfd = 0;
    int portno;


    char buffer[512];

    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;

    socklen_t clilen = sizeof(cli_addr);

    int recMsgSize;
    int packet_received = -1;
    int packetLength = 30;
    int packetType;
    //int s = 0;
    //int offset;

    //int packetSequenceNum = 0;
    int packetSequenceNum;

    if (argc < 2) {
        cout << "Error! Port Number Required." << endl;
        exit(1);
    }

    ofstream log("arrival.log", ios_base::out | ios_base::trunc);
    portno = atoi(argv[1]);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        cout << "Failed to create socket" << endl;
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
        cout << "Failed to Bind" << endl;
    }

    FILE *fp = fopen("output.txt", "wa");
    cout << "Creating File: output.txt" << endl;

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


    while (1) {
        recMsgSize = recvfrom(sockfd, serialized, 37, 0, (struct sockaddr *) &cli_addr, &clilen);
        if (recMsgSize < 0)
            cout << "Error: Failed To Receive Message!" << endl << endl;

        received_packet->deserialize(serialized);
        received_packet->printContents();
        cout << endl << endl;
        cout << "PRINTING TYPE: " << received_packet->getType() << endl << endl;

        cout << endl;
        //cout << "Packet Data: " << endl << received_packet->getData() << endl;
        //cout << "Packet Length " << received_packet->getLength() << endl;
        cout << endl;


        cout << "SENDING ACK!" << endl << endl;
        sendto(sockfd, ackPacketMsg, 64, 0, (struct sockaddr *) &cli_addr, clilen);

        packetType = received_packet->getType();
        packetSequenceNum = received_packet->getSeqNum();
        cout << endl << endl << endl;
        cout << "Packet Sequence Number: " << packetSequenceNum << endl << endl << endl;
        log << packetSequenceNum << endl;
        if (packetType == 2)
            break;
        //memcpy( destination, source, num)
        //memcpy(fileContents + bytesReceived, received_packet->getData(), 30);
        strncpy(fileContents + bytesReceived, received_packet->getData(), 30);
        bytesReceived += strlen(received_packet->getData());


        cout << "GOING INTO IF STATEMENT" << endl;

        if (packetSequenceNum == packet_received + 1) {
            packet_received++;
            sendto(sockfd, ackPacketMsg, 64, 0, (struct sockaddr *) &cli_addr, clilen);

            //offset = 30 * s;
            //s += 1;

            /*
            int offset = 30 * packetSequenceNum;
            //memcpy( destination, source, num)
            memcpy(&buffer[offset], received_packet->getData(), 30);
            if (packetSequenceNum == 6)
                break;
            */

        }







        // After the server has received all data packets and an EOT from the client,
        // it should send an EOT
        // packet with the type field set to 2, and then exit.
        // EOT end of transmission type field == 2
        // ACK packet == 0
        // Data Packet == 1

        //FILE* fp = fopen( "Output.txt", "wa");


        //DELETE THIS LATER
        if (packetSequenceNum > 12)
            break;





    }


    cout << "End Of Transmission Packet Received..." << endl;
    cout << "Attempting to write to file.." << endl;
    fwrite(fileContents, sizeof(char), bytesReceived, fp);
    cout << "Closing File Object" << endl;
    fclose(fp);
    cout << "Closing Socket..." << endl;
    close(sockfd);
    cout << "Gracefully Shutting Down..." << endl;
    return 0;


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



















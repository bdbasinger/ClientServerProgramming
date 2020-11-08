//Brennan Basinger 
//bdb264


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



using namespace std;


void error(const char *msg)
{
    perror(msg);
    exit(1);
}








int main(int argc, char *argv[])
{
    int sockfd = 0;
    int newsockfd, portno, b;


    char buffer[513];



    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;

    socklen_t clilen = sizeof(cli_addr);

    int recMsgSize;
    int packet_received = -1;
    int packetLength = 30;
    int packetType;
    int packetSequenceNum = 0;

    if (argc < 2)
    {
        cout << "Error! Port Number Required." << endl;
        exit(1);
     }

    ofstream log("arrival.log", ios_base::out | ios_base::trunc);

    portno = atoi(argv[1]);


    //bzero((char *) &serv_addr, sizeof(serv_addr));
    //bzero(buffer, 512);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        cout << "Failed to create socket" << endl;
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
    {
        cout << "Failed to Bind" << endl;
    }

    FILE* fp = fopen( "output.txt", "wa");
    cout << "Creating File: output.txt" << endl;


    int bytesReceived = 0;
    char fileContents[512];
    memset(fileContents, 0, sizeof(fileContents));
    fileContents[511] = 0;
    char payload[30];

    while (1)
    {
        //memset(buffer, 0, sizeof(buffer));
        memset(payload, 0, sizeof(payload));
        payload[29] = 0;

        recMsgSize = recvfrom(sockfd, &payload, sizeof(payload), 0,
                (struct sockaddr *) &cli_addr, &clilen);

        //memcpy( destination, source, num)
        memcpy(fileContents + bytesReceived, payload, 30);
        bytesReceived += recMsgSize;

        //cout << payload << endl;

        cout << endl;
        cout << endl;

        cout << "Bytes Received: " << recMsgSize << endl;

        //packet *received_packet = new packet(0,0,packetLength, buffer);
        packet *received_packet = new packet(1, 0, 30, payload);

        received_packet->deserialize(payload);
        // type = t; seqnum = s; length = l; data = d;

        //cout << "Writing to Log: " << endl;
        log << received_packet->getSeqNum() << endl;

        received_packet->printContents();

        packetType = received_packet->getType();
        packetLength = received_packet->getLength();
        packetSequenceNum = received_packet->getSeqNum();

        //log << "Sequence Number for packet received: " << received_packet->getSeqNum() << endl;




        cout << "GOING INTO IF STATEMENT" << endl;

        if (packetSequenceNum == packet_received + 1)
        {
            packet_received++;

            //int offset = 30 * packetSequenceNum;
            //memcpy(&buffer[offset], received_packet->getData(),30);

            received_packet->printContents();

            //packet *ackPacket = new packet(0, packetSequenceNum, packetLength, 0);
            packet *ackPacket = new packet(0, 0, 0, NULL);
            cout << "Sending ACK" << endl;
            sendto(sockfd, &ackPacket, sizeof(ackPacket), 0,
                    (struct sockaddr *) &cli_addr, clilen);
            if (packetSequenceNum == 6)
                break;

        }




        else if(packetType == 2)
        {
            cout << "EOT" << endl;


        }

        // type = t;
        // seqnum = s;
        // length = l;
        // data = d;


        //After the server has received all data packets and an EOT from the client,
        // it should send an EOT
        //packet with the type field set to 2, and then exit.


        // EOT end of transmission type field == 2
        // ACK packet == 0
        // Data Packet == 1


        //FILE* fp = fopen( "Output.txt", "wa");



        //tot += b;











    }


    fwrite(fileContents, sizeof(char), bytesReceived, fp);
    fclose(fp);
    close(sockfd);
    return 0;


















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


cout << "End Of Program" << endl;


}
















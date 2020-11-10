// Brennan Basinger
// bdb264


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include <fstream>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include "packet.h"
#include "packet.cpp"
#include <math.h>
#include <cerrno>



#define TIMEOUT 2
#define ATTEMPTLIMIT 20 // set max attempts to send packet
#define packetLen 40



using namespace std;

void error(const char *msg);
int max (int x, int y);
int min(int x, int y);
off_t GetFileLength(std::string const& filename);


int main(int argc, char *argv[]) {

    int sockfd = 0;
    int portno;
    struct sockaddr_in serv_addr; // Change back to serv_addr fromAddr

    struct hostent *s;


    int nPackets = 0; // Number of packets to send CHANGE
    int packet_received = -1; // highest ack received CHANGE
    int packet_sent = -1; // highest packet sent CHANGE
    int len; // Size of received datagram CHANGE
    int chunkSize = 30; //CHANGE
    int wSize = 7;
    int flag = 1;
    unsigned int size;
    int packetLength = 30;
    int packetSequenceNum = 0;
    int Attempt = 0;
    int base = 0;


    // Create Logs
    ofstream sequenceNumberLog("clientseqnum.log", ios_base::out | ios_base::trunc);
    ofstream ackLog("clientack.log", ios_base::out | ios_base::trunc);

    // Confirm user specifies 3 arguments when executing program
    if (argc < 4) {
        cout << "Required Arguments: <hostname> <port number> <file name>" << endl;
        exit(0);
    }
    // atoi string to integer: port number specified by command line argument
    portno = atoi(argv[2]);
    s = gethostbyname(argv[1]);

    //Create Datagram Socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        cout << "Failed to Create Socket." << endl;
        return 0;
    }

    if (sockfd < 0)
        error("ERROR opening socket");

    if (s == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    int fileLength = GetFileLength(argv[3]);

    // Client opens file specified by Command Line argument
    FILE *fp = fopen(argv[3], "rb");

    if (fp == NULL) {
        cout << "Error" << endl;
        perror("File");
        return 0;
    }

    // buffer that contains the data from the file


    // buffer that is sent
    char payloadBuffer[1024];
    char payloadA[512]; // buffer containing payload data
    payloadA[511] = '\0';


    // Reads from file specified until end of file is reached
    // And writes the data to the payloadA buffer
    while (!feof(fp))
    {
        fread(&payloadA, sizeof(payloadA), 1, fp);
    }

    nPackets = fileLength / 30;
    cout << "Number of Packets: " << nPackets << endl;

    if (fileLength % 30)
        nPackets++;



    // serv_addr == server

    //Make address for server

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    //serv_addr.sin_addr.s_addr = INADDR_ANY;
    bcopy((char *)s->h_addr, (char *)&serv_addr.sin_addr.s_addr,s->h_length);

    socklen_t slen = sizeof(serv_addr);

    // set the payload data that we're gonna send to 0
    //memset(payloadBuffer, 0, sizeof(payloadBuffer));

    // WAS LIKE THIS
    //memcpy(payloadBuffer, sendBuffer, packetLength);
    // CHANGED TO THIS
    //memcpy(payloadBuffer, payloadA, sizeof(payloadA));

    //changed sendBuffer to payloadA

    int i = 0;
    char spacketA[packetLen];
    packet *mySendPacket = new packet(1, packetSequenceNum, strlen(payloadA), payloadA );
    size_t dest_size = sizeof(spacketA);


    while ((Attempt < ATTEMPTLIMIT) && (packet_received < nPackets - 1)) {
        if (flag > 0) {
            flag = 0;
            for (packetSequenceNum = 0; packetSequenceNum < wSize; packetSequenceNum++)
            {
                //Get highest packet
                packet_sent = min(max(base + packetSequenceNum, packet_sent), nPackets - 1);
                if (packetSequenceNum == nPackets) {
                    packetLength = fileLength % 30;
                }

                //ORIGINAL CODE
                //type, seq_num, length, data
/*
                if ((base + packetSequenceNum) < nPackets)
                {
                    memset(data, 0, sizeof(data));
                    memcpy(data, payloadA + i, 30);
                    data[29] = 0;
                    i = i + 30;
                    packet *spacket = new packet(1, packetSequenceNum, packetLength + 7, data);
                    spacket->printContents();

                    //memset(data, 0, sizeof(data));
                    spacket->serialize(data);

                    sendto(sockfd, data, sizeof(data), 0,(struct sockaddr *) &serv_addr, slen);
                    sequenceNumberLog << spacket->getSeqNum() << endl;
                    ackLog << spacket->getSeqNum() << endl;

                    if (packetSequenceNum >= 6)
                    {
                        //packetSequenceNum = 0;
                        return 0;
                    }

                    */
                    //char payloadA[512];
                    // memset(pointer to block of memory to fill, value to be set, number of bytes to be set)
                    // memcpy(destination array where content is to be copied, pointer to source of data to be copied,
                    // number of bytes to copy)
                    if ((base + packetSequenceNum) < nPackets)
                    {
                        /* Holds Serialized Packet
                        //char payloadA[512];
                        //payloadA[511] = '\0';
                        //memset(data, 0, sizeof(data));
                        //memcpy(data, sendBuffer + i, 30);
                        //data[29] = 0;
                        //i = i + 30;
                         */


                        //type, seq_num, length, data
                        memset(spacketA, 0, packetLen);

                        //memcpy(spacketA, payloadA + i, packetLen);
                        strncpy(spacketA, payloadA + i, packetLen);
                        spacketA[dest_size - 1] = '\0';

                        cout << endl << endl;

                        //cout << "payloadA: " << payloadA + i << endl << endl;

                        cout << "SPACKET STUFF: " << spacketA << "END OF spacket STUFF!" << endl << endl;


                        i = i + 30;
                        //cout << spacketA << endl;

                        // type seqnum length data
                        //

                        //mySendPacket->printContents();


                        mySendPacket->serialize(spacketA);
                        sendto(sockfd,spacketA, packetLen,0,(struct sockaddr *) &serv_addr, slen );


                        // sendto(sockfd, data, sizeof(data), 0,(struct sockaddr *) &serv_addr, slen);
                        // now going to send serialize and send 3 packets

                        // char payloadA[512]="123"; // payload data
                        // char spacketA[packetLen];  // for holding serialized packet
                        // memset(spacketA, 0, packetLen); // serialize the packet to be sent
                        // packet mySendPacket(1, 101, strlen(payloadA), payloadA); // make the packet to be serialized and sent
                        // mySendPacket.serialize(spacketA); // serialize so spacket contains serialized contents of mySendPacket's payload
                        // sendto(mysocket, spacketA, 50, 0, (struct sockaddr *)&server, slen);


                    }
            }
        }

        size = sizeof(serv_addr);
        alarm(TIMEOUT);

        //changed to NULL
        packet *ackPacket = new packet(0, 0, 0, NULL);

        while ((len = (recvfrom(sockfd, &ackPacket, sizeof(int) * 3, 0, (struct sockaddr *) &serv_addr,
                                &size))) < 0)
            if (errno == EINTR)
            {
                if (Attempt < ATTEMPTLIMIT)
                {
                    cout << ATTEMPTLIMIT - Attempt << endl;
                    break;
                }
                else
                    return 0;
            }
            else
                return 0;

        if (len) {
            int type = ackPacket->getType();
            int sequence_num = ackPacket->getSeqNum();
            if ((sequence_num > packet_received) && (type == 0)) {
                cout << "Received Acknowledgement" << endl;
                packet_received++;
                base = packet_received;
                if (packet_received == packet_sent) {
                    alarm(0);
                    Attempt = 0;
                    flag = 1;
                } else {
                    Attempt = 0;
                    flag = 0;
                    alarm(TIMEOUT);
                }
            }

        }

    }

        close(sockfd);
        cout << "Closing Socket" << endl;
        return 0;
}


void error(const char *msg)
{
    perror(msg);
    exit(0);
}


int max (int x, int y)
{
    if (y > x)
    {
        return y;
    }
    else
    {
        return x;
    }
}

int min(int x, int y)
{
    if(y > x)
    {
        return x;
    }
    else
    {
        return y;
    }
}


off_t GetFileLength(std::string const& filename)
{
    struct stat st;
    if (stat(filename.c_str(), &st) == -1)
    {
        throw std::runtime_error(std::strerror(errno));
    }
    return st.st_size;
}


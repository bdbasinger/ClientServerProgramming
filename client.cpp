// Authors:

// Brennan Basinger
// bdb264

// Sam Boggs
// sjb578


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
#define packetLen 32

using namespace std;

void error(const char *msg);

int max(int x, int y);

int min(int x, int y);

off_t GetFileLength(std::string const &filename);


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
    int sequence_num = 0;
    int Attempt = 0;
    int base = 0;
    char ack[512];
    ack[511] = '\0';




    //arg 1 localhost
    //arg 2 emulator port
    //arg 3 client port number
    //arg 4 file.txt


    // 1
    //Declare and Setup Connection for Emulator Port
    struct hostent *em_host;            // pointer to a structure of type hostent
    em_host = gethostbyname(argv[1]);   // host name for emulator
    if(em_host == NULL){                // failed to obtain server's name
        cout << "Failed to obtain emulator.\n";
        exit(EXIT_FAILURE);
    }

    // ******************************************************************
    // ******************************************************************


    // 2
    // Client Sets up DGRAM Socket for Sending
    int CESocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(CESocket < 0){
        cout << "Error: failed to open datagram socket.\n";
    }



    // 3
    // Setup sockaddr_in Structure For Sending
    struct sockaddr_in CE;
    socklen_t CE_length = sizeof(CE);
    bzero(&CE, sizeof(CE));
    CE.sin_family = AF_INET;
    bcopy((char *)em_host->h_addr, (char*)&CE.sin_addr.s_addr, em_host->h_length);  // both using localhost so this is fine
    char * end;
    int em_rec_port = strtol(argv[2], &end, 10);  // get emulator's receiving port and convert to int
    CE.sin_port = htons(em_rec_port);             // set to emulator's receiving port

    // ******************************************************************
    // ******************************************************************


    // 4
    // client sets up datagram socket for receiving
    int ECSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(ECSocket < 0){
        cout << "Error: failed to open datagram socket.\n";
    }


    // 5
    // set up the sockaddr_in structure for receiving
    struct sockaddr_in EC;
    socklen_t EC_length = sizeof(EC);
    bzero(&EC, sizeof(EC));
    EC.sin_family = AF_INET;
    EC.sin_addr.s_addr = htonl(INADDR_ANY);
    char * end2;
    int cl_rec_port = strtol(argv[3], &end2, 10);  // client's receiving port and convert to int
    EC.sin_port = htons(cl_rec_port);             // set to emulator's receiving port

    // 6
    // do the binding
    if (bind(ECSocket, (struct sockaddr *)&EC, EC_length) == -1){
        cout << "Error in binding.\n";
    }





















    // Create Logs
    ofstream sequenceNumberLog("clientseqnum.log", ios_base::out | ios_base::trunc);
    ofstream ackLog("clientack.log", ios_base::out | ios_base::trunc);

    // Confirm user specifies 3 arguments when executing program
    if (argc < 4) {
        cout << "Required Arguments: <hostname> <port number> <file name>" << endl;
        exit(0);
    }

    // atoi string to integer: port number specified by command line argument
    // portno = atoi(argv[3]);

    //s = gethostbyname(argv[1]);


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

    // buffer that is sent
    char payloadBuffer[1024];
    char payloadA[512]; // buffer containing payload data
    payloadA[511] = '\0';

    // Reads from file specified until end of file is reached
    // And writes the data to the payloadA buffer
    while (!feof(fp)) {
        fread(&payloadA, sizeof(payloadA), 1, fp);
    }

    nPackets = fileLength / 30;
    cout << "Number of Packets: " << nPackets << endl;

    if (fileLength % 30)
        nPackets++;

    cout << "Number of Packets: " << nPackets << endl;
    //Make address for server

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    bcopy((char *) s->h_addr, (char *) &serv_addr.sin_addr.s_addr, s->h_length);
    socklen_t slen = sizeof(serv_addr);


    //SETUP sockaddr_in structure for sending



    int i = 0;
    char spacketA[packetLen];
    size_t dest_size = sizeof(spacketA);
    int tmpSeqNum = 0;
    int count = 0;

    while ((Attempt < ATTEMPTLIMIT) && (packet_received < nPackets - 1)) {
        if (flag > 0) {
            flag = 0;

            for (packetSequenceNum = 0; packetSequenceNum < wSize; packetSequenceNum++) {


                //Get highest packet
                packet_sent = min(max(base + packetSequenceNum, packet_sent), nPackets - 1);
                if (packetSequenceNum == nPackets) {
                    packetLength = fileLength % 30;
                }
                if (count == nPackets - 1)
                {
                    cout << endl;

                    packet *eotPacket = new packet(2,0,0,0);
                    memset(spacketA, 0, packetLen);
                    spacketA[dest_size - 1] = '\0';
                    eotPacket->serialize(spacketA);
                    cout << endl << endl << "SENDING EOT PACKET: " << endl << endl << endl;
                    sendto(sockfd, spacketA, packetLen, 0, (struct sockaddr *) &serv_addr, slen);
                }

                count++;
                // memset(pointer to block of memory to fill, value to be set, number of bytes to be set)
                // memcpy(destination array where content is to be copied, pointer to source of data, num bytes to copy)
                if ((base + packetSequenceNum) < nPackets) {
                    packet *mySendPacket = new packet(1, packetSequenceNum, strlen(payloadA), payloadA);

                    //type, seq_num, length, data
                    memset(spacketA, 0, packetLen);
                    strncpy(spacketA, payloadA + i, packetLen);
                    spacketA[dest_size - 1] = '\0';

                    cout << endl << endl;

                    cout << "SPACKET: " << spacketA << " END OF SPACKET " << endl << endl;
                    i = i + 30;

                    mySendPacket->serialize(spacketA);
                    sendto(sockfd, spacketA, packetLen, 0, (struct sockaddr *) &serv_addr, slen);


                    // sendto(sockfd, data, sizeof(data), 0,(struct sockaddr *) &serv_addr, slen);
                    // now going to send serialize and send 3 packets
                    // char payloadA[512]="123"; // payload data
                    // char spacketA[packetLen];  // for holding serialized packet
                    // memset(spacketA, 0, packetLen); // serialize the packet to be sent
                    // packet mySendPacket(1, 101, strlen(payloadA), payloadA);
                    // make the packet to be serialized and sent
                    // mySendPacket.serialize(spacketA);
                    // serialize so spacket contains serialized contents of mySendPacket's payload
                    // sendto(mysocket, spacketA, 50, 0, (struct sockaddr *)&server, slen);
                    cout << endl << endl;
                    cout << "COUNT COUNT COUNT COUNT: " << count << endl << endl;


                }
                //if (count >= nPackets)
                    //packetSequenceNum = 1000;
            }
        }

        size = sizeof(serv_addr);
        alarm(TIMEOUT);
        packet *ackPacket = new packet(0, 0, 0, NULL);
        while ((len = (recvfrom(sockfd, ack, 64, 0, (struct sockaddr *) &serv_addr,
                                &size))) < 0)

            if (errno == EINTR) {
                if (Attempt < ATTEMPTLIMIT) {
                    cout << ATTEMPTLIMIT - Attempt << endl;
                    break;
                } else
                    return 0;
            } else
                return 0;

        if (len) {
            int type = ackPacket->getType();
            sequence_num++;
            if ((sequence_num > packet_received) && (type == 0)) {
                cout << "Received Acknowledgement" << endl;
                packet_received++;
                base = packet_received;
                if (packet_received == packet_sent) {
                    alarm(0);
                    Attempt = 0;
                    flag = 1;
                    //if((sequence_num >= 5) && (sequence_num < nPackets))
                    //{
                    //wSize++;
                    //tmpSeqNum = 7;
                    //}
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

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int max(int x, int y) {
    if (y > x) {
        return y;
    } else {
        return x;
    }
}

int min(int x, int y) {
    if (y > x) {
        return x;
    } else {
        return y;
    }
}


off_t GetFileLength(std::string const &filename) {
    struct stat st;
    if (stat(filename.c_str(), &st) == -1) {
        throw std::runtime_error(std::strerror(errno));
    }
    return st.st_size;
}


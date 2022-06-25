#include <iostream>
#include "sockpp/tcp_acceptor.h"
#include "sockpp/version.h"
#include "logpp.h"
#include <thread>
#include <string>
#include <sstream>
#include <types.hh>
#include <fstream>
#include <vector>

using namespace std;
using namespace logpp;

static string sockaddr_to_string(sockpp::sock_address_any addr)
{
    ostringstream oss;
    oss << addr;
    return oss.str();
}

enum State
{
    HANDSHAKE = 0x00,
    STATUS = 0x01,
    LOGIN = 0x02
};

void handle_con(sockpp::tcp_socket sock)
{
    Logger logger("Sock");
    State state = HANDSHAKE;

    auto addr_str = sockaddr_to_string(sock.peer_address());
    ssize_t n;
    char buf[512];
    // Read packets
    try
    {
        while ((n = sock.read(buf, sizeof(buf))) > 0)
        {
            auto packetLength = readVarInt(buf);
            auto packetID = readVarInt(buf, packetLength.len + 1);
            switch (state)
            {
            case HANDSHAKE:
            {
                if (packetID.num == 0x00)
                {
                    cout << "handshake paq" << endl;
                    auto protVer = readVarInt(buf, packetID.len + 1);
                    auto serverAddress = readUTF8String(buf, protVer.len + 1);
                    auto serverPort = readUShort(buf, serverAddress.len + 1);
                    auto nextState = readVarInt(buf, serverAddress.len + 3);
                    state = State(nextState.num);
                }
                break;
            }
            case STATUS:
            {
                switch (packetID.num)
                {
                case 0x00:
                {
                    // REQUEST PACKET
                    cout << "req paq" << endl;
                    // send response
                    auto pid = encodeVarInt(0x00);
                    string json = "{'version':{'name':'1.8.7','protocol':47},'players':{'max':5,'online':1,'sample':[]},'description':{'text':'Hello'},'favicon':''}";
                    auto jsonString = encodeUTF8String(json);
                    auto jsonLength = encodeVarInt(jsonString.len);
                    auto lengthAll = encodeVarInt(pid.len + jsonString.len + jsonLength.len);
                    int len = pid.len + jsonString.len + jsonLength.len + lengthAll.len;
                    char packet[len];
                    memset(packet, 0, len);
                    for (int i = 0; i < lengthAll.len; i++) packet[i] = lengthAll.enc[i];
                    for (int i = 0; i < pid.len; i++) packet[i + lengthAll.len] = pid.enc[i];
                    for (int i = 0; i < jsonLength.len; i++) packet[i + lengthAll.len + pid.len] = jsonLength.enc[i];
                    for (int i = 0; i < jsonString.len; i++) packet[i + lengthAll.len + pid.len + jsonLength.len] = jsonString.str[i];
                    sock.write_n(packet, len);
                }
                }
            }
            case LOGIN:
            {
            }
            }
        }
    }
    catch (const char *msg)
    {
        logger.err(msg);
    }
    logger.info("Closing connection from " + addr_str);
}

int main(void)
{
    Logger logger("Main");
    logger.info("Starting Proxy");
    logger.info("Using sockpp version " + sockpp::SOCKPP_VERSION);
    int16_t port = 25565;
    sockpp::tcp_acceptor acc(port);
    if (!acc)
        cerr << acc.last_error_str() << endl;

    while (true)
    {
        // Accept a new client connection
        sockpp::tcp_socket sock = acc.accept();
        // Check for errors
        if (!sock)
        {
            logger.err("Error accepting incoming connection: ");
            cerr << acc.last_error_str() << endl;
        }
        else
        {
            logger.info("New connection opened from " + sockaddr_to_string(sock.peer_address()));
            // Create a thread and transfer the new stream to it.
            thread thr(handle_con, std::move(sock));
            thr.detach();
        }
    }

    return 0;
}
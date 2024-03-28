/*
Author: Bo Pang
Class:  ECE6122  2023-11-21
Description: Lab5 implementation.
*/

#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <thread>
struct TcpMessage {
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
};

sf::Packet& operator <<(sf::Packet& packet, const TcpMessage& message)
{
    return packet << message.nVersion << message.nType << message.nMsgLen << message.chMsg;
}

sf::Packet& operator >>(sf::Packet& packet, TcpMessage& message)
{
    return packet >> message.nVersion >> message.nType >> message.nMsgLen >> message.chMsg;
}
std::atomic<bool> isConnected(true);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <host> <port>\n";
        return 1;
    }

    std::string host = argv[1];
    if(host == "localhost"){
        std::cout<<"connect local"<< std::endl;
        host = "127.0.0.1";
    }
    
    unsigned short port = static_cast<unsigned short>(std::stoi(argv[2]));

    sf::TcpSocket socket;
    if (socket.connect(host, port) != sf::Socket::Done) {
        std::cerr << "Connection failed\n";
        return 1;
    }
    std::cout << "Connected to server\n";
    std::thread listeningThread([&]() {
    while (isConnected) {
        sf::Packet receivePacket;
        if (socket.receive(receivePacket) == sf::Socket::Done) {
            TcpMessage receivedMessage;
            receivePacket >> receivedMessage;
            std::cout << "Received Msg Type: " << static_cast<int>(receivedMessage.nType) 
                      << " ; Msg: " << receivedMessage.chMsg << std::endl;
        } else {
            // Handle disconnection or error if necessary
            std::cout<< "Server disconnected"<<std::endl;
            isConnected = false;
            break;
        }
    }
    });

    TcpMessage message;
    std::string input;
    while (isConnected) {
        std::cout << "Please enter command: ";
        std::getline(std::cin, input);

        if (input.empty()) {
            continue;
        }

        if (input[0] == 'q') {
            std::cout<< "COM:q"<<std::endl;
            isConnected = false;
            break;
        } 
        else if (input[0] == 'v') {
            int version = std::stoi(input.substr(2));
            std::cout<< "COM:v ,version: "<< version <<std::endl;
            message.nVersion = static_cast<unsigned char>(version);
        } 
    else if (input[0] == 't') {
    std::istringstream iss(input.substr(2));  // Create a string stream starting after 't '
    std::string token;
    std::vector<std::string> tokens;

    // Split the input into tokens based on space
    while (std::getline(iss, token, ' ')) {
        tokens.push_back(token);
    }

    if (tokens.size() == 2) {  // Expecting two tokens after 't': the type and the message
        try {
            message.nType = static_cast<unsigned char>(std::stoi(tokens[0]));  // First token is the type
            std::string msgContent = tokens[1];  // Second token is the message content
            std::strncpy(message.chMsg, msgContent.c_str(), sizeof(message.chMsg) - 1);
            message.chMsg[sizeof(message.chMsg) - 1] = '\0'; // Ensure null-termination
            message.nMsgLen = static_cast<unsigned short>(std::strlen(message.chMsg));

            std::cout << "type: " << static_cast<int>(message.nType) << " string: " << message.chMsg << std::endl;

            sf::Packet packet;
            packet << message;
            if (socket.send(packet) != sf::Socket::Done) {
                std::cerr << "Failed to send message\n";
                break;
            }
            // ... receive and handle response ...
        } 
        catch (const std::exception& e) {
            std::cerr << "Error parsing command: " << e.what() << std::endl;
        }
    } 
    else {
        std::cerr << "Invalid format for 't' command\n";
    }
    }
    }
    socket.disconnect();
    if(listeningThread.joinable()){
        listeningThread.join();
    }
    
    return 0;
}
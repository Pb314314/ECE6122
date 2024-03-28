#include <SFML/Network.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>

struct TcpMessage {
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
};

sf::Packet& operator <<(sf::Packet& packet, const TcpMessage& message) {
    return packet << message.nVersion << message.nType << message.nMsgLen << message.chMsg;
}

sf::Packet& operator >>(sf::Packet& packet, TcpMessage& message) {
    return packet >> message.nVersion >> message.nType >> message.nMsgLen >> message.chMsg;
}

std::string lastMessage;
std::mutex messageMutex;
std::vector<sf::TcpSocket*> clientSockets;
std::vector<std::thread> clientThreads;
std::mutex clientsMutex;
std::atomic<bool> isServerRunning(true);

void handleClient(sf::TcpSocket* clientSocket) {
    while (true) {
        sf::Packet packet;
        if (clientSocket->receive(packet) != sf::Socket::Done) {
            // If receiving fails, exit the loop to end the thread
            std::cout<< "a cilent leave!" <<std::endl;
            break;
        }

        TcpMessage message;
        packet >> message;

        // Check version; if not matching, ignore the message
        if (static_cast<int>(message.nVersion) != 102) {
            continue;
        }

        // Store the last received message
        {
            std::lock_guard<std::mutex> guard(messageMutex);
            lastMessage = message.chMsg;
        }

        // Process the message based on its type
        if (static_cast<int>(message.nType) == 77) {
            // Forward message to all other clients
            std::cout<< "type 77, forward!" <<std::endl;
            std::lock_guard<std::mutex> clientsGuard(clientsMutex);
            for (auto& socket : clientSockets) {
                if (socket != clientSocket) {
                    socket->send(packet);
                }
            }
        } else if (static_cast<int>(message.nType) == 201) {
            // Reverse the message and send back to the same client
            std::cout<< "type 201, resend!" <<std::endl;
            std::reverse(message.chMsg, message.chMsg + message.nMsgLen);
            sf::Packet responsePacket;
            responsePacket << message.nVersion << message.nType << message.nMsgLen << message.chMsg;
            clientSocket->send(responsePacket);
        }
    }

    // Cleanup when the client disconnects
    clientSocket->disconnect();
    {
    std::lock_guard<std::mutex> clientsGuard(clientsMutex);
    auto it = std::find(clientSockets.begin(), clientSockets.end(), clientSocket);
    if (it != clientSockets.end()) {
        clientSockets.erase(it);
    }
    }
    delete clientSocket;
}



int main() {
    const unsigned short port = 50001;
    sf::TcpListener listener;

    if (listener.listen(port) != sf::Socket::Done) {
        std::cerr << "Error: Unable to listen on port " << port << std::endl;
        return 1;
    }

    std::cout << "Server is listening on port " << port << "..." << std::endl;

    std::thread commandThread([&]() {
    std::string cmd;
    while (true) {
        std::cout << "Please enter command: ";
        std::getline(std::cin, cmd);

        if (cmd == "exit") {
            isServerRunning = false;
            listener.close();
            break;
        } else if (cmd == "msg") {
            std::lock_guard<std::mutex> guard(messageMutex);
            std::cout << "Last Message: " << lastMessage << std::endl;
        } else if (cmd == "clients") {
            std::lock_guard<std::mutex> clientsGuard(clientsMutex);
            std::cout << "Number of Clients: " << clientSockets.size() << std::endl;
            for (const auto& socket : clientSockets) {
                std::cout << "IP Address: " << socket->getRemoteAddress() 
                          << " | Port: " << socket->getRemotePort() << std::endl;
            }
        }
    }

    // Disconnect each client and clean up the sockets
    for (sf::TcpSocket* socket : clientSockets) {
        if (socket) {
            socket->disconnect();
            //delete socket;
        }
    }
});

    while (isServerRunning) {
        sf::TcpSocket* newClient = new sf::TcpSocket;
        if (listener.accept(*newClient) == sf::Socket::Done) {
            std::lock_guard<std::mutex> clientsGuard(clientsMutex);
            clientSockets.push_back(newClient); // Store the socket
            std::thread clientThread(handleClient, newClient);
            clientThreads.emplace_back(std::move(clientThread)); // Create and store the thread
        } else {
            delete newClient;
        }
    }

    for (sf::TcpSocket* socket : clientSockets) {
        if (socket) {
            //delete socket;
        }
    }
    // Wait for command thread to finish
    if (commandThread.joinable()) {
        commandThread.join();
    }

    // Join client threads
    for (std::thread& thread : clientThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    return 0;
}

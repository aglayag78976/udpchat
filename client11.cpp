// UDP Client
#include <iostream>
#include <winsock2.h>
#include <string>
using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#define SERVER "127.0.0.1"
#define BUFLEN 512
#define PORT 8888

class UDPClient {
public:
    UDPClient() {
        WSAStartup(MAKEWORD(2, 2), &ws);
        client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        server.sin_addr.S_un.S_addr = inet_addr(SERVER);
    }

    ~UDPClient() {
        closesocket(client_socket);
        WSACleanup();
    }

    void start() {
        cout << "Enter nickname: ";
        cin.getline(nickname, BUFLEN);
        cout << "Enter color (1-15): ";
        cin >> color;
        cin.ignore();

        sendto(client_socket, nickname, strlen(nickname), 0, (sockaddr*)&server, sizeof(sockaddr_in));
        char buffer[BUFLEN];

        while (true) {
            cout << "Message: ";
            cin.getline(buffer, BUFLEN);

            string full_message = string(nickname) + " (color " + std::to_string(color) + "): " + buffer;
            sendto(client_socket, full_message.c_str(), full_message.length(), 0, (sockaddr*)&server, sizeof(sockaddr_in));

            memset(buffer, 0, BUFLEN);
            int slen = sizeof(sockaddr_in);
            recvfrom(client_socket, buffer, BUFLEN, 0, (sockaddr*)&server, &slen);
            cout << buffer << "\n";
        }
    }

private:
    WSADATA ws;
    SOCKET client_socket;
    sockaddr_in server;
    char nickname[BUFLEN];
    int color;
};

int main() {
    UDPClient client;
    client.start();
    return 0;
}

#include <iostream>
#include <winsock2.h>
#include <vector>
#include <map>
#include <ctime>
using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#define BUFLEN 512
#define PORT 8888

class UDPServer {
public:
    UDPServer() {
        WSAStartup(MAKEWORD(2, 2), &wsa);
        server_socket = socket(AF_INET, SOCK_DGRAM, 0);
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(PORT);
        bind(server_socket, (sockaddr*)&server, sizeof(server));
    }

    ~UDPServer() {
        closesocket(server_socket);
        WSACleanup();
    }

    void start() {
        sockaddr_in client;
        int slen = sizeof(sockaddr_in);
        char buffer[BUFLEN];

        while (true) {
            memset(buffer, 0, BUFLEN);
            int message_len = recvfrom(server_socket, buffer, BUFLEN, 0, (sockaddr*)&client, &slen);
            string message(buffer);

            string timestamp = get_time();
            string full_message = "[" + timestamp + "] " + message;
            messages.push_back(full_message);

            if (clients.find(client.sin_addr.S_un.S_addr) == clients.end()) {
                clients[client.sin_addr.S_un.S_addr] = client;
                string join_message = "[" + timestamp + "] [Server] " + message + " joined the chat";
                messages.push_back(join_message);
                send_history(client);
            }
            broadcast_message(full_message, client);
        }
    }

private:
    WSADATA wsa;
    SOCKET server_socket;
    sockaddr_in server;
    map<unsigned long, sockaddr_in> clients;
    vector<string> messages;

    void send_history(sockaddr_in client) {
        for (const string& msg : messages) {
            sendto(server_socket, msg.c_str(), msg.length(), 0, (sockaddr*)&client, sizeof(client));
        }
    }

    void broadcast_message(const string& message, const sockaddr_in& sender) {
        for (auto& client_pair : clients) {
            sockaddr_in client = client_pair.second;
            sendto(server_socket, message.c_str(), message.length(), 0, (sockaddr*)&client, sizeof(client));
        }
    }

    string get_time() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char buffer[9];
        strftime(buffer, 9, "%H:%M:%S", ltm);
        return string(buffer);
    }
};

int main() {
    UDPServer server;
    server.start();
    return 0;
}

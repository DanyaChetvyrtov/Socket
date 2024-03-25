#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <winSock2.h>
#include <WS2tcpip.h>
#include <string>

using namespace std;

void Handler(SOCKET CurrentSocket, ADDRINFO* CurrentAddress) {
    closesocket(CurrentSocket);
    freeaddrinfo(CurrentAddress);
    WSACleanup();
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);


    WSADATA wsaData;
    ADDRINFO hints;
    ADDRINFO* addrResult = NULL;
    SOCKET ConnectSocket = INVALID_SOCKET;

    string sendBuffer;
    char recvBuffer[512];
    
    int result;

    // запуск сокетов в wind
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0) {
        cout << "WSAStartup failed, result = " << result << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    result = getaddrinfo("localhost", "444", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl;
        WSACleanup();
        return 1;
    }

    // Socket creation
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Connection 
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Unable connect to server" << endl;
        ConnectSocket = INVALID_SOCKET;
        Handler(ConnectSocket, addrResult);
        return 1;
    }


    // cout << "Sent: " << result << " bytes" << endl;

    /*
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "Shutdown error: " << result << endl;
        Handler(ConnectSocket, addrResult);
        return 1;
    }
    */

    cout << "Successfully connected.\n";
    cout << "Send something:\n";
    do {
        getline(cin, sendBuffer);
        result = send(ConnectSocket, sendBuffer.c_str(), sendBuffer.length(), 0);
        if (result == SOCKET_ERROR) {
            cout << "Send failed, error: " << result << endl;
            Handler(ConnectSocket, addrResult);
            return 1;
        }

        ZeroMemory(recvBuffer, 512);

        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "\"" << recvBuffer << "\" was received from server" << endl;
            cout << endl;
        }
        else if (result == 0) {
            cout << "Connection closed" << endl;
        }
        else {
            cout << "recv failde with error" << endl;
        }
    } while (sendBuffer != "stop_server");

    cout << "Connection closed" << endl;

    Handler(ConnectSocket, addrResult);
    return 0;
}

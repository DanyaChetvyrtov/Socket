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

// ѕроверка на валидность содержимого строки
bool isValid(string client_string) {
    string valid_symb = "0123456789 ";

    for (int i = 0; i < client_string.length(); i++) {
        if (!(valid_symb.find(client_string[i]) != string::npos))
            return false;
    }
    return true;
}

// явл€етс€ ли строка целым числом
bool isInt(string str) {

    for (int i = 0; i < str.length(); i++)
        if (!isdigit(str[i]))
            return false;

    return true;
}

// —ортировка, котора€ работает со строкой
string shellSort(string string_of_nums)
{
    int* num_array = new int[string_of_nums.length()];
    int j, array_index = 0;
    string sub_string;


    // ---ƒостаЄм числа из строки и кладЄм в массив---
    sub_string = string_of_nums[0];
    for (int i = 0; i < string_of_nums.length() - 1; i++) {
        j = i + 1;

        while (string_of_nums[j] != ' ') {

            if (j >= string_of_nums.length())
                break;
            sub_string += string_of_nums[j];
            j++;
        }

        if (isInt(sub_string)) {
            num_array[array_index] = stoi(sub_string);
            array_index++;
        }

        sub_string = "";
        i = j - 1;
    }
    // ----------------------------------------


    // ---Cортировка Ўелла---

    for (int gap = array_index / 2; gap > 0; gap /= 2)
    {
        for (int i = gap; i < array_index; i++)
        {
            int temp = *(num_array + i);

            int j;
            for (j = i; j >= gap && *(num_array + (j - gap)) > temp; j -= gap)
                *(num_array + j) = *(num_array + (j - gap));

            *(num_array + j) = temp;
        }
    }
    // -------------------------------


    string sort_result = "";

    for (int i = 0; i < array_index; i++) {
        sort_result += (to_string(num_array[i]) + " ");
    }

    delete[] num_array;

    return sort_result;
}

// ќчистка валидной строки от лишних пробелов,если они есть
string Gapd(string user_str) {
    int j;

    j = 0;
    if (user_str[0] == ' ') {
        for (int i = 0; i < user_str.length(); i++) {
            if (user_str[i] != ' ')
                break;
            j++;
        }
        user_str.erase(0, j);
    }


    j = 0;
    if (user_str[user_str.length() - 1] == ' ') {
        for (int i = user_str.length() - 1; i >= 0; i--) {
            if (user_str[i] != ' ')
                break;
            j++;
        }
        user_str.erase(user_str.length() - j, j);
    }


    while (user_str.find("  ") != string::npos) {
        for (int i = 0; i < user_str.length() - 1; i++) {
            if (user_str[i] == ' ' && user_str[i + 1] == ' ')
                user_str.erase(user_str.begin() + i);
        }
    }


    return user_str;
}


int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    WSADATA wsaData;
    ADDRINFO hints;
    ADDRINFO* addrResult = NULL;
    SOCKET ClientSocket = INVALID_SOCKET;
    SOCKET ListenSocket = INVALID_SOCKET;

    string sendBuffer;
    char recvBuffer[512];

    int result;

    result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0) {
        cout << "WSAStartup failed, result = " << result << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    // AF_INET - IPv4
    // AF_INET6 - IPv6
    hints.ai_family = AF_INET;
    // SOCK_STREAM - like TCP
    // SOCK_DGRAM  - like UDP   Уconnectionless socketsФ
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    result = getaddrinfo(NULL, "444", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl;
        WSACleanup();
        return 1;
    }

    // Socket creation
    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }


    // Connection 
    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Binding socket failed" << endl;
        ListenSocket = INVALID_SOCKET;
        Handler(ListenSocket, addrResult);
        return 1;
    }

    result = listen(ListenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        cout << endl;
        Handler(ListenSocket, addrResult);
        return 1;
    }

    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        cout << "Accepting socket failed" << endl;
        Handler(ListenSocket, addrResult);
        return 1;
    }

    closesocket(ListenSocket);
    cout << "Someone has connected" << endl;
    do {
        ZeroMemory(recvBuffer, 512);

        result = recv(ClientSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;
            if (isValid(recvBuffer)) {
                cout << "Data is valid." << endl;
                cout << "Altered data: " << shellSort(Gapd(recvBuffer)) << endl;
                sendBuffer = shellSort(Gapd(recvBuffer));
            }
            else {
                cout << "Data isn't valid." << endl;
                sendBuffer = "Data isn't valid.";
            }
            cout << endl;

            result = send(ClientSocket, sendBuffer.c_str(), sendBuffer.length(), 0);
            if (result == SOCKET_ERROR) {
                cout << "Failed to send data back" << endl;
                Handler(ClientSocket, addrResult);
                return 1;
            }
        }
        else if (result == 0) {
            cout << "Connection closing..." << endl;
        }
        else {
            cout << "recv failde with error" << endl;
            Handler(ClientSocket, addrResult);
            return 1;
        }
    } while (result > 0);

    result = shutdown(ClientSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "shutdown client socket failed" << endl;
        Handler(ClientSocket, addrResult);
        return 1;
    }

    Handler(ClientSocket, addrResult);
    return 0;
}

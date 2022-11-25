win32:
	gcc src/main.cpp src/ServerProtocol/CommunicationManager.cpp src/Networks/OSI/Windows/UDPSocket.cpp -o client.exe -Iinc -lws2_32

linux:
	gcc src/main.cpp src/ServerProtocol/CommunicationManager.cpp src/Networks/OSI/Linux/UDPSocket.cpp -o client.out -Iinc

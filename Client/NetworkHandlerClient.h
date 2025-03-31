#pragma once
#include "..\SharedResources.h"
#include "GameClient.h"



class NetworkHandlerClient
{
private:
	SOCKET Boss;
	ClientGame *Game = nullptr;

	bool IsSpectator = false;

public:
	NetworkHandlerClient(std::string Ip, int port) {
		Game = new ClientGame();  //Create client game
		//initialise winsock
		WSADATA wsData;
		WORD ver = MAKEWORD(2, 2);  //version type

		int wsok = WSAStartup(ver, &wsData);		//create wsok with version 2.2

		if (wsok != 0) {
			std::cerr << "cannot initialise Socket, Bye!" << std::endl;
			this->~NetworkHandlerClient();
			return;
		}
		//createsocket
		Boss = socket(AF_INET, SOCK_STREAM, 0);		//Boss is the server

		if (Boss == INVALID_SOCKET) {
			std::cerr << "Cannot create server Socket" << std::endl;
			this->~NetworkHandlerClient();
			return;
		}

		//fill in hint

		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port); //translates big endian to little endian
		inet_pton(AF_INET, Ip.c_str(), &hint.sin_addr);		//defines the Ip and port number of the server


		//connect to server
		int connectionResult = -1;
		while (connectionResult == -1) {
			connectionResult = connect(Boss, (sockaddr*)&hint, sizeof(hint));		//attempt to connect to the server defined in hint
			//std::cout << connectionResult;
		}
		//if (connectionResult == SOCKET_ERROR) {
		//	std::cerr << "cannot connect to server" << std::endl;
		//	this->~NetworkHandlerClient();
		//	return;
		//}
		
		GameLoop();
	}

	~NetworkHandlerClient() {  //destructor
		//closesocket(Boss);
		std::cout << "Connection terminated." << std::endl;
		closesocket(Boss);		//Closes the server socket
		WSACleanup();
	}

	//Old function, ignore
	void InputLoop() {  //depricated
		std::string userInput;
		do {
			//std::cout << "input please" << std::endl;
			std::getline(std::cin, userInput);	//ask for input
			if (userInput.size() > 0) {
				int sendResult = send(Boss, userInput.c_str(), userInput.size() + 1, 0);
				if (sendResult != SOCKET_ERROR) {
					ZeroMemory(buff, 4096);
					int bytesRecieved = recv(Boss, buff, 4096, 0);
					if (bytesRecieved > 0) {
						std::cout << "Server says: " << std::string(buff, 0, bytesRecieved) << std::endl;
					}
				}
			}

		} while (userInput.size() > 0);
		//close everything
		//closesocket(Boss);
		//WSACleanup();
	}  //depricated
	//End of old functions, pay attention again

	void GameLoop() {
		SendHello();	//Straight after a successful connection, try to send a Hello packet to the sever
		while (true) {
			ZeroMemory(buff, 4096);	//guessing this is important
			//std::cout << "Waiting for input <3" << std::endl;

			int bytesRecieved = recv(Boss, buff, 1, 0);		//waits for a packet header from the server

			if ((bytesRecieved == SOCKET_ERROR)) {
				std::cerr << "Server Lost, shutting down." << std::endl;
				this->~NetworkHandlerClient();
				break;
			}
			else if ((bytesRecieved == 0)) {
				std::cerr << "Packet lost, shutting down." << std::endl;
				this->~NetworkHandlerClient();
				break;
			}

			std::string PacketType = std::string(buff, 0, bytesRecieved);		//turns the packet header into a string
			HandleInput(PacketType);		//decides what to do with the packet header
		
		}
	}

	void HandleInput(std::string packetType) {
		//std::cout << "Handling " << packetType << " Packet" << std::endl;
		if (packetType[0] == HELLO_PACKET) {
			//shouldn't recieve!
			//Sent to server when starting
		}
		else if (packetType[0] == WELCOME_PACKET) {
			//Send to user as an acknowledgement of the hello packet
			Game->PrintTutorial();
		}
		else if (packetType[0] == START_PACKET) {
			HandleStart();
		}
		else if (packetType[0] == INPUT_PACKET) {
			//shouldn't recieve!
			//Send by the client with the chosen cards
		}
		else if (packetType[0] == RESULT_PACKET) {
			ReadGameResults();
			//Holds results of a turn after the server has calculated the input packet
		}
		else if (packetType[0] == PROMPT_PACKET) {
			SendCards();
			//Doesnt work for some reason, replaced with MOVE_PACKET \/
		}
		else if (packetType[0] == MOVE_PACKET) {
			SendCards();
			//Replaced prompt packet, tells the user to make a move, then sends the cards as a input packet
		}
		else if (packetType[0] == END_PACKET) {
			//The server is shutting down
			this->~NetworkHandlerClient();
		}
		else if (packetType[0] == CONCLUSION_PACKET) {
			HandleConclusionPacket();
		}
		else if (packetType[0] == SPECTATOR_PACKET) {
			HandleSpectatorPacket();
		}
		else {
			//hopefully this wont happen anymore
			std::cout << "seems like an invalid input..." << packetType << std::endl;
			this->~NetworkHandlerClient();
		}
	}

	void ReadGameResults() {
		int Turnresult;		//Holds the result of the turn to be passed into the game client
		char Board[16];		//Holds the board temporarily to be passed into the game client
		int temp;			//temporary holder
		int byteRecieved;	//Messages are sent as integers, so this holds that infomation to be translated

		//Board result!!
		byteRecieved = recv(Boss, buff, 16, 0);						//Recieves the second game result, the updated board.
		std::string Tempstring = std::string(buff, 0, byteRecieved);				//Translates the result into a plaintext string
		translateString(Board, Tempstring);								//Translates the result into a string using the custom Translate string function

		//Game result!!
		byteRecieved = recv(Boss, buff, 8, 0);						//Recieves the first Game result, the game result.
		Tempstring = std::string(buff, 0, byteRecieved);	//Translates the result into a plaintext string
		Turnresult = stoi(Tempstring);									//Translates the result into an integer
		//std::cout << "Game result: " << Turnresult << std::endl;

		temp = Game->RecieveResult(Turnresult, Board);					//Passes the board and game result into the gameclient
		if (Turnresult == 4 or Turnresult == 8) {						//4 is a player win, 8 is a opponent win
																		//Do more here to handle a win result
			this->~NetworkHandlerClient();
		}
	}

	void translateString(char outboard[16], std::string instring) {	//Used to convert string into a board
		for (int i = 0; i < 16; i++) {
			outboard[i] = instring[i];
		}
		//std::cout << "Test" << std::endl;
	}

	void SendCards() {
		int card1;  //temporary cards comtainer to be passed as reference
		int card2;
		
		Game->TakeTurn(card1, card2);	//Passes the cards as references

		std::string Scard1 = (std::to_string(card1));		//Translates the cards into strings to be sent
		std::string Scard2 = (std::to_string(card2));

		//Sends input packet
		send(Boss, (char*)&INPUT_PACKET, 1, 0);		
		//std::cout << "sending card1: " << Scard1 << std::endl;
		send(Boss, Scard1.c_str(), sizeof(Scard1.c_str() + 1), 0);
		//std::cout << "sending card2: " << Scard2 << std::endl;
		send(Boss, Scard2.c_str(), sizeof(Scard2.c_str() + 1), 0);

	}

	void SendHello() {
		//sends hello packet
		send(Boss, (char*)&HELLO_PACKET, 1, 0);
		send(Boss, Game->GetName().c_str(), Game->GetName().size() + 1, 0);	//Send name of Client user to server
	}

	void HandleConclusionPacket() {
		int byteRecieved;
		char result;
		ZeroMemory(buff, 4096);
		byteRecieved = recv(Boss, buff, 8, 0);						//Recieves the second game result, the updated board.
		std::string Tempstring = std::string(buff, 0, byteRecieved);	//Translates the result into a plaintext string
		result = stoi(Tempstring);
		Game->HandleWin(result);
		this->~NetworkHandlerClient();
	}

	void HandleStart() {
		int byteRecieved = recv(Boss, buff, 4096, 0);		//recieves players name
		std::string Username = std::string(buff, 0, byteRecieved);	//Translates players name to string
		Game->GameStarting(Username);
	}
	
	void HandleSpectatorPacket() {
	
		if (!IsSpectator) {
			std::cout << "The lobby you joined is full, You have been added as a spectator." << std::endl;
			IsSpectator = true;
		}
		else {
			int byteRecieved;												//Messages are sent as integers, so this holds that infomation to be translated
			byteRecieved = recv(Boss, buff, 8, 0);							//Recieves the players name;
			Game->SetOpponentName(std::string(buff, 0, byteRecieved));		//Translates the result into a plaintext string, then sets it to the opponents name
			ReadGameResults();												//Lets read game results handle the rest, treating the current player always as the opponent
		
		}
	}
};


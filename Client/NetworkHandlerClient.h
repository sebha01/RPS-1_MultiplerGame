#pragma once
#include "..\SharedResources.h"
#include "GameClient.h"

/*
----------------------------------------------------------------
						CLASS DEFINITION
----------------------------------------------------------------
*/

class NetworkHandlerClient
{
	private:
		SOCKET Boss;
		unique_ptr<ClientGame> Game = nullptr;

	public:
		NetworkHandlerClient(string Ip, int port)
		{
			Game = make_unique<ClientGame>();  //Create client game

			//initialise winsock
			WSADATA wsData;
			WORD ver = MAKEWORD(2, 2);  //version type

			int wsok = WSAStartup(ver, &wsData);		//create wsok with version 2.2
			if (wsok != 0) 
			{
				cerr << "cannot initialise Socket, Bye!" << endl;
				return;
			}

			//createsocket
			Boss = socket(AF_INET, SOCK_STREAM, 0);		//Boss is the server

			if (Boss == INVALID_SOCKET) 
			{
				cerr << "Cannot create server Socket" << endl;
				return;
			}

			sockaddr_in hint;
			hint.sin_family = AF_INET;
			hint.sin_port = htons(port); //translates big endian to little endian
			inet_pton(AF_INET, Ip.c_str(), &hint.sin_addr);		//defines the Ip and port number of the server


			//connect to server
			int connectionResult = -1;
			while (connectionResult == -1) 
			{
				connectionResult = connect(Boss, (sockaddr*)&hint, sizeof(hint));		//attempt to connect to the server defined in hint
			}
		
			GameLoop();
		}

		~NetworkHandlerClient() 
		{
			//closesocket(Boss);
			cout << "Connection terminated." << endl;
			closesocket(Boss);		//Closes the server socket
			WSACleanup();
		}

		void GameLoop() 
		{
			SendHello();	//Straight after a successful connection, try to send a Hello packet to the sever

			while (true) 
			{
				ZeroMemory(buff, 4096);	//guessing this is important

				int bytesRecieved = recv(Boss, buff, 1, 0);		//waits for a packet header from the server

				if ((bytesRecieved == SOCKET_ERROR))  
				{
					cerr << "Server Lost, shutting down." << endl;
					break;
				}
				else if ((bytesRecieved == 0)) 
				{
					cerr << "Packet lost, shutting down." << endl;
					break;
				}

				string PacketType = string(buff, 0, bytesRecieved);		//turns the packet header into a string
				HandleInput(PacketType);		//decides what to do with the packet header
		
			}
		}

		void HandleInput(string packetType) 
		{
			if (packetType[0] == HELLO_PACKET) 
			{
				//shouldn't recieve!
				//Sent to server when starting
			}
			else if (packetType[0] == WELCOME_PACKET) 
			{
				//Send to user as an acknowledgement of the hello packet
				Game->ShowWelcomeScreen();
			}
			else if (packetType[0] == START_PACKET) 
			{
				HandleStart();
			}
			else if (packetType[0] == INPUT_PACKET) 
			{
				//shouldn't recieve!
			}
			else if (packetType[0] == RESULT_PACKET) 
			{
				ReadGameResults();
				//Holds results of a turn after the server has calculated the input packet
			}
			else if (packetType[0] == PROMPT_PACKET) 
			{
				SendCards();
				//Doesnt work for some reason, replaced with MOVE_PACKET \/
			}
			else if (packetType[0] == MOVE_PACKET) 
			{
				SendCards();
				//Replaced prompt packet, tells the user to make a move, then sends the cards as a input packet
			}
			else if (packetType[0] == END_PACKET) 
			{
				return;
			}
			else if (packetType[0] == CONCLUSION_PACKET) 
			{
				HandleConclusionPacket();
			}
			else 
			{
				cerr << "seems like an invalid input..." << packetType << endl;
			}
		}

		void ReadGameResults() 
		{
			int turnResult;
			int byteReceived;

			// Receive the result of the round: the outcome of Rock, Paper, or Scissors
			byteReceived = recv(Boss, buff, sizeof(int), 0);

			if (byteReceived > 0)
			{
				string tempString = string(buff, 0, byteReceived);
				turnResult = stoi(tempString);  // Convert result to integer

				// Pass the result to the Game client
				Game->HandleWin(turnResult);  // This will update the Game client with the result
			}
			else
			{
				cerr << "Failed to receive game result." << endl;
			}
		}

		void SendCards() 
		{
			int c1, c2;
			Game->TakeTurn(c1, c2);
			int remove = Game->SelectFinalChoice(c1, c2);
			int final = (remove == 1) ? c2 : c1;

			string choiceStr = to_string(final);

			// Send the player's choice to the server
			send(Boss, (char*)&MOVE_PACKET, 1, 0);
			send(Boss, choiceStr.c_str(), choiceStr.size() + 1, 0);

		}

		void SendHello() 
		{
			//sends hello packet
			send(Boss, (char*)&HELLO_PACKET, 1, 0);
			send(Boss, Game->GetName().c_str(), Game->GetName().size() + 1, 0);	//Send name of Client user to server
		}

		void HandleConclusionPacket() 
		{
			int byteRecieved;
			char result;

			ZeroMemory(buff, 4096);
			byteRecieved = recv(Boss, buff, 8, 0);		//Recieves the second game result, the updated board.

			string Tempstring = string(buff, 0, byteRecieved);	//Translates the result into a plaintext string
			result = stoi(Tempstring);
			Game->HandleWin(result);
			this->~NetworkHandlerClient();
		}

		void HandleStart() 
		{
			int byteRecieved = recv(Boss, buff, 4096, 0);		//recieves players name
			string Username = string(buff, 0, byteRecieved);	//Translates players name to string

			Game->GameStarting(Username);
		}
};


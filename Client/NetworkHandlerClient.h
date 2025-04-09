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
		NetworkHandlerClient(string Ip, int port) 
		{
			Game = new ClientGame();  //Create client game
			//initialise winsock
			WSADATA wsData;
			WORD ver = MAKEWORD(2, 2);  //version type

			int wsok = WSAStartup(ver, &wsData);		//create wsok with version 2.2

			if (wsok != 0) {
				cerr << "cannot initialise Socket, Bye!" << endl;
				this->~NetworkHandlerClient();
				return;
			}
			//createsocket
			Boss = socket(AF_INET, SOCK_STREAM, 0);		//Boss is the server

			if (Boss == INVALID_SOCKET) {
				cerr << "Cannot create server Socket" << endl;
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
			while (connectionResult == -1) 
			{
				connectionResult = connect(Boss, (sockaddr*)&hint, sizeof(hint));		//attempt to connect to the server defined in hint
			}
		
			GameLoop();
		}

		~NetworkHandlerClient() 
		{
			cout << "Destructor Called..." << endl;
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
					this->~NetworkHandlerClient();
					break;
				}

				string PacketType = string(buff, 0, bytesRecieved);		//turns the packet header into a string
				HandleInput(PacketType);		//decides what to do with the packet header
			}
		}

		void HandleInput(string packetType) 
		{
			//cout << "Handling " << packetType << " Packet" << endl;
			if (packetType[0] == HELLO_PACKET) 
			{
				//shouldn't recieve!
				//Sent to server when starting
			}
			else if (packetType[0] == WELCOME_PACKET) 
			{
				//Send to user as an acknowledgement of the hello packet
				Game->PrintTutorial();
			}
			else if (packetType[0] == START_PACKET) 
			{
				HandleStart();
			}
			else if (packetType[0] == INPUT_PACKET) 
			{
				//shouldn't recieve!
				//Send by the client with the chosen cards
			}
			else if (packetType[0] == RESULT_PACKET) 
			{
				ReadGameResults();
				//Holds results of a turn after the server has calculated the input packet
			}
			else if (packetType[0] == PROMPT_PACKET) 
			{
				SendChoices();
				//Doesnt work for some reason, replaced with MOVE_PACKET \/
			}
			else if (packetType[0] == MOVE_PACKET) 
			{
				SendChoices();
				//Replaced prompt packet, tells the user to make a move, then sends the cards as a input packet
			}
			else if (packetType[0] == END_PACKET) 
			{
				//The server is shutting down
				SendByeMessage();
				this->~NetworkHandlerClient();
			}
			else if (packetType[0] == CONCLUSION_PACKET) 
			{
				HandleConclusionPacket();
			}
			else 
			{
				//hopefully this wont happen anymore
				cout << "seems like an invalid input..." << packetType << endl;
				SendByeMessage();
				this->~NetworkHandlerClient();
			}
		}

		void ReadGameResults()
		{
			cout << "Entered Read Game Results function" << endl;
			int Turnresult;         // Holds the result of the turn to be passed into the game client
			int byteRecieved;       // Used to store the number of bytes received

			// Recieve the game result (player's turn result)
			byteRecieved = recv(Boss, buff, 8, 0);   // Receives the game result (e.g., win/loss status)
			if (byteRecieved == SOCKET_ERROR || byteRecieved == 0)
			{
				cerr << "Error receiving game result" << endl;
				return;
			}

			// Convert the received data to a string and then to an integer
			string Tempstring = string(buff, 0, byteRecieved);
			Turnresult = stoi(Tempstring);

			// Handle the game result in the game client
			//Game->HandleResult(Turnresult);  // Pass the result directly to the game client to handle the win/loss

			// If the turn result indicates a win (e.g., 4 = win, 8 = opponent win), close the connection
			if (Turnresult == 4 || Turnresult == 8)
			{
				cout << "Closing the network via ReadGameResults function" << endl;
				SendByeMessage();
				this->~NetworkHandlerClient();  // Close the connection after a win/loss
			}

			cout << "End of Read Game Results function" << endl;
		}

		void SendChoices() 
		{
			int finalChoice;  //temporary cards comtainer to be passed as reference
		
			Game->TakeTurn(finalChoice);	//Passes the cards as references

			string SfinalChoice = (to_string(finalChoice));		//Translates the cards into strings to be sent

			//Sends input packet
			send(Boss, (char*)&INPUT_PACKET, 1, 0);		
			send(Boss, SfinalChoice.c_str(), sizeof(SfinalChoice.c_str() + 1), 0);
		}

		void SendHello() {
			//sends hello packet
			send(Boss, (char*)&HELLO_PACKET, 1, 0);
			send(Boss, Game->GetName().c_str(), Game->GetName().size() + 1, 0);	//Send name of Client user to server
		}

		void HandleConclusionPacket() 
		{
			int byteRecieved;
			int result = 0;
			ZeroMemory(buff, 4096);

			byteRecieved = recv(Boss, buff, 8, 0);						//Recieves the second game result, the updated board.
			string Tempstring = string(buff, 0, byteRecieved);	//Translates the result into a plaintext string

			result = stoi(Tempstring);
			
			//result = recv(Boss, buff, 4096, 0);
			
			cout << result << " This is at HandleConclusionPacket" << endl;
			
			Game->HandleResult(result);

			SendByeMessage();
			this->~NetworkHandlerClient();
		}

		void HandleStart() 
		{
			int byteRecieved = recv(Boss, buff, 4096, 0);		//recieves players name
			string Username = string(buff, 0, byteRecieved);	//Translates players name to string

			Game->GameStarting(Username);
		}

		void SendByeMessage()
		{
			cout << "Connection terminated." << endl;
			cout << "Disconnecting..." << endl;
			Sleep(500000);
		}
};


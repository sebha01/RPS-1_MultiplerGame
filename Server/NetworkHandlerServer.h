#pragma once
#include "..\SharedResources.h"
#include "GameServer.h"

using namespace std;

class NetworkHandlerServer
{
	private:

		//SOCKET *FocusedSocket;
		//SOCKET *UnfocusedSocket;
		CLIENT* FocusedClient;
		CLIENT* UnfocusedClient;
		GameServer* Game = nullptr;

		fd_set ListenerSpectator;
		SOCKET listener;

		int port;

	public:
		NetworkHandlerServer(int inport) 
		{
			port = inport;
			// Initialise winsock
			Game = new GameServer();					//Creates the server side of the game

			WSADATA wsData;
			WORD ver = MAKEWORD(2, 2);					//version

			int wsok = WSAStartup(ver, &wsData);		//create wsok with version 2.2

			if (wsok != 0) 
			{
				cout << "cannot initialise Socket, Bye!" << endl;
				this->~NetworkHandlerServer();
			}
			//create a listener

			listener = socket(AF_INET, SOCK_STREAM, 0);		//The listener socket is used so the server can Listen for clients trying to connect
			//The listener socket needs to be closed when all users are connected

			if (listener == INVALID_SOCKET) 
			{
				closesocket(listener);
				cout << "Cannot create listener Socket" << endl;
				this->~NetworkHandlerServer();
			}


			//bind the IP
			sockaddr_in hint;
			hint.sin_family = AF_INET;
			hint.sin_port = htons(port);					 //'htons' translates Big endian to Little endian		
			hint.sin_addr.S_un.S_addr = INADDR_ANY;			 //Finds any available address to use

			//Binds listener to Ip and port defined by the hint
			bind(listener, (sockaddr*)&hint, sizeof(hint));  //find function needs address of hint structure, cast to a socketaddress

			//Tells the listener socket to listen for clients attempting to join
			listen(listener, SOMAXCONN);					 //allows the maximum amount of connections, and sets the listener port to listen
			cout << "Server is running, waiting for clients:" << endl;


			//creates the master set, this holds sockets before they are assigned to pointers
			FD_SET master;

			FD_ZERO(&master);			//clears the master set
			FD_SET(listener, &master);	//Adds listener socket to the master set 

			int socketCount = 0;
			while (socketCount < 2) 
			{

				sockaddr_in client;						//used for collecting infomation about the client, although these are currently unused they could be printed to server for better client tracking
				int clientSize = sizeof(client);		//If these are removed, their references can be set to nullptr.
				//CLIENT NewClient;
				//NewClient.ClientSocket = accept(listener, (sockaddr*)&client, &clientSize);					//Wait for a client to attempt to connect, then set it to NEWCLIENT																//Adds the new client to the master set
				//std::cout << "Sending hello to joining client (" << NewClient.ClientSocket << ")" << std::endl;		//Prints the socket address number, not the name. This can be used for tracking

				if ((FocusedClient == nullptr) and (socketCount == 0)) 
				{
					FocusedClient = new CLIENT;
					FocusedClient->ClientSocket = accept(listener, (sockaddr*)&client, &clientSize);
					recv(FocusedClient->ClientSocket, buff, 1, 0);		//Wait for the client to send a hello packet to recieve its name
					HandleHello(*FocusedClient);

				}
				else 
				{
					UnfocusedClient = new CLIENT;
					UnfocusedClient->ClientSocket = accept(listener, (sockaddr*)&client, &clientSize);
					recv(UnfocusedClient->ClientSocket, buff, 1, 0);		//Wait for the client to send a hello packet to recieve its name
					HandleHello(*UnfocusedClient);
				}

				socketCount += 1;

			}

			//closesocket(listener);		//All clients have joined, this socket is not required anymore
			unsigned long ul = 1;
			cout << ioctlsocket(listener, FIONBIO, (unsigned long*)&ul);		//Sets the listener socket to non blocking
			ListenerSpectator.fd_array[0] = listener;
			//FocusedSocket = &master.fd_array[1];	//might randomise this
			//UnfocusedSocket = &master.fd_array[2];

			//add a new packet here to tell the users who their opponents are, and that the game is starting
			Sleep(8000);
			cout << "lobby full, starting game!" << endl;
			NotifyGameStart();
			send(FocusedClient->ClientSocket, (char*)&MOVE_PACKET, 1, 0);	//prompt the first user to start the game

			GameLoop();		//begin the game loop
		}

		~NetworkHandlerServer() 
		{
			cout << "Shutting down server..." << endl;
			send(FocusedClient->ClientSocket, (char*)&END_PACKET, 1, 0);	//Tells each client that the server is shutting down
			send(UnfocusedClient->ClientSocket, (char*)&END_PACKET, 1, 0);
			closesocket(UnfocusedClient->ClientSocket);					//Closes both sockets
			closesocket(FocusedClient->ClientSocket);
			closesocket(listener);
			WSACleanup();									//Clean up network
		}

		void GameLoop() 
		{		//The main loop of the class, handles all inputs
			while (true) 
			{
				ZeroMemory(buff, 4096);		//Unsure what this does but it feels important

				//wait for client to send a packet header
				int bytesRecieved = recv(FocusedClient->ClientSocket, buff, 1, 0);		//Holds inputted data

				if ((bytesRecieved == SOCKET_ERROR)) 
				{
					cerr << "Client disconnected, shutting down." << endl;
					this->~NetworkHandlerServer();
					break;
				}
				else if ((bytesRecieved == 0)) 
				{
					cerr << "Packet lost, shutting down." << endl;
					this->~NetworkHandlerServer();
					break;
				}

				string PacketType = string(buff, 0, bytesRecieved);	//Translates the packet header into a string
				HandleInput(PacketType);										//Decides how to handle each type of packet
			}
		}

		void HandleInput(std::string packetType) 
		{	//handles packet types
			//std::cout << "Handling " << packetType << " Packet" << std::endl;
			//std::cout << "Handling " << packetType << " Packet" << std::endl;
			if (packetType[0] == HELLO_PACKET) 
			{
				//This is now handled else where, so it should never be recieved here.
				//HandleHello();
			}
			else if (packetType[0] == WELCOME_PACKET) 
			{
				//Shouldn't recieve
				//send(*FocusedSocket, (char*)&PROMPT_PACKET, 1, 0);
				//In older versions when this system was reactionary and not turn based, joining the server would immediately start the game. This is no longer the case
			}
			else if (packetType[0] == START_PACKET) 
			{
				//This is sent by the server when the game is ready to start
				//shouldn't recieve!
			}
			else if (packetType[0] == INPUT_PACKET) 
			{
				RecievePlayerChoices();
			}
			else if (packetType[0] == RESULT_PACKET) 
			{
				//This is sent by the server to hold the results of a players input
				//Shouldn't recieve!
			}
			else if (packetType[0] == PROMPT_PACKET) 
			{
				//This doesnt work for some reason, replaced with the MOVE_PACKET
				//Shouldn't recieve!
			}
			else if (packetType[0] == MOVE_PACKET) 
			{
				//Replaces the prompt packet
				// Tells the user to make take their turn
				//Shouldn't recieve!
			}

			else if (packetType[0] == END_PACKET) 
			{
				//Means a client has disconnected or the game is over.
				this->~NetworkHandlerServer();
			}
			else if (packetType[0] == CONCLUSION_PACKET) 
			{
				//shouldn't Recieve
				//Send by the server when the game is over
			}
			else 
			{
				//Hopefully this wont happen
				cout << "seems like an invalid input..." << packetType << endl;
				this->~NetworkHandlerServer();
			}

		}

		void RecievePlayerChoices() 
		{
			int p1Choice = recv(FocusedClient->ClientSocket, buff, 8, 0); // Receive Player 1's choice
			int p2Choice = recv(UnfocusedClient->ClientSocket, buff, 8, 0); // Receive Player 2's choice

			// Convert from string to integer (or directly if data is already in integer format)
			int p1 = std::stoi(std::string(buff, 0, p1Choice));
			int p2 = std::stoi(std::string(buff, 0, p2Choice));

			// Pass the choices to GameServer
			Game->RecievePlayerChoices(p1, p2);

			// Call calculate result and send it back to clients
			int result = Game->calculateResult(p1, p2);

			// Send result back to players
			send(FocusedClient->ClientSocket, (char*)&result, sizeof(result), 0);
			send(UnfocusedClient->ClientSocket, (char*)&result, sizeof(result), 0);
		}

		void HandleWin() 
		{
			int player1Result = 1;
			int player2Result = 0;
			if (FocusedClient->points > UnfocusedClient->points) 
			{
				player1Result = 1;
				player2Result = 2;
			}
			else if (FocusedClient->points < UnfocusedClient->points) 
			{
				player1Result = 2;
				player2Result = 1;
			}
			else {
				player1Result = 0;
				player2Result = 0;
			}


			send(FocusedClient->ClientSocket, (char*)&CONCLUSION_PACKET, 1, 0);					//Send the start of a result packet
			send(UnfocusedClient->ClientSocket, (char*)&CONCLUSION_PACKET, 1, 0);					//Send the start of a result packet
			ZeroMemory(buff, 4096);
			send(FocusedClient->ClientSocket, (std::to_string(player1Result)).c_str(), 8, 0);							
			send(UnfocusedClient->ClientSocket, (std::to_string(player2Result)).c_str(), 8, 0);							
		}

		void NotifyGameStart() 
		{	//tells each user that the game is ready to start, and the name of their opponent.
			send(FocusedClient->ClientSocket, (char*)&START_PACKET, 1, 0);					//Send the start of a start packet
			send(FocusedClient->ClientSocket, UnfocusedClient->ClientName.c_str(), UnfocusedClient->ClientName.size() + 1, 0);			//sends opponents name

			send(UnfocusedClient->ClientSocket, (char*)&START_PACKET, 1, 0);
			send(UnfocusedClient->ClientSocket, FocusedClient->ClientName.c_str(), FocusedClient->ClientName.size() + 1, 0);

		}

		void HandleHello(CLIENT& Newclient) 
		{
			int byteRecieved = recv(Newclient.ClientSocket, buff, 4096, 0);		//recieves players name
			//might want to save this somehow so the clients can know who they are up against

			string Username = string(buff, 0, byteRecieved);	//Translates players name to string
			cout << "User: " << Username << " has connected to the server." << endl;
			Newclient.ClientName = Username;

			send(Newclient.ClientSocket, (char*)&WELCOME_PACKET, 1, 0);		//sends welcome packet to client
		}

		void SwapClientFocus() 
		{
			CLIENT* temp;		//Temporary socket used for swapping
			temp = UnfocusedClient;
			UnfocusedClient = FocusedClient;
			FocusedClient = temp;
			1 + 1;

			//send(*FocusedSocket, (char*)&MOVE_PACKET, 1, 0);		//handled elsewhere now
		}
};


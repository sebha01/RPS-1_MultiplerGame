#pragma once
#include "..\SharedResources.h"
#include "GameServer.h"


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

			unsigned long ul = 1;
			cout << ioctlsocket(listener, FIONBIO, (unsigned long*)&ul);		//Sets the listener socket to non blocking
			ListenerSpectator.fd_array[0] = listener;

			//add a new packet here to tell the users who their opponents are, and that the game is starting
			Sleep(5000);
			cout << "lobby full, starting game!" << endl;
			NotifyGameStart();
			send(FocusedClient->ClientSocket, (char*)&MOVE_PACKET, 1, 0);	//prompt the first user to start the game

			GameLoop();		//begin the game loop
		}

		~NetworkHandlerServer() 
		{
			cout << "NetworkHandlerServer Destructor called..." << endl;
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
				//CheckSpectatorListenerSocket();
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

		void HandleInput(string packetType) 
		{	//handles packet types
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
				RecievePlayerCards();
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
				cout << "End packet recieved..." << endl;
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

		void RecievePlayerCards() 
		{
			//Input handling
			int byteRecieved = recv(FocusedClient->ClientSocket, buff, 8, 0);			//Recieves the first card as an integer
			string choice = string(buff, 0, byteRecieved);		//Translates the recieved infomation into a string
			int ch = stoi(choice);										//Translates the string into a card

			cout << endl << byteRecieved << endl << ch << endl << choice << endl << endl;

			if (Game->getp1Decided() == false && Game->getp2Decided() == false)
			{
				Game->setPlayer1Choice(ch);
				SwapClientFocus();
				send(FocusedClient->ClientSocket, (char*)&MOVE_PACKET, 1, 0);
			}
			else if (Game->getp1Decided() == true && Game->getp2Decided() == false)
			{
				Game->setPlayer2Choice(ch);
				HandleWin();
			}
			else if (Game->getp1Decided() == true && Game->getp2Decided() == true)
			{
				HandleWin();
			}
		}

		void HandleWin() 
		{
			cout << "Start of handle win function" << endl;
			//--
			// HANDLE WIN IMPORTANT TO PROGRESS
			//--
			int player1Result = 0;
			int player2Result = 0;

			cout << "Player One Choice -> " << ChoiceToString(static_cast<Choice>(Game->getP1Choice())) << endl;
			cout << "Player Two Choice -> " << ChoiceToString(static_cast<Choice>(Game->getP2Choice())) << endl;

			if (Game->getP1Choice() == Game->getP2Choice())
			{
				player1Result = 0;
				player2Result = 0;

				cout << "Restarting round..." << endl;

				send(FocusedClient->ClientSocket, (char*)&MOVE_PACKET, 1, 0);
				send(UnfocusedClient->ClientSocket, (char*)&MOVE_PACKET, 1, 0);
			}
			else if (
				(Game->getP1Choice() == ROCK && Game->getP2Choice() == SCISSORS) ||
				(Game->getP1Choice() == PAPER && Game->getP2Choice() == ROCK) ||
				(Game->getP1Choice() == SCISSORS && Game->getP2Choice() == PAPER)
				)
			{
				player1Result = 1;
				player2Result = 2;

				cout << "Player 1 wins " << endl;

				send(FocusedClient->ClientSocket, (char*)&CONCLUSION_PACKET, 1, 0);
				send(UnfocusedClient->ClientSocket, (char*)&CONCLUSION_PACKET, 1, 0);
			}
			else
			{
				player1Result = 2;
				player2Result = 1;

				cout << "Player 2 wins" << endl;

				send(FocusedClient->ClientSocket, (char*)&CONCLUSION_PACKET, 1, 0);
				send(UnfocusedClient->ClientSocket, (char*)&CONCLUSION_PACKET, 1, 0);
			}

			ZeroMemory(buff, 4096);
			send(FocusedClient->ClientSocket, (char*)&player1Result, sizeof(int), 0);
			send(UnfocusedClient->ClientSocket, (char*)&player2Result, sizeof(int), 0);

			cout << "End of Handle Win Function" << endl;
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
		}
};


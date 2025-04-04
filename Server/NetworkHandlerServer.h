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

	CLIENT* Spectators[4];		//Allows a maximum of 4 spectators per match
	int NumOfSpectaors = 0;		//Counts the number of spectators currently watching

	int port;
public:
	NetworkHandlerServer(int inport) {
		port = inport;
		// Initialise winsock
		Game = new GameServer();					//Creates the server side of the game

		WSADATA wsData;
		WORD ver = MAKEWORD(2, 2);					//version

		int wsok = WSAStartup(ver, &wsData);		//create wsok with version 2.2

		if (wsok != 0) {
			std::cout << "cannot initialise Socket, Bye!" << std::endl;
			this->~NetworkHandlerServer();
		}
		//create a listener

		listener = socket(AF_INET, SOCK_STREAM, 0);		//The listener socket is used so the server can Listen for clients trying to connect
		//The listener socket needs to be closed when all users are connected

		if (listener == INVALID_SOCKET) {
			closesocket(listener);
			std::cout << "Cannot create listener Socket" << std::endl;
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
		std::cout << "Server is running, waiting for clients:" << std::endl;


		//creates the master set, this holds sockets before they are assigned to pointers
		FD_SET master;

		FD_ZERO(&master);			//clears the master set
		FD_SET(listener, &master);	//Adds listener socket to the master set 

		int socketCount = 0;
		while (socketCount < 2) {

			sockaddr_in client;						//used for collecting infomation about the client, although these are currently unused they could be printed to server for better client tracking
			int clientSize = sizeof(client);		//If these are removed, their references can be set to nullptr.
			//CLIENT NewClient;
			//NewClient.ClientSocket = accept(listener, (sockaddr*)&client, &clientSize);					//Wait for a client to attempt to connect, then set it to NEWCLIENT																//Adds the new client to the master set
			//std::cout << "Sending hello to joining client (" << NewClient.ClientSocket << ")" << std::endl;		//Prints the socket address number, not the name. This can be used for tracking

			if ((FocusedClient == nullptr) and (socketCount == 0)) {
				FocusedClient = new CLIENT;
				FocusedClient->ClientSocket = accept(listener, (sockaddr*)&client, &clientSize);
				recv(FocusedClient->ClientSocket, buff, 1, 0);		//Wait for the client to send a hello packet to recieve its name
				HandleHello(*FocusedClient);

			}
			else {
				UnfocusedClient = new CLIENT;
				UnfocusedClient->ClientSocket = accept(listener, (sockaddr*)&client, &clientSize);
				recv(UnfocusedClient->ClientSocket, buff, 1, 0);		//Wait for the client to send a hello packet to recieve its name
				HandleHello(*UnfocusedClient);
			}

			socketCount += 1;

		}

		//closesocket(listener);		//All clients have joined, this socket is not required anymore
		unsigned long ul = 1;
		std::cout << ioctlsocket(listener, FIONBIO, (unsigned long*)&ul);		//Sets the listener socket to non blocking
		ListenerSpectator.fd_array[0] = listener;
		//FocusedSocket = &master.fd_array[1];	//might randomise this
		//UnfocusedSocket = &master.fd_array[2];

		//add a new packet here to tell the users who their opponents are, and that the game is starting
		Sleep(5000);
		std::cout << "lobby full, starting game!" << std::endl;
		NotifyGameStart();
		send(FocusedClient->ClientSocket, (char*)&MOVE_PACKET, 1, 0);	//prompt the first user to start the game

		GameLoop();		//begin the game loop
	}

	~NetworkHandlerServer() {
		std::cout << "Shutdown!!!" << std::endl;
		send(FocusedClient->ClientSocket, (char*)&END_PACKET, 1, 0);	//Tells each client that the server is shutting down
		send(UnfocusedClient->ClientSocket, (char*)&END_PACKET, 1, 0);
		closesocket(UnfocusedClient->ClientSocket);					//Closes both sockets
		closesocket(FocusedClient->ClientSocket);
		closesocket(listener);
		for (int i = 0; i < NumOfSpectaors; i++) {
			closesocket(Spectators[i]->ClientSocket);
		}
		WSACleanup();									//Clean up network
		//delete *Game;									//Delete the game
	}

	void GameLoop() {		//The main loop of the class, handles all inputs
		while (true) {
			CheckSpectatorListenerSocket();
			ZeroMemory(buff, 4096);		//Unsure what this does but it feels important

			//wait for client to send a packet header
			int bytesRecieved = recv(FocusedClient->ClientSocket, buff, 1, 0);		//Holds inputted data

			if ((bytesRecieved == SOCKET_ERROR)) {
				std::cerr << "Client disconnected, shutting down." << std::endl;
				this->~NetworkHandlerServer();
				break;
			}
			else if ((bytesRecieved == 0)) {
				std::cerr << "Packet lost, shutting down." << std::endl;
				this->~NetworkHandlerServer();
				break;
			}

			std::string PacketType = std::string(buff, 0, bytesRecieved);	//Translates the packet header into a string
			HandleInput(PacketType);										//Decides how to handle each type of packet
		}
	}

	void HandleInput(std::string packetType) {	//handles packet types
		//std::cout << "Handling " << packetType << " Packet" << std::endl;
		//std::cout << "Handling " << packetType << " Packet" << std::endl;
		if (packetType[0] == HELLO_PACKET) {
			//This is now handled else where, so it should never be recieved here.
			//HandleHello();
		}
		else if (packetType[0] == WELCOME_PACKET) {
			//Shouldn't recieve
			//send(*FocusedSocket, (char*)&PROMPT_PACKET, 1, 0);
			//In older versions when this system was reactionary and not turn based, joining the server would immediately start the game. This is no longer the case
		}
		else if (packetType[0] == START_PACKET) {
			//This is sent by the server when the game is ready to start
			//shouldn't recieve!
		}
		else if (packetType[0] == INPUT_PACKET) {
			RecievePlayerCards();
		}
		else if (packetType[0] == RESULT_PACKET) {
			//This is sent by the server to hold the results of a players input
			//Shouldn't recieve!
		}
		else if (packetType[0] == PROMPT_PACKET) {
			//This doesnt work for some reason, replaced with the MOVE_PACKET
			//Shouldn't recieve!
		}
		else if (packetType[0] == MOVE_PACKET) {
			//Replaces the prompt packet
			// Tells the user to make take their turn
			//Shouldn't recieve!
		}

		else if (packetType[0] == END_PACKET) {
			//Means a client has disconnected or the game is over.
			this->~NetworkHandlerServer();
		}
		else if (packetType[0] == CONCLUSION_PACKET) {
			//shouldn't Recieve
			//Send by the server when the game is over
		}
		else {
			//Hopefully this wont happen
			std::cout << "seems like an invalid input..." << packetType << std::endl;
			this->~NetworkHandlerServer();
		}

	}

	void RecievePlayerCards() {
		//Input handling
		int byteRecieved = recv(FocusedClient->ClientSocket, buff, 8, 0);			//Recieves the first card as an integer
		std::string SCard1 = std::string(buff, 0, byteRecieved);		//Translates the recieved infomation into a string
		int card1 = stoi(SCard1);										//Translates the string into a card

		byteRecieved = recv(FocusedClient->ClientSocket, buff, 8, 0);				//Recieves the second card as an integer
		std::string SCard2 = std::string(buff, 0, byteRecieved);		//Translates the recieved infomation into a string
		int card2 = stoi(SCard2);										//Translates the string into a card

		//std::cout << "Recieved card1: " << card1 << std::endl;
		//std::cout << "Recieved card2: " << card2 << std::endl;

		//Output handling
		int result = Game->calculateresult(card1, card2);				//Uses the game server to calculate the result of the move
		char Map[16];													//Temporary container for sending the map
		Game->SendMap(card1, card2, Map, true);							//Recieves the map from the server, updating the map variable

		//Sends to the player that made the move
		if (result == 4) {
			HandleWin();
		}
		else {
			//std::cout << "Sending Results packet!" << std::endl;
			send(FocusedClient->ClientSocket, (char*)&RESULT_PACKET, 1, 0);					//Send the start of a result packet
			send(FocusedClient->ClientSocket, (char*)&Map, 16, 0);							//sends Map
			send(FocusedClient->ClientSocket, (std::to_string(result)).c_str(), 8, 0);		//sends game result
			if (result == 2)
			{
				FocusedClient->points++;
			}
			//Sends to the other player
			Game->SendMap(card1, card2, Map, false);							//Needs to recalculate the map to not show the opponent the cards
			send(UnfocusedClient->ClientSocket, (char*)&RESULT_PACKET, 1, 0);
			send(UnfocusedClient->ClientSocket, (char*)&Map, 16, 0);
			result += 4;														//Increases game game result to show opponent moves
			send(UnfocusedClient->ClientSocket, (std::to_string(result)).c_str(), 8, 0);

			SendSpectatorsBoard(card1, card2, result);

			SwapClientFocus();

			send(FocusedClient->ClientSocket, (char*)&MOVE_PACKET, 1, 0);			//Called AFTER swapping clients
		}
	}

	void HandleWin() {
		int player1Result = 1;
		int player2Result = 0;
		if (FocusedClient->points > UnfocusedClient->points) {
			player1Result = 1;
			player2Result = 2;
		}
		else if (FocusedClient->points < UnfocusedClient->points) {
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

	void NotifyGameStart() {	//tells each user that the game is ready to start, and the name of their opponent.

		send(FocusedClient->ClientSocket, (char*)&START_PACKET, 1, 0);					//Send the start of a start packet
		send(FocusedClient->ClientSocket, UnfocusedClient->ClientName.c_str(), UnfocusedClient->ClientName.size() + 1, 0);			//sends opponents name

		send(UnfocusedClient->ClientSocket, (char*)&START_PACKET, 1, 0);
		send(UnfocusedClient->ClientSocket, FocusedClient->ClientName.c_str(), FocusedClient->ClientName.size() + 1, 0);

	}

	void HandleHello(CLIENT& Newclient) {
		int byteRecieved = recv(Newclient.ClientSocket, buff, 4096, 0);		//recieves players name
		//might want to save this somehow so the clients can know who they are up against

		std::string Username = std::string(buff, 0, byteRecieved);	//Translates players name to string
		std::cout << "User: " << Username << " has connected to the server." << std::endl;
		Newclient.ClientName = Username;

		send(Newclient.ClientSocket, (char*)&WELCOME_PACKET, 1, 0);		//sends welcome packet to client
	}

	void SwapClientFocus() {
		CLIENT* temp;		//Temporary socket used for swapping
		temp = UnfocusedClient;
		UnfocusedClient = FocusedClient;
		FocusedClient = temp;
		1 + 1;

		//send(*FocusedSocket, (char*)&MOVE_PACKET, 1, 0);		//handled elsewhere now
	}

	void AddSpectator() {
		if (NumOfSpectaors < 1)
		{
			//std::cout << "Looking for spectators" << std::endl;
			SOCKET listener = socket(AF_INET, SOCK_STREAM, 0);		//Like before, this listener socket recieves  requests to join the lobby. 
																	//But now that the lobby is full, any new requests will be treated as spectators

			unsigned long ul = 1;



			//bind the IP
			sockaddr_in hint;
			hint.sin_family = AF_INET;
			hint.sin_port = htons(port);					 //'htons' translates Big endian to Little endian		
			hint.sin_addr.S_un.S_addr = INADDR_ANY;			 //Finds any available address to use

			sockaddr_in client;						//used for collecting infomation about the client, although these are currently unused they could be printed to server for better client tracking
			int clientSize = sizeof(client);		//If these are removed, their references can be set to nullptr.

			//Binds listener to Ip and port defined by the hint
			bind(listener, (sockaddr*)&hint, sizeof(hint));  //find function needs address of hint structure, cast to a socketaddress

			std::cout <<ioctlsocket(listener, FIONBIO, (unsigned long*)&ul);		//Sets the listener socket to non blocking
			//this listener socket needs to be non blocking so it doesnt wait for a spectator to join for the game to continue
			
			//Tells the listener socket to listen for clients attempting to join as spectators

			listen(listener, SOMAXCONN);					 //allows the maximum amount of connections, and sets the listener port to listen
			int tempResult = accept(listener, (sockaddr*)&client, &clientSize);
			
			if (tempResult != -1) {		//If temp result = -1, there is nobody waiting to join, so skip this
				Spectators[NumOfSpectaors] = new CLIENT;	//Creates a new client in the spectator array to hold the spectator
				Spectators[NumOfSpectaors]->ClientSocket = tempResult;	//binds the socket address to the newly made client
				Spectators[NumOfSpectaors]->ClientName = ("Spectator" + NumOfSpectaors);		//sets the name of the spectator
				std::cout << "New spectator has joined the lobby";		//logs the spectator joining
				send(Spectators[NumOfSpectaors]->ClientSocket, (char*)&SPECTATOR_PACKET, 1, 0);					//Send the start of a spectator packet	
																									//Can get away with only sending the start of the spectator packet so the user is told its a spectator and changes accordingly
				NumOfSpectaors++;
			}
			closesocket(listener);		//Closes the socket after each test
		}
	}

	void CheckSpectatorListenerSocket() {
		if (NumOfSpectaors < 4)
		{
			sockaddr_in client;						//used for collecting infomation about the client, although these are currently unused they could be printed to server for better client tracking
			int clientSize = sizeof(client);		//If these are removed, their references can be set to nullptr.
			int tempResult = accept(listener, (sockaddr*)&client, &clientSize);

			if (tempResult != -1) {		//If temp result = -1, there is nobody waiting to join, so skip this
				Spectators[NumOfSpectaors] = new CLIENT;	//Creates a new client in the spectator array to hold the spectator
				Spectators[NumOfSpectaors]->ClientSocket = tempResult;	//binds the socket address to the newly made client
				Spectators[NumOfSpectaors]->ClientName = ("Spectator" + NumOfSpectaors);		//sets the name of the spectator
				std::cout << "New spectator has joined the lobby";		//logs the spectator joining
				send(Spectators[NumOfSpectaors]->ClientSocket, (char*)&SPECTATOR_PACKET, 1, 0);					//Send the start of a spectator packet	
				//Can get away with only sending the start of the spectator packet so the user is told its a spectator and changes accordingly
				NumOfSpectaors++;
			}
			if (NumOfSpectaors == 1) {closesocket(listener);}
		}

		//closesocket(listener);		//Closes the socket after each test
	}
	void SendSpectatorsBoard(int card1, int card2, int result) {
		for (int i = 0; i < NumOfSpectaors; i++) {
				char Map[16];													//Temporary container for sending the map
			Game->SendMap(card1, card2, Map, true);
			send(Spectators[i]->ClientSocket, (char*)&SPECTATOR_PACKET, 1, 0);					//Send the start of a spectator_packet

			send(Spectators[i]->ClientSocket, FocusedClient->ClientName.c_str(), FocusedClient->ClientName.size() + 1, 0);	//Sends the name of the player who made the move

		
			send(Spectators[i]->ClientSocket, (char*)&Map, 16, 0);								//sends Map
			send(Spectators[i]->ClientSocket, (std::to_string(result)).c_str(), 8, 0);			//Sends game result
			

		}
	}
};


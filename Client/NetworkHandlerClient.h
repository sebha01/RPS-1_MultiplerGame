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
				//cout << connectionResult;
			}
			//if (connectionResult == SOCKET_ERROR) {
			//	cerr << "cannot connect to server" << endl;
			//	this->~NetworkHandlerClient();
			//	return;
			//}
		
			GameLoop();
		}

		~NetworkHandlerClient() 
		{  //destructor
			//closesocket(Boss);
			cout << "Connection terminated." << endl;
			closesocket(Boss);		//Closes the server socket
			WSACleanup();
		}

		////Old function, ignore
		//void InputLoop() 
		//{  //depricated
		//	string userInput;
		//	do {
		//		//cout << "input please" << endl;
		//		getline(cin, userInput);	//ask for input
		//		if (userInput.size() > 0) {
		//			int sendResult = send(Boss, userInput.c_str(), userInput.size() + 1, 0);
		//			if (sendResult != SOCKET_ERROR) {
		//				ZeroMemory(buff, 4096);
		//				int bytesRecieved = recv(Boss, buff, 4096, 0);
		//				if (bytesRecieved > 0) {
		//					cout << "Server says: " << string(buff, 0, bytesRecieved) << endl;
		//				}
		//			}
		//		}

		//	} while (userInput.size() > 0);
		//	//close everything
		//	//closesocket(Boss);
		//	//WSACleanup();
		//}  //depricated
		////End of old functions, pay attention again

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
					this->~NetworkHandlerClient();
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
				//The server is shutting down
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
				this->~NetworkHandlerClient();
			}
		}

		void ReadGameResults() 
		{
			int Turnresult;		//Holds the result of the turn to be passed into the game client
			char Board[16];		//Holds the board temporarily to be passed into the game client
			int temp;			//temporary holder
			int byteRecieved;	//Messages are sent as integers, so this holds that infomation to be translated

			//Board result!!
			byteRecieved = recv(Boss, buff, 16, 0);						//Recieves the second game result, the updated board.
			string Tempstring = string(buff, 0, byteRecieved);				//Translates the result into a plaintext string
			translateString(Board, Tempstring);								//Translates the result into a string using the custom Translate string function

			//Game result!!
			byteRecieved = recv(Boss, buff, 8, 0);						//Recieves the first Game result, the game result.
			Tempstring = string(buff, 0, byteRecieved);	//Translates the result into a plaintext string
			Turnresult = stoi(Tempstring);									//Translates the result into an integer
			//cout << "Game result: " << Turnresult << endl;

			temp = Game->RecieveResult(Turnresult, Board);					//Passes the board and game result into the gameclient
			if (Turnresult == 4 or Turnresult == 8) {						//4 is a player win, 8 is a opponent win
																			//Do more here to handle a win result
				this->~NetworkHandlerClient();
			}
		}

		void translateString(char outboard[16], string instring) 
		{	//Used to convert string into a board
			for (int i = 0; i < 16; i++) {
				outboard[i] = instring[i];
			}
			//cout << "Test" << endl;
		}

		void SendCards() 
		{
			int choice1;  //temporary cards comtainer to be passed as reference
			int choice2;
		
			Game->TakeTurn(choice1, choice2);	//Passes the cards as references

			string Scard1 = (to_string(choice1));		//Translates the cards into strings to be sent
			string Scard2 = (to_string(choice2));

			//Sends input packet
			send(Boss, (char*)&INPUT_PACKET, 1, 0);		
			//cout << "sending card1: " << Scard1 << endl;
			send(Boss, Scard1.c_str(), sizeof(Scard1.c_str() + 1), 0);
			//cout << "sending card2: " << Scard2 << endl;
			send(Boss, Scard2.c_str(), sizeof(Scard2.c_str() + 1), 0);

		}

		void SendHello() {
			//sends hello packet
			send(Boss, (char*)&HELLO_PACKET, 1, 0);
			send(Boss, Game->GetName().c_str(), Game->GetName().size() + 1, 0);	//Send name of Client user to server
		}

		void HandleConclusionPacket() 
		{
			int byteRecieved;
			char result;
			ZeroMemory(buff, 4096);

			byteRecieved = recv(Boss, buff, 8, 0);						//Recieves the second game result, the updated board.
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


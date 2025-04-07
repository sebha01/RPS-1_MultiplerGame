#pragma once
#include <iostream>
#include <algorithm>

using namespace std;

class GameServer
{
	private:
		int player1Choice = 0;
		int player2Choice = 0;
		int bestOf = 0;
		int p1Score = 0;
		int p2Score = 0;
		bool player1Win = false;
		bool player2Win = false;

		enum Choices
		{
			ROCK = 1,
			PAPER = 2,
			SCISSORS = 3
		};
	public:

		GameServer() 
		{
			player1Choice = 0;
			player2Choice = 0;
			p1Score = 0;
			p2Score = 0;
		}

		void RecievePlayerChoices(int c1, int c2) 
		{
			//This would be recieved as a packet from the client instead of a function call
			//recieve card one				
			//receive card two
			this->player1Choice = c1;
			this->player2Choice = c2;
		}

		int calculateResult(int p1Choice, int p2Choice) 
		{
			//Return 0 if tie, 1 if player 1 and 2 if player 2

			if (p1Choice == p2Choice) { return 0; };

			//Calculate winning plays for p1, if anything else p2 wins
			if ((p1Choice == ROCK && p2Choice == SCISSORS) ||
				(p1Choice == PAPER && p2Choice == ROCK) ||
				(p1Choice == SCISSORS && p2Choice == PAPER))
			{
				return 1;
			}

			//p2 wins if nothing else complete as we have deduced it is not a draw and p2 has not won
			return 2;
		}

		bool checkwin() 
		{
			if (p1Score >= bestOf / 2 + 1) 
			{
				player1Win = true;
				return true;
			}
			else if (p2Score >= bestOf / 2 + 1) 
			{
				player2Win = true;
				return true;
			}
			return false;
		}

		void SendResult(int result, CLIENT* client) 
		{
			// Send result back to the client
			send(client->ClientSocket, (char*)&result, sizeof(result), 0);
		}
};


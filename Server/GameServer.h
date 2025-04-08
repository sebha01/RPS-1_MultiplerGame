#pragma once
#include "../SharedResources.h"


class GameServer
{
	private:
		int playerOneChoice[2]; // Player 1's two choices
		int playerTwoChoice[2]; // Player 2's two choices
		bool gameOver;          // To track if the game is over
	public:

		GameServer() 
		{
			gameOver = false;
		}

		int RecieveInputOne(int card1, int card2) 
		{
			//This would be recieved as a packet from the client instead of a function call
			//recieve card one				
			//receive card two
			return calculateresult(card1, card2);
			//SendresultPlayerOne();
			return 1;
		}

		int calculateresult(int card1, int card2) 
		{
			int counter = 0;						//This resets the counter
			int returnflag = 0;
		}

		bool checkwin() 
		{
			//probably a nicer way to do this
			bool found = false;
			int i = 0;

			return not found;
		}

		void SendMap(int card1, int card2, char OutGrid[16], bool currentPlayer)	 
		{
  
		}
};


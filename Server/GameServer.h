#pragma once
#include "../SharedResources.h"


class GameServer
{
	private:
		int playerOneChoice = 0;
		int playerTwoChoice = 0;

		bool p1Decided = false;
		bool p2Decided = false;
	public:

		void setPlayer1Choice(int c)
		{
			this->playerOneChoice = c;
			p1Decided = true;
		}

		void setPlayer2Choice(int ch)
		{
			this->playerTwoChoice = ch;
			p2Decided = true;
		}

		bool getp1Decided()
		{
			return p1Decided;
		}

		bool getp2Decided()
		{
			return p2Decided;
		}

		int getP1Choice()
		{
			return playerOneChoice;
		}

		int getP2Choice()
		{
			return playerTwoChoice;
		}

		GameServer() {}

		/*
		
		
		void ResetRound()
		{
			// Reset player choices and decision flags
			playerOneChoice = 0;
			playerTwoChoice = 0;
			p1Decided = false;
			p2Decided = false;

			// Optionally reset points or other game variables if needed
			// playerOnePoints = 0;
			// playerTwoPoints = 0;

			// Optional: Notify players about the new round
			send(FocusedClient->ClientSocket, (char*)&START_PACKET, 1, 0);
			send(UnfocusedClient->ClientSocket, (char*)&START_PACKET, 1, 0);
		}
		

		*/
};


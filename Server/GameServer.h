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
		GameServer() {}
		void ResetRound();
		//Setters
		void setPlayer1Choice(int c);
		void setPlayer2Choice(int ch);
		//Getters
		bool getp1Decided();
		bool getp2Decided();
		int getP1Choice();
		int getP2Choice();	
};








/*
----------------
MEMBER FUNCTIONS
----------------
*/

void GameServer::ResetRound()
{
	// Reset player choices and decision flags
	playerOneChoice = 0;
	playerTwoChoice = 0;
	p1Decided = false;
	p2Decided = false;
}

void GameServer::setPlayer1Choice(int c)
{
	this->playerOneChoice = c;
	p1Decided = true;
}

void GameServer::setPlayer2Choice(int ch)
{
	this->playerTwoChoice = ch;
	p2Decided = true;
}

bool GameServer::getp1Decided()
{
	return p1Decided;
}

bool GameServer::getp2Decided()
{
	return p2Decided;
}

int GameServer::getP1Choice()
{
	return playerOneChoice;
}

int GameServer::getP2Choice()
{
	return playerTwoChoice;
}
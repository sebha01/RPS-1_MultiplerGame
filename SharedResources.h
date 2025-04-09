#pragma once
#include <WS2tcpip.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "windows.h"
#pragma comment (lib, "ws2_32.lib")

/*
This SharedResources.h file contains gloabal functions and libraries used by both the client and server projects
to stop repeating lines of code through the whole solution.
*/

using namespace std;

char static const	HELLO_PACKET =					'H';		//General hello
char static const	WELCOME_PACKET =				'W';		//Welcomes client to loaded server
char static const	START_PACKET =					'S';		//tells user game is ready to start			(W.I.P)
char static const	INPUT_PACKET =					'I';		//User input packet
char static const	RESULT_PACKET =					'R';		//Result of players turn
char static const	MOVE_PACKET =					'M';		//Prompts the player to take their turn
char static const	END_PACKET =					'E';		//The program has been exited.
char static const	CONCLUSION_PACKET =				'C';		//The game has ended, send results			(W.I.P)
char static const	ROUND_RESTART_PACKET =			'A';
char static const   WAIT_FOR_OTHER_CLIENT_PACKET =	'B';

char buff[4096];

struct CLIENT 
{
	SOCKET ClientSocket = 0;
	string ClientName;
	int points = 0;
};

#define MIN_CHOICE 1
#define MAX_CHOICE 3

enum Choice
{
	ROCK = 1,
	PAPER = 2,
	SCISSORS = 3
};

string ChoiceToString(Choice c)
{
	switch (c)
	{
		case ROCK:
			return "Rock";
		case PAPER:
			return "Paper";
		case SCISSORS:
			return "Scissors";
		default:
			return "";
	}
}
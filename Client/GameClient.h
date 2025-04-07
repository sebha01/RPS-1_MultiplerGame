#pragma once

#include "../SharedResources.h"

/*
----------------------------------------------------------------
						CLASS DEFINITION
----------------------------------------------------------------
*/

class ClientGame
{
	private:
		string playerName;
		string opponentName;
		int minChoice = 1;
		int maxChoice = 3;

	public:
		ClientGame() 
		{
			//char player1[30] = "lalala";				
			srand(time(NULL));							//this command is only required at the start and sets the clock

			cout << ("Please enter your name? - ");
			getline(cin, playerName);
			cout << endl;
		}
		string GetName();
		int validateInput(int minChoice, int maxChoice);
		void ShowWelcomeScreen();
		void TakeTurn(int& choice1, int& choice2);
		int SelectFinalChoice(int c1, int c2);
		void HandleWin(int win);
		void GameStarting(string inOpponent);
		void SetOpponentName(string input);
};








/*
----------------------------------------------------------------
						MEMBER FUNCTIONS
----------------------------------------------------------------
*/

string ClientGame::GetName()
{
	return playerName;
}

int ClientGame::validateInput(int minChoice, int maxChoice)
{
	int choice = 0;
	//got this validation from my pokemon game year 1 assignment, if you need me to show you ask me
	while (!(cin >> choice) || (choice < minChoice || choice > maxChoice))
	{
		cout << endl << "Invalid input, please choose a number between " << minChoice << " and " << maxChoice << " -> ";

		//clear the input and ignore any other characters on that line if user has typed in a sentence
		cin.clear();
		cin.ignore(500, '\n');
	}

	return choice;
}

void ClientGame::ShowWelcomeScreen()
{
	cout << "Welcome " << playerName << ", to Rock Paper Scissors minus One!" << endl << endl;
	cout << "Rules:" << endl;
	cout << "1: You will be matched against another player in a one-on-one round." << endl;
	cout << "2: When it is your turn, select between Rock (1), Paper (2), or Scissors (3)." << endl;
	cout << "3: Once you have chosen your first choice you will repeat step 2 to get your second choice." << endl;
	cout << "4: After both players have chosen, each player will remove one of their choices." << endl;
	cout << "5: The result of the round will be shown — win, lose, or draw." << endl;
	cout << "6: The game continues if the result is draw until a player wins to conclude the match." << endl << endl;
	cout << ("Please wait until the lobby is full") << endl;
}

void ClientGame::TakeTurn(int& choice1, int& choice2)
{
	cout << endl << "Please enter a number between 1 and 3 to select your choice:" << endl;
	cout << "1: Rock" << endl;
	cout << "2: Paper" << endl;
	cout << "3: Scissors" << endl;

	cout << " Please enter your first choice -> ";
	choice1 = validateInput(minChoice, maxChoice);

	cout << endl << " Please enter your second choice -> ";
	choice2 = validateInput(minChoice, maxChoice);
}

int ClientGame::SelectFinalChoice(int c1, int c2)
{
	cout << "Please choose one to minus: " << endl;
	cout << "1: " << ChoiceToString(c1) << endl;
	cout << "2: " << ChoiceToString(c2) << endl;
	cout << "Your choice -> ";

	int finalChoice = validateInput(1, 2);

	return finalChoice;
}


void ClientGame::HandleWin(int result)
{
	if (result == 0)
	{
		cout << "It's a draw." << endl;

	}
	else if (result == 1)
	{
		cout << "You win!" << endl;
	}
	else 
	{
		cout << opponentName << " won!" << endl;
	}
}


void ClientGame::GameStarting(string inOpponent)
{
	system("CLS");
	opponentName = inOpponent;
	cout << "You have been matched against " << inOpponent << "." << endl << "Game is starting..." << endl;
}

void ClientGame::SetOpponentName(string input) 
{
	opponentName = input;
}
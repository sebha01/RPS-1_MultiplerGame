#pragma once

#include "../SharedResources.h"

class ClientGame
{
	private:
		string playerName;
		string opponentName;
		int finalChoice = 0;
	public:
		ClientGame() 
		{
			ShowTitle();

			cout << ("Please enter your name? - ");  //this command is used to store the players name for us later on within the game

			getline(cin, playerName);
			cout << endl;
		}
		void ShowTitle();
		string GetName();
		void PrintTutorial();
		int validateInput(int minChoice, int maxChoice);
		void TakeTurn(int& finalChoice);
		void HandleResult(int Turnresult);
		void GameStarting(string inOpponent);
		void SetOpponentName(string input);
};











/*
----------------
MEMBER FUNCTIONS
----------------
*/

void ClientGame::ShowTitle()
{
	cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
	cout << setw(80) << "-- -- ROCK PAPER SCISSORS MINUS ONE -- --" << endl;
	cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
	cout << endl;
}

string ClientGame::GetName()
{
	return playerName;
}

void ClientGame::PrintTutorial()
{
	cout << "Welcome " << playerName << ", to Rock Paper Scissor Minus One!" << endl << endl;
	cout << "Rules:" << endl;
	cout << "1: You will be matched against another player" << endl;
	cout << "2: When it is your turn, enter your choice of 1-3 to choose from Rock, Paper or Scissors" << endl;
	cout << "3: You will do this twice so that you have two choices, from there you must choose to remove one of your choices." << endl;
	cout << "4: You will then have your final choice used against your opponents final choice to decide the winner." << endl;
	cout << "5: The game concludes when one player wins or loses, until then you will continue to play rounds if you draw against each other." << endl;
	cout << "6: The loser will be shot and the opposing player will be allowed to live, best of luck :)." << endl;
	cout << endl << "Please wait until the lobby is full" << endl;

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

void ClientGame::TakeTurn(int& finalChoice)
{
	cout << endl << "Please make two choices between Rock Paper and Scissors, enter a number between 1 and 3, anything else will not be accepted -> " << endl;
	cout << "1: Rock" << endl;
	cout << "2: Paper" << endl;
	cout << "3: Scissors" << endl;

	//Get first choice
	cout << endl << "Your first choice -> ";
	int choice1 = validateInput(MIN_CHOICE, MAX_CHOICE);

	//Get second choiceSD
	cout << endl << "Your second choice -> ";
	int choice2 = validateInput(MIN_CHOICE, MAX_CHOICE);

	cout << endl << "Now pick between your first and second choice to decide your final choice, the other that is not picked will be discarded ->" << endl;
	cout << "1: " << ChoiceToString(static_cast<Choice>(choice1)) << endl;
	cout << "2: " << ChoiceToString(static_cast<Choice>(choice2)) << endl;
	cout << "Decide wisely... For it could be your last :D  -> ";

	finalChoice = validateInput(1, 2);

	switch (finalChoice)
	{
	case 1:
		finalChoice = choice1;
		break;
	case 2:
		finalChoice = choice2;
		break;
	}

	cout << endl << "Your final choice is " << ChoiceToString(static_cast<Choice>(finalChoice)) << endl;

	cout << endl << "Waiting for other player to decide... " << endl;
}

void ClientGame::HandleResult(int Turnresult)
{
	cout << endl;

	// Example of what the Turnresult could represent
	if (Turnresult == 1)
	{
		cout << "You win! Congratulations! Now the other dies :)" << endl;
	}
	else if (Turnresult == 2)
	{
		cout << "You lost! Better luck next time." << endl;
		cout << "Oops!I completely forgot, there is no next time hahaha." << endl;
	}
	else if (Turnresult == 0)
	{
		cout << "It's a draw!" << endl;
	}
	else
	{
		cout << "Unexpected error" << Turnresult << endl << endl;
	}
}

void ClientGame::GameStarting(string inOpponent)
{
	startDelay(6.0, "Lobby full, connecting you to your opponent");
	system("CLS");
	opponentName = inOpponent;
	ShowTitle();
	cout << "You have been matched against " << inOpponent << "." << endl;
	startDelay(5.0, "Game is starting");
}

void ClientGame::SetOpponentName(string input)
{
	opponentName = input;
}
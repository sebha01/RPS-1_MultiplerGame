#pragma once

#include <iostream>
#include <string>

using namespace std;

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

		string GetName() 
		{ 
			return playerName;
		}

		int validateInput(int choice, int minChoice, int maxChoice)
		{
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


		void ShowWelcomeScreen() { //Call when recieve welcome packet?
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

		void TakeTurn(int& choice1, int& choice2) 
		{
			cout << endl << "Please enter a number between 1 and 3 to select your choice:" << endl;
			cout << "1: Rock" << endl;
			cout << "2: Paper" << endl;
			cout << "3: Scissors" << endl;

			cout << " Please enter your first choice -> ";	
			validateInput(choice1, minChoice, maxChoice);			

			cout << endl << " Please enter your second choice -> ";	
			validateInput(choice2, minChoice, maxChoice);


			choice1 = CalculateCard(choice1);
			choice2 = CalculateCard(choice2);
		}

		int CalculateCard(int card) {
			int part1 = (card / 10);
			int part2 = (card % 10);
			int returnval = 0;

			if ((part1 < 0) or (part1 > 4) or (part2 < 0) or (part2 > 4)) {
				returnval = -1;
			}
			else {
				returnval = ((part1 - 1) * 4) + (part2 - 1);
			}
			if ((returnval < 0) or (returnval > 15)) {
				returnval = -1;
			}
			return returnval;
		}

		int RecieveResult(int turnresult, char Localgrid[17]) {
			switch (turnresult) {		//If the player made an incorrect input, try again (This could probably be handled entirely by the player)
			case 1:
				cout << "You have already looked in one of these spaces." << endl;
				break;
			case 2:
				cout << "A MATCH!! NICE" << endl;
				//points += 1;
				break;
			case 3:
				cout << "You got nothing." << endl;
				break;
			case 4:
				cout << "All matches found" << endl;
				break;
			case 5:
			case 7:
				cout << opponentName << "  got nothing" << endl;
				break;
			case 6:
				cout << opponentName << "  got a match!" << endl;
				break;
			case 8:
				cout << opponentName << "  found the last match." << endl;
				break;
			}
			//cout << "Turn result: " << turnresult << endl;

			PrintBoard(Localgrid);

			return 0;
		}

		void PrintBoard(char Localgrid[16]) {
			int counter = 0;
			for (int x = 0; x < 16; x++)
			{
				counter++;
				cout << "|  " << Localgrid[x];	//The grid is shown using this command for testing purposes only must be removed prior to playing

				if (counter == 4)
				{
					cout << "|  " << ("\n");
					counter = 0;
				}

			}
		}

		void HandleWin(int win) {
			if (win == 0) {
				cout << "It's a draw." << endl;

			}
			else if (win == 1) {
				cout << "you win!" << endl;
			}
			else {
				cout << opponentName << " won!" << endl;
			}
		}

		void GameStarting(string inOpponent) {
			system("CLS");
			opponentName = inOpponent;
			cout << "You have been matched against " << inOpponent << "." << endl << "Game is starting..." << endl;
		}

		void SetOpponentName(string input) {
			opponentName = input;
		}
};



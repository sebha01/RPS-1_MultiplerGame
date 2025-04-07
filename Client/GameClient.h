#pragma once

#include "../SharedResources.h"

class ClientGame
{
	private:
		string playerName;
		string opponentName;

	public:
		ClientGame() {
			char player1[30] = "lalala";				//this sets the variable for name entry
			srand(time(NULL));							//this command is only required at the start and sets the clock

			cout << ("Please enter your name? - ");  //this command is used to store the players name for us later on within the game
			//cin >> playerName;
			getline(cin, playerName);
			cout << endl;
		}

		string GetName() { return playerName; }

		void PrintTutorial() { //Call when recieve welcome packet?
			cout << "Welcome" << playerName << ", to the Starlights Match Pairs" << endl;
			cout << "Rules:" << endl;
			cout << "1: When it is your turn, enter the location of two cards on the board" << endl;
			cout << "2: Use for format xy,  for instance the first card on the first row is 11, the second is 12" << endl;
			cout << "3: You will be matched against another player" << endl;
			cout << "4: You will be able to see the result of your opponents move, but not the cards themselves" << endl;
			cout << "5: The game concludes when all cards have been matched, the player with the most matches at the end wins." << endl << endl;
			cout << (" Please wait until the lobby is full") << endl;

		}

		void TakeTurn(int& card1, int& card2) {
			bool valid = false;
			while (valid == false) {
				cin.clear();
				card1 = 0;
				card2 = 0;
				string Scard1;
				string Scard2;
				while (valid == false) {
					try
					{
						cout << (" Please enter Card 1 : ");	//Enter the location of your first card
						getline(cin, Scard1);			//Displays your first card
						cout << endl << (" Please enter Card 2 : ");	//Enter the location of your next card
						getline(cin, Scard2);		//Displays your second card
						card1 = stoi(Scard1);
						card2 = stoi(Scard2);

						valid = true;

					}
					catch (const exception&)
					{
						cout << "Invalid input, please try again:" << endl << endl;
					}
				}
				valid = false;
				card1 = CalculateCard(card1);
				card2 = CalculateCard(card2);

				if ((card1 == -1) or (card2 == -1)) {
					cout << "Invalid input, please try again:" << endl << endl;
					cin.clear();
				}
				else { valid = true; }
			}


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



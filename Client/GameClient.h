#include <iostream>
#include <string>
#pragma once

using namespace std;

class ClientGame
{
	private:
		std::string playerName;
		std::string opponentName;

	public:
		ClientGame() {
			char player1[30] = "lalala";				//this sets the variable for name entry
			srand(time(NULL));							//this command is only required at the start and sets the clock

			std::cout << ("Please enter your name? - ");  //this command is used to store the players name for us later on within the game
			//std::cin >> playerName;
			std::getline(std::cin, playerName);
			std::cout << std::endl;
		}

		std::string GetName() { return playerName; }

		void PrintTutorial() { //Call when recieve welcome packet?
			std::cout << "Welcome" << playerName << ", to the Starlights Match Pairs" << std::endl;
			std::cout << "Rules:" << std::endl;
			std::cout << "1: When it is your turn, enter the location of two cards on the board" << std::endl;
			std::cout << "2: Use for format xy,  for instance the first card on the first row is 11, the second is 12" << std::endl;
			std::cout << "3: You will be matched against another player" << std::endl;
			std::cout << "4: You will be able to see the result of your opponents move, but not the cards themselves" << std::endl;
			std::cout << "5: The game concludes when all cards have been matched, the player with the most matches at the end wins." << std::endl << std::endl;
			std::cout << (" Please wait until the lobby is full") << std::endl;

		}

		void TakeTurn(int& card1, int& card2) {
			bool valid = false;
			while (valid == false) {
				std::cin.clear();
				card1 = 0;
				card2 = 0;
				std::string Scard1;
				std::string Scard2;
				while (valid == false) {
					try
					{
						std::cout << (" Please enter Card 1 : ");	//Enter the location of your first card
						std::getline(std::cin, Scard1);			//Displays your first card
						std::cout << std::endl << (" Please enter Card 2 : ");	//Enter the location of your next card
						std::getline(std::cin, Scard2);		//Displays your second card
						card1 = stoi(Scard1);
						card2 = stoi(Scard2);

						valid = true;

					}
					catch (const std::exception&)
					{
						std::cout << "Invalid input, please try again:" << std::endl << std::endl;
					}
				}
				valid = false;
				card1 = CalculateCard(card1);
				card2 = CalculateCard(card2);

				if ((card1 == -1) or (card2 == -1)) {
					std::cout << "Invalid input, please try again:" << std::endl << std::endl;
					std::cin.clear();
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
				std::cout << "You have already looked in one of these spaces." << std::endl;
				break;
			case 2:
				std::cout << "A MATCH!! NICE" << std::endl;
				//points += 1;
				break;
			case 3:
				std::cout << "You got nothing." << std::endl;
				break;
			case 4:
				std::cout << "All matches found" << std::endl;
				break;
			case 5:
			case 7:
				std::cout << opponentName << "  got nothing" << std::endl;
				break;
			case 6:
				std::cout << opponentName << "  got a match!" << std::endl;
				break;
			case 8:
				std::cout << opponentName << "  found the last match." << std::endl;
				break;
			}
			//std::cout << "Turn result: " << turnresult << std::endl;

			PrintBoard(Localgrid);

			return 0;
		}

		void PrintBoard(char Localgrid[16]) {
			int counter = 0;
			for (int x = 0; x < 16; x++)
			{
				counter++;
				std::cout << "|  " << Localgrid[x];	//The grid is shown using this command for testing purposes only must be removed prior to playing

				if (counter == 4)
				{
					std::cout << "|  " << ("\n");
					counter = 0;
				}

			}
		}

		void HandleWin(int win) {
			if (win == 0) {
				std::cout << "It's a draw." << std::endl;

			}
			else if (win == 1) {
				std::cout << "you win!" << std::endl;
			}
			else {
				std::cout << opponentName << " won!" << std::endl;
			}
		}

		void GameStarting(std::string inOpponent) {
			system("CLS");
			opponentName = inOpponent;
			std::cout << "You have been matched against " << inOpponent << "." << std::endl << "Game is starting..." << std::endl;
		}

		void SetOpponentName(std::string input) {
			opponentName = input;
		}
};



#pragma once
#include <iostream>
#include <algorithm>
class GameServer
{
private:
	char grid[16] = { 'a', 'a', 'b', 'b', 'c', 'c', 'd', 'd', 'e', 'e', 'f', 'f', 'g', 'g', 'h', 'h' };  //this should be unaccessable to the client, only the server knows the board state
public:

	GameServer() {
		//randomise Grid!
		std::random_shuffle(&grid[0], &grid[16]);

	}

	int RecieveInputOne(int card1, int card2) {
		//This would be recieved as a packet from the client instead of a function call
		//recieve card one				
		//receive card two
		return calculateresult(card1, card2);
		//SendresultPlayerOne();
		return 1;
	}

	int calculateresult(int card1, int card2) {
		int counter = 0;						//This resets the counter
		int returnflag = 0;
		if ((grid[card1] == 'x') || (grid[card2] == 'x'))		//This 'if statment' states if you have made the choice 
		{
			returnflag = 1;//cout << ("\nUnfortunately you have selected a card already matched, please try again\n\n");

			//try again
		}
		else if ((grid[card1] == grid[card2]) && (grid[card1] != 'x') && (grid[card2] != 'x')) //This is the 'if statment' that will match the pairs and then mark them as 'x' 
		{
			//cout << ("\nCongratulations you have found a pair!!\n");
			grid[card1] = 'x';
			grid[card2] = 'x';
			returnflag = 2;
			//pair
		}
		else if (((grid[card1] != grid[card2]) && (grid[card1] != 'x') && (grid[card2] != 'x'))) //This 'if statment' states if the guesses are incorrect
		{
			returnflag = 3;//cout << ("\nOopps remember the cards as you need to try again\n\n");
			//fail
		}
		if (checkwin()) {returnflag = 4;}
		return returnflag;
	}

	bool checkwin() {
		//probably a nicer way to do this
		bool found = false;
		int i = 0;
		while ((found == false) and (i < 16)) {		//Checks through the board for anything that isnt an x(to signify its been found)
			if (grid[i] != 'x') {
				found = true;
			}
			i++;
		}
		return not found;
	}

	void SendMap(int card1, int card2, char OutGrid[16], bool currentPlayer)	 {
		//char returngrid[17];

		for (int i = 0; i < 16; i++) {
			if (grid[i] == 'x') {
				OutGrid[i] = 'x';
			}
			else if ((i == card1) or (i == card2)) {
				if (currentPlayer) {
					OutGrid[i] = grid[i];
				}
				else {
					OutGrid[i] = '?';
				}
			}
			else {
				OutGrid[i] = ' ';
			}
		}
		//Send the empty map, with cleared boxes shown
		//OutGrid = returngrid;
	}



};


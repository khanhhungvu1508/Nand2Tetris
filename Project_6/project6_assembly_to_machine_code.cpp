#include <iostream>
#include <vector>
#include <bitset>
#include <string>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#pragma warning(disable:4996)

using namespace std;

#define numBit 16

enum TYPE_CODE { LOOP_START, ADDRESS, LOOP_VARIABLE, LITERAL_VARIABLE, OPERATION };

void readFile(vector <string>& listCommand)
{
	for (int i = 0; i < listCommand.size(); i++)
	{
		string newCode = "";
		for (int j = 0; j < listCommand[i].size(); j++)
		{
			if (listCommand[i][j] == '/')
				break;
			else if (listCommand[i][j] == ' ')
				continue;
			else
				newCode += listCommand[i][j];
		}
		// Checking format @Rx with x = 0 -> 15. If yes, convert to @x
		for (int j = 0; j <= 15; j++)
		{
			string cmpCode = "@R" + to_string(j);
			if (!newCode.compare(cmpCode))
			{
				newCode.assign("@" + to_string(j));
				break;
			}
		}

		// Checking format @SCREEN, @KBD, @SP, @LCL, @ARG, @THIS, @THAT
		if (!newCode.compare("@SCREEN"))
			newCode.assign("@16384");
		else if (!newCode.compare("@KBD"))
			newCode.assign("@24576");
		else if (!newCode.compare("@SP"))
			newCode.assign("@0");
		else if (!newCode.compare("@LCL"))
			newCode.assign("@1");
		else if (!newCode.compare("@ARG"))
			newCode.assign("@2");
		else if (!newCode.compare("@THIS"))
			newCode.assign("@3");
		else if (!newCode.compare("@THAT"))
			newCode.assign("@4");
		listCommand[i] = newCode;
	}
}

bool isNumber(string ins)
{
  for (int i = 0; i < ins.size(); i++)
		if (ins[i] > '9' || ins[i] < '0')
			return false;
	return true;
}

bool isLoop(map <string, string>& binaryString, vector <string> listCommand, string ins)
{
	bool found = false;
	ins.assign("(" + ins + ")");
	int addr = 0;
	for (int i = 0; i < listCommand.size(); i++)
	{
		if (listCommand[i][0] != '(')
			addr++;
		else
		{
			if (!listCommand[i].compare(ins))
			{
				found = true;
				break;
			}
		}
	}
	if (found == true)
	{
		bitset<numBit> bSet(addr);
		string res = "";
		for (int i = numBit - 1; i >= 0; i--)
			res += to_string(bSet[i]);
		ins.erase(0, 1);
		ins.pop_back();
		binaryString['@' + ins] = res;
		return true;
	}
	return false;
}

TYPE_CODE getType(map <string, string>& binaryString, vector <string> listCommand, vector <string>& listLiteral, string instruction)
{
	if (instruction[0] == '(')
		return LOOP_START;
	if (instruction[0] == '@')
	{
		string ins = instruction.substr(1);
		if (isNumber(ins))
			return ADDRESS;
		if (isLoop(binaryString, listCommand, ins))
			return LOOP_VARIABLE;
		listLiteral.push_back("@" + ins);
		return LITERAL_VARIABLE;
	}
	return OPERATION;
}

void convertAddress(map <string, string>& binaryString, string instruction)
{
	string ins = instruction.substr(1);
	bitset<numBit> bSet(stoi(ins));
	cout << bSet;
	string res = "";
	for (int i = numBit - 1; i >= 0; i--)
		res += to_string(bSet[i]);
	binaryString[instruction] = res;
}

string getABit(string computation)
{
	return (computation.find('M') != string::npos) ? "1" : "0";
}

string getCompBits(string computation)
{
	if (!computation.compare("0"))
		return "101010";
	if (!computation.compare("1"))
		return "111111";
	if (!computation.compare("-1"))
		return "111010";
	if (!computation.compare("D"))
		return "001100";
	if (!computation.compare("M") || !computation.compare("A"))
		return "110000";
	if (!computation.compare("!D"))
		return "001101";
	if (!computation.compare("!M") || !computation.compare("!A"))
		return "110001";
	if (!computation.compare("-D"))
		return "001111";
	if (!computation.compare("-M") || !computation.compare("-A"))
		return "110011";
	if (!computation.compare("D+1"))
		return "011111";
	if (!computation.compare("M+1") || !computation.compare("A+1"))
		return "110111";
	if (!computation.compare("D-1"))
		return "001110";
	if (!computation.compare("M-1") || !computation.compare("A-1"))
		return "110010";
	if (!computation.compare("D+M") || !computation.compare("D+A"))
		return "000010";
	if (!computation.compare("D-M") || !computation.compare("D-A"))
		return "010011";
	if (!computation.compare("M-D") || !computation.compare("A-D"))
		return "000111";
	if (!computation.compare("D&M") || !computation.compare("D&A"))
		return "000000";
	if (!computation.compare("D|M") || !computation.compare("D|A"))
		return "010101";
	throw string("Invalid computation: " + computation);
}

string getDestBits(string destination)
{
	string writeM = (destination.find("M") != string::npos) ? "1" : "0";
	string writeD = (destination.find("D") != string::npos) ? "1" : "0";
	string writeA = (destination.find("A") != string::npos) ? "1" : "0";
	return writeA + writeD + writeM;
}

string getJumpBits(string jump)
{
	if (!jump.compare("000"))
		return "000";
	if (!jump.compare("JGT"))
		return "001";
	if (!jump.compare("JEQ"))
		return "010";
	if (!jump.compare("JGE"))
		return "011";
	if (!jump.compare("JLT"))
		return "100";
	if (!jump.compare("JNE"))
		return "101";
	if (!jump.compare("JLE"))
		return "110";
	if (!jump.compare("JMP"))
		return "111";
	throw string("Invalid jump: " + jump);
}

void convertOperation(map <string, string>& binaryString, string instruction)
{
	bool haveDest = (instruction.find("=") != string::npos);
	bool haveJump = (instruction.find(";") != string::npos);
	string computation, destination, jump;
	string substr;
	stringstream ss(instruction);
	if (haveDest && !haveJump)
	{
		getline(ss, substr, '=');
		destination = substr;
		getline(ss, substr, '=');
		computation = substr;
		jump = "000";
	}
	else if (!haveDest && haveJump)
	{
		getline(ss, substr, ';');
		computation = substr;
		getline(ss, substr, ';');
		jump = substr;
		destination = "";
	}
	else
	{
		getline(ss, substr, '=');
		destination = substr;
		getline(ss, substr, '=');
		stringstream ss1(substr);
		string substr2;
		getline(ss1, substr2, ';');
		computation = substr2;
		getline(ss1, substr2, ';');
		jump = substr2;

	}
	string res = "111" + getABit(computation) + getCompBits(computation) + getDestBits(destination) + getJumpBits(jump);
	cout << res;
	binaryString[instruction] = res;
}

void convertLiteralVariable(map <string, string>& binaryString, vector <string> listLiteral, string instruction)
{
	int addr = 15 + listLiteral.size();
	bitset<numBit> bSet(addr);
	cout << bSet;
	string res = "";
	for (int i = numBit - 1; i >= 0; i--)
		res += to_string(bSet[i]);
	binaryString[instruction] = res;
}

void convertMachine(string filename, const char* fileout)
{
	ifstream myfile;
	myfile.open(filename);

	vector <string> listCommand;
	map <string, string> binaryString;
	vector <string> listLiteral;

	string line = "";
	char letter;
	int numofLine = 0;
	while (myfile.get(letter))
	{
		if (letter == '\r')
			continue; // Avoid effect by window complier!
		if (letter != '\n')
			line += letter;
		else
		{
			listCommand.push_back(line);
			line.clear();
		}
	}
	listCommand.push_back(line);
	myfile.close();
	freopen(fileout, "w", stdout);
	readFile(listCommand);
	listCommand.erase(std::remove(listCommand.begin(), listCommand.end(), ""), listCommand.end());
	bool countDel = 0;
	for (int i = 0; i < listCommand.size(); i++)
	{
		if (binaryString.find(listCommand[i]) != binaryString.end())
		{
			if (countDel == 0)
				countDel = 1;
			else
				cout << endl;
			cout << binaryString[listCommand[i]];
			continue;
		}
		// Dont update!
		TYPE_CODE type = getType(binaryString, listCommand, listLiteral, listCommand[i]);
		switch (type)
		{
		case LOOP_START:
			break;
		case ADDRESS:
			if (countDel == 0)
				countDel = 1;
			else
				cout << endl;
			convertAddress(binaryString, listCommand[i]);
			break;
		case LOOP_VARIABLE:
			if (countDel == 0)
				countDel = 1;
			else
				cout << endl;
			cout << binaryString[listCommand[i]];
			break;
		case LITERAL_VARIABLE:
			if (countDel == 0)
				countDel = 1;
			else
				cout << endl;
			convertLiteralVariable(binaryString, listLiteral, listCommand[i]);
			break;
		case OPERATION:
			if (countDel == 0)
				countDel = 1;
			else
				cout << endl;
			convertOperation(binaryString, listCommand[i]);
			break;
		default:
			throw string("Invalid instruction: " + listCommand[i]);
		}
	}
}

int main()
{
	convertMachine("Add.asm", "Add.hack");
	convertMachine("Max.asm", "Max.hack");
	convertMachine("MaxL.asm", "MaxL.hack");
	convertMachine("Pong.asm", "Pong.hack");
	convertMachine("PongL.asm", "PongL.hack");
	convertMachine("Rect.asm", "Rect.hack");
	convertMachine("RectL.asm", "RectL.hack");
	return 0;
}

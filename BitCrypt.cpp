#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <cmath>
using namespace std;

//Outputs a single char to screen
void read_func(unsigned short buffer, int LETTER_SIZE = 5) {
	buffer >>= 16 - LETTER_SIZE;//Shift buffer to read position
	char out =  'a' + static_cast<int>(buffer);//Add number contained in buffer to 'a', this will result in either 'a' or a nwe letter
	if (buffer >= 26) out = ' ';//Set seperation marks to spaces
	cout << out;//Print letter or space
}

void create_code(string in_str, string fname){
	vector<unsigned char> code_out;
	
	int const LETTER_SIZE = 5;//Each char will be 5 bits when reduccd
	int const BUFFER_SIZE = 8;//Letters will be stored in 8 bit segments
	int remaining_bits = 8;//Count down to refresh buffer
	
	unsigned short in_buffer;//8 bit buffer
	
	unsigned char temp = 0;
	unsigned char mask = pow(2, LETTER_SIZE) - 1;//gets rid of extraneous bits i.e. 11111111 becomes 0011111 
	
	temp ^= temp;
	while(in_str.length() % 8 != 0){
		in_str += temp;//Fill out string lenth to prevent erroneous chars
	}
	
	temp = 0;
	for (int i = 0; i < in_str.size(); i++){
		in_buffer = mask & (in_str[i] - 1);  //Blank out uneeded bits and add to in_buffer
		if(remaining_bits - LETTER_SIZE > 0){//If there are enough remaining bits in the buffer,
			remaining_bits -= LETTER_SIZE;   //then subtract letter size from bits counter
		}
		else{//If there are not enough bits left, do the hard part
			in_buffer >>= LETTER_SIZE - remaining_bits;//shift buffer to target letter section and subtract bits that won't fit
			temp |= in_buffer;//copy the remaining bits from the target letter onto the temporary buffer
			code_out.push_back(temp);//add temp to output vector
			temp = 0;//reset temp
			remaining_bits = BUFFER_SIZE - (LETTER_SIZE - remaining_bits);//set remaining bits to BUFFER_SIZE - bits left over from last letter
			in_buffer = mask & (in_str[i] - 1); //add next letter to in_buffer
		}
		in_buffer <<= remaining_bits; //shift buffer left to open up space for next letter
		temp |= in_buffer;//copy in_buffer to temp
	}
	
	//save code to file
	ofstream output(fname);//open output file
	if(output.is_open()){//if the file opens
		for(int i = 0; i < code_out.size(); i++){
			cout << setw(3) << setfill('0') << static_cast<int>(code_out[i]) << ' ';//output raw numbers so it looks cool
			if(i % 10 == 0 && i > 1){cout << endl;}//new line every five numbers
			output << static_cast<unsigned char>(code_out[i]) << '\n';//add each number to file
		}
		output.close();//close file
		cout << endl << "Code created!" << endl;//print success message
	}
	else{
		cout << "Unable to open file!";//print error message
	}
}

void decrypt_code(string ofname){
	ifstream input(ofname);//open target file
	vector<string> str_v;
	if(input.is_open()){//if target file is open
		vector<unsigned char> code_out;//vector to hold each line
		char oc;
		
		//Build code_out vector
		while( input.get(oc)){//Loop through lines in file
			if(oc != '\n')code_out.push_back(oc);//add to decoding vector
		}
		
		//output raw contents of code_out
		for(int i = 0; i < code_out.size(); i++){
			cout <<setw(3) << setfill('0') << static_cast<int>(code_out[i]) << ' ';
			if(i % 10 == 0 && i > 1){ cout << endl;}
		}
		cout << endl;
		
		
		int const LETTER_SIZE = 5;//Each letter is 5 bits of a 16 bit char
		int const BUFFER_SIZE = 8;//Size of buffer
		unsigned short buffer = 0;//Buffer var, will hold value pulled from "bytes" 16 at a time, this is key
		int bits_remaining = 8;//Holds number of unread bits in buffer
		int pull_pos = 0;//Pull index for "bytes"
		int count = 1;
		//Go through all of bytes
		
		
		//The read portion initially loads two sections of code_out into 
		//the buffer, and then reads the first letter, after wards it loops
		//until all letters have been read.
		buffer |= code_out[pull_pos++];//initialize read
		buffer <<= BUFFER_SIZE;//shift buffer to read position
		buffer |= code_out[pull_pos++];//copy in second set of vars
		read_func(buffer);
		while(pull_pos < code_out.size() + 1){
			buffer <<= 1;//shift buffer left one space
			if(count % BUFFER_SIZE == 0){//every eight cycles, a 8 bits are added to the right side of the 16 bit buffer
				buffer |= code_out[pull_pos++];//add in next set
			}
			if(count % LETTER_SIZE == 0){//every five cycles send buffer to the read function to print a letter
				read_func(buffer);
			}
			++count;//counter for tracking when to add new letters and when to read letters
		}
	}
	cout << endl;
}

void input(int& user_input, string prompt, int min, int max = std::numeric_limits<int>::max()){
	do{
		cout << prompt;	
		if (!(cin >> user_input) || user_input < min || user_input > max){//Read input and validate
			cout << "Invalid input!\n";//Show error message
		}
		cin.clear();//Clear input buffer
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	while (!(user_input >= min) || user_input > max);
}

int main() {
	int choice;
	
	do{
		system("cls");
		cout << "Hello, welcome to TinyCrypt!" << endl;
		cout << "Would you like to: \n\t1)Encrypt\n\t2)Decrypt\n\t3)Quit" << endl;
		
		input(choice, "Enter Selection: ", 1, 3);
		switch (choice){
			case 1:{
				string in_str, fname;
				cout << "Please enter a secret message: ";
				getline(cin, in_str);
				cout << "Please enter a file name: ";
				getline(cin, fname);
				create_code(in_str, fname);
				break;
			}
			case 2:{
				string ofname;
				cout << "Please enter a filename to decrypt: ";
				getline(cin, ofname);
				
				decrypt_code(ofname);
				break;
			}
		}
		system("pause");
	}while(choice != 3);
	

	return 0;
}

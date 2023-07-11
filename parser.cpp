#include <iostream>
#include <string>
#include <vector>

//Loop through t
//Check for has delmiter appean after 
//"hello world      "

//Add check length before handle special
//look at back only
class Tokenizer{
	private :
		std::string msg;
		const char *delim;
		const char *special;

		std::vector<std::string> tokens;
		int	begin;
		int	end;
	public :
		Tokenizer(std::string &msg, char *delim, char *special){
			this->msg = msg;
			this->delim = delim;
			this->special = special;

			//end will return -1 if reading end of strign where there are no more token
			this->begin = msg.find_first_not_of(delim);
			this->end = msg.find_first_of(delim, this->begin);
		}
		//Update the begin and end value
		//Check the current string and splitout the nex totken
		void getNextToken(){

			std::string temp;

			if (begin != std::string::npos && end != std::string::npos){
				temp = msg.substr(begin, end - begin);
				this->begin = msg.find_first_not_of(delim, end);
				this->end = msg.find_first_of(delim, this->begin);
			}
			else if (begin != std::string::npos && end == std::string::npos){
				temp = msg.substr(begin, msg.length() - begin);
				this->begin = msg.find_first_not_of(delim, end);
			}
			// std::cout << "Temp: " << temp << std::endl;
			if (temp.find_first_of(special) != std::string::npos && temp.length() > 1){
				// std::cout << "Special: " << temp << std::endl;
				handleSpecial(temp);
			}
			else{

				tokens.push_back(temp);
			}
		
		}

		// void handleSpecial(std::string &current){
		// 	int pos = current.find_first_of(special);
		// 	std::string temp = current.substr(0, pos);

		// 	if (!temp.empty())
		// 		tokens.push_back(temp);
		// 	temp = current.substr(pos, 1);
		// 	if (!temp.empty())
		// 		tokens.push_back(temp);
		// 	std::string remain = current.substr(pos + 1, current.length() - pos - 1);
		// 	if (remain.find_first_of(special) != std::string::npos){
		// 		handleSpecial(remain);
		// 	}
		// }

		//Three cases
		//In front
		//Behind
		//Special char only
		void handleSpecial(std::string &current){
			//find both special char and normal
			std::cout << "Current: " << current << std::endl;
			int specialChar = current.find_first_of(this->special);
			int normalChar = current.find_first_not_of(this->special);
			std::cout << "Special: " << special << std::endl;
			std::cout << "Normal: " << normalChar << std::endl;
		}

		void Tokenize(){
			while(begin != end){
				getNextToken();
			}
		}
		void print(){
			for(std::string token:tokens)
				std::cout << token << std::endl;
		}
};


int main(){
	// std::string msg = "Hello World!!!		How    are you? Fine .    let!go";
	std::string msg = "!!LET GO!!MORe ?";

	char *delim = " \r\t";
	char *special = "?!";

	Tokenizer tokenizer(msg,delim,special);
	tokenizer.Tokenize();
	tokenizer.print();
}
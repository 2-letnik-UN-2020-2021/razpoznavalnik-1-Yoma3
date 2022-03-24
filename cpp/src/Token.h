#ifndef TOKEN_H
#define TOKEN_H
#include <iostream>
#include <string>

using namespace std;

class Token {
private:
	string lexem;	    //leksikalni element
	int column;			//stolpec izvorne datoteke kjer je prvi znak leksema
	int row;			//vrstica izvorne datoteke kjer je prvi znak leksema
	int token;			//osnovni leksikalni simbol
	bool eof;			//konec datoteke
public:
    //privzeti konstruktor
	Token() : lexem("") {}
    //konstruktor
	Token(const std::string& aLexem, int aColumn, int aRow, int aToken, bool EoF) : lexem(aLexem),
		column(aColumn), row(aRow), token(aToken), eof(EoF) {}
	const std::string getLexem() const{
		return lexem;
	}

	const int getColumn() const{
		return column;
	}
	const int getRow() const{
		return row;
	}
	const int getToken() const {
		return token;
	}
	const int isEoF() const{
		return eof;
	}
	friend std::ostream& operator<<(std::ostream& out,const Token &aToken) {
        string token = "";
        switch(aToken.getToken()){
            case 1:
                token = "float";
                break;
            case 2:
                token = "error";
                break;
            case 3:
                token = "float";
                break;
            case 4:
            case 5:
                token = "variable";
                break;
            case 6:
                token = "plus";
                break;
            case 7:
                token = "minus";
                break;
            case 8:
                token = "times";
                break;
            case 9:
                token = "divide";
                break;
            case 10:
                token = "pow";
                break;
            case 11:
                token = "lparen";
                break;
            case 12:
                token = "rparen";
                break;
            case 13:
                token = "space";
                break;

        }

		if(aToken.getToken() != 0)
            out << token << "(\"" << aToken.getLexem() << "\") ";
		return out;
	}
   /* friend std::ostream& operator<<(std::ostream& out,const Token &aToken) {
        out << "'" << aToken.getLexem() << "' " << aToken.getToken() << " (" << aToken.getRow() << ", " << aToken.getColumn() << ") " << (aToken.isEoF() ? "true" : "false");
        return out;
    }*/
};
#endif // TOKEN_H
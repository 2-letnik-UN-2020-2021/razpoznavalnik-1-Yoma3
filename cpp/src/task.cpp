#ifndef SCANNER_H
#define SCANNER_H
#include "Token.h"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>

class Scanner{
private:
    istream* input;         //vhodna datoteka
    Token LastToken;        //trenutni osnovni leksikalni simbol
    int row;                //vrstica vhodne datoteke
    int column;             //stolpec vhodne datoteke

    //opis tabele prehodov
    const static int maxState = 13;      //stevilo stanj avtomata
    const static int startState = 0;    //zacetno stanje
    const static int noEdge = -1;       //ni prehoda

    int automata[maxState + 1][256];    //tabela prehodov
    int finite[maxState + 1];           //tabela koncnih stanj

    //inicializacija tabele prehodov in tabele koncnih stanj
    void initAutomata() {
        for (int i = 0; i <= maxState; i++) {
            for (int j = 0; j < 256; j++) {
                automata[i][j] = noEdge;
            }
        }
        //ce smo v stanju 0 ali 1 ob cifri, gremo v stanje 1
        for (int j = '0'; j <= '9'; j++) {
            automata[0][j] = 1;
            automata[1][j] = 1;
            automata[2][j] = 3;
            automata[3][j] = 3;
            automata[4][j] = 5;
            automata[5][j] = 5;
        }
        automata[1]['.'] = 2;
        for (int j = 'A'; j <= 'Z'; j++) {
            automata[0][j] = 4;
            automata[4][j] = 4;
        }
        for (int j = 'a'; j <= 'z'; j++) {
            automata[0][j] = 4;
            automata[4][j] = 4;
        }
        automata[0]['+'] = 6;
        automata[0]['-'] = 7;
        automata[0]['*'] = 8;
        automata[0]['/'] = 9;
        automata[0]['^'] = 10;
        automata[0]['('] = 11;
        automata[0][')'] = 12;

        automata[0][' '] = automata[0]['\n'] = automata[0]['\t'] = 13;
        automata[13][' '] = automata[13]['\n'] = automata[13]['\t'] = 13;


        finite[0] = 0;      //zacetno stanje
        finite[1] = 1;      //float/int
        finite[2] = -2;     //. - error
        finite[3] = 3;      //float
        finite[4] = 4;      //variable
        finite[5] = 5;      //variable
        finite[6] = 6;      //+
        finite[7] = 7;      //-
        finite[8] = 8;    //*
        finite[9] = 9;      // / - divide
        finite[10] = 10;    // ^ - pow
        finite[11] = 11;    // ( lparen
        finite[12] = 12;    // ) rparen
        finite[13] = 13;      //space
    }
protected:
    //naslednje stanje je doloceno, ce poznamo trenutno stanje in
    //znak na vhodu, s preprostim vstopom v tabelo
    int getNextState(int aState, int aChar) const {
        if (aChar == -1) {
            return noEdge;
        }
        return automata[aState][aChar];
    }
    //ali je stanje koncno
    bool isFiniteState(int aState) const {
        return (finite[aState] > 0);
    }
    //vrni koncno stanje
    int getFiniteState(int aState) const {
        return finite[aState];
    }

private:
    int peek() {
        return input->peek();
    }
    //preberi naslednji znak na vhodu
    int read() {
        int temp = input->get();
        column++;
        if (temp == '\n') {
            row++;
            column = 1;
        }
        return temp;
    }
    bool eof() {
        return peek() == -1;
    }
    Token nextTokenImp(){
        int currentState = startState;
        string lexem;
        int startColumn = column;
        int startRow = row;
        do {
            //ob trenutnem stanju in chodnemu znaku pojdi v novo stanje
            int tempState = getNextState(currentState, peek());
            if (tempState != noEdge) {
                //prehod v novo stanje je mozen
                currentState = tempState;
                //zdruzi prebrani znak v leksem
                lexem += (char)read();
            }
            else {
                //prehod ni mozen --> ali je stanje koncno?
                if (isFiniteState(currentState)) {
                    //stanje je koncno
                    //vracamo osnovni leksikalni simbol
                    Token token(lexem, startColumn, startRow, getFiniteState(currentState), eof());
                    if (token.getToken() == 13) {
                        return nextToken();     //presledke in nove vrstice prezremo
                    }
                    return token;
                }
                else {
                    //stanje ni koncno --> vracamo leksikalno napako
                    return Token("", startColumn, startRow, 0, eof());
                }
            }
        } while (true);
    }

public:
    //konstruktor
    Scanner(istream* aInput) {
        row = 1;
        column = 1;
        initAutomata();
        input = aInput;
    }
    Token nextToken(){
        return LastToken = nextTokenImp();
    }
    Token currentToken() {
        return LastToken;
    }
};

class Parser{
private:
    Scanner scanner;
public:
    Parser(Scanner newScanner): scanner(newScanner){}

    /*
        E ::= T EE;
        EE ::= + T EE | - T EE | epsilon;
        T ::= F TT;
        TT ::= * F TT | / F TT | ^ F TT | epsilon;
        F ::= ( E ) | #number | - #number | #variable;

     */

    //    finite[0] = 0;      //zacetno stanje
    //    finite[1] = 1;      //float/int
    //    finite[2] = -2;     //. - error
    //    finite[3] = 3;      //float
    //    finite[4] = 4;      //variable
    //    finite[5] = 5;      //variable
    //    finite[6] = 6;      //+
    //    finite[7] = 7;      //-
    //    finite[13] = 13;    //*
    //    finite[9] = 9;      // / - divide
    //    finite[10] = 10;    // ^ - pow
    //    finite[11] = 11;    // ( lparen
    //    finite[12] = 12;    // ) rparen
    //    finite[8] = 0;      //space

    bool F(){
        int current = scanner.currentToken().getToken();
        switch (current) {
            case 1: case 3: case 4: case 5: case 9: case 8: case 10://if token is sign/variable/number -> return true
                return true;
            case 6: case 7://case 7: if current token is -, next check the next one ---> if it's not one of the above, return false
                if (scanner.currentToken().getLexem() == "-" || scanner.currentToken().getLexem() == "+") {
                    scanner.nextToken();
                    return F();     //recursive
                }
                return false;
            case 11://case 11: if current token is (, check if it's a start with variable/number and ends with ) ---> if not, return false
                if (scanner.currentToken().getLexem() == "(") {
                    scanner.nextToken();
                    bool tempE = E();
                    return (scanner.currentToken().getLexem() == ")" && tempE);
                }
                return false;
            default:
                return false;
        }
    }
    //first char is * / ^ = after that F, then again it can be TT ex: a + b = c
    //    finite[13] = 13;    //*
    //    finite[9] = 9;      // / - divide
    //    finite[10] = 10;    // ^ - pow
    bool TT(){
        int current = scanner.currentToken().getToken();
        if (current == 8 || current == 9 || current == 10){ //if current is: * / ^
            scanner.nextToken();
            bool tempF = F();
            scanner.nextToken();
            return (tempF && TT());
        }
        if(scanner.currentToken().getLexem() == ")"){ return true; }
        return (current > 5 || (current == 0 && scanner.currentToken().isEoF()));
    }
    //some variable then + or -
    bool T(){
        bool tempF= F();
        scanner.nextToken();
        return(tempF && TT());
    }
    //if + -, then can be T then again + -
    //    finite[6] = 6;      //+
    //    finite[7] = 7;      //-
    bool EE(){
        int current = scanner.currentToken().getToken();
        if(current == 6 || current == 7){
            scanner.nextToken();
            bool tempT = T();
            return (tempT && EE());
        }
        return true;    //in case there is nothing after
    }
    //some value then + -
    bool E(){
        return (T() && EE());
    }

    bool parse(){
        return (E() && scanner.currentToken().isEoF());
    }
};


  int main(int argc, char** argv)
  {

  //  ifstream file(argv[1]);
    ifstream file(R"(C:\Users\Yoma\CLionProjects\razpoznavalnik-1-Yoma3\cpp\src\test.txt)");
    if (file.is_open()) {
        Scanner s(&file);
        s.nextToken();
        Parser p(s);
        cout << p.parse();
        //if(p.parse()) cout << "positive";
        //else cout << "negative";
    }

}

#endif // SCANNER_H
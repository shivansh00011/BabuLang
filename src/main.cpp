#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>
#include <cstddef> 

#include "./parser.hpp"
#include "./generation.hpp"
#include "./tokenize.hpp"
#include "./arena.hpp"

using namespace std;


// babu lang -> just like talking to your girlfriend , it barley make's sense 

// enum TokenType{
//     _return,
//     int_lit,
//     semi,
//     openParam,
//     closedParam,
// };

// struct Token {
//     TokenType type ;
//     optional < string> value;
// };





// string token_to_asm(vector<Token>tokens){
//     string output ="global _start\n_start:\n" ;
//     for(int i=0;i<tokens.size();i++){
//         const Token& token = tokens[i];
//         if(token.type == _return){
//             if(i+1<tokens.size()&&tokens[i+1].type==int_lit){
//                 if(i+2<tokens.size()&&tokens[i+2].type == semi){
//                    output+= "     mov rax , 0x2000001\n";
//                    output += "     mov rdi , " + tokens[i+1].value.value() + "\n";
//                    output+= "     syscall\n";
//                 }
//             }
//         }
//     }
//     return output;
// }

int main (int argc,char*argv[]){
    cout<<argc;
   if(argc != 2){
    cout<<"incorrect usage correct usage is ..."<<endl;
    cout<<"./Babu <input.babu>"<<endl;
    return EXIT_FAILURE;
   }

   ifstream file (argv[1]);

   if(!file){
    cout<<"unable to read file from memory";
    return 1;
   }
   char ch;
   string input = "";  
    while (file.get(ch)) {
        input += ch; 
    }
    Tokenizer tokenizer(move(input));
    vector<Token> things = tokenizer.tokenize();
    file.close();

    ofstream MyFile("org.asm");
    Parser parser (move(things));
    optional <NodeProg>prog = parser.parseProg();
    if(!prog.has_value()){
        cerr <<"problem in parseing"<<endl;
        exit(EXIT_FAILURE);
    }
    Generator generator(prog.value());
    MyFile << generator.gen_prog();
    MyFile.close();
// system("nasm -f macho64 out.asm -o out.o");
// system("ld -o out out.o -macosx_version_min 10.13 -lSystem -syslibroot `xcrun --show-sdk-path` -e _start");

    
    return 0;
   
}
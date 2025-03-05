#pragma once
#include <vector>
#include <optional>
#include <string>
#include <iostream>

using namespace std;


//what is enum,optional,inline?

enum class TokenType {
    _return,
    literal,
    openParam,
    closedParam,
    semi,
    ident,
    let,
    eq,
    plus,
    star,
    div,
    sub,
    openCurly,
    closedCurly,
    output,
    if_,
};

enum varType{
   int_lit,
   string_lit
};

struct Token {
    TokenType type;
    std::optional<std::string> value;  
    std::optional<varType> variableType;  
    
};

optional<int> bin_prec(TokenType type){
    switch (type)
    {
    case TokenType :: plus:
        return 0;
    case TokenType :: star:
        return 1;
    case TokenType :: div :
        return 1;
    case TokenType :: sub :
        return 0;
    
    default:
        return {};
    }
}

class Tokenizer {
    public:
        inline explicit Tokenizer (std :: string src) : m_src(std :: move(src)){//move src helps prevent multiple coppies of the code

        }
     inline std::vector<Token> tokenize (){
        string buf = "";
         vector<Token> token;

         while (peek().has_value()) {
            if (isalpha(peek().value())) {
                buf.push_back(consume());
                while (peek().has_value() && std::isalnum(peek().value())) {
                    buf.push_back(consume());
                }
            
                if (buf == "babuBus") {
                    token.push_back({ TokenType::_return});
                    buf.clear();
                }
                else if (buf == "mano") {
                    token.push_back({ TokenType::let });
                    buf.clear();
                }else if (buf == "likhoBabu") {
                    token.push_back({ TokenType::output});
                    buf.clear();
                }else if(buf == "agar"){
                    token.push_back({ TokenType::if_});
                    buf.clear();
                }
                else {
                    token.push_back({ TokenType::ident, buf });
                    buf.clear();
                    continue;
                }

            }
               else if( isdigit(peek().value())){
                    buf.push_back(consume());  

                    while(peek().has_value()&&isdigit(peek().value())){
                        buf.push_back(consume());
                    }

                    token.push_back({.type= TokenType :: literal, .value=buf,.variableType=varType :: int_lit});
                    buf.clear();
                    continue;


                }else if (peek().has_value() && peek().value() == '"') {
                    string buff = "";
                    consume();  
                
                    while (peek().has_value() && peek().value() != '"') {
                      
                        if (!peek().has_value()) {
                            cerr << "String error: Closing quotes not found" << endl;
                            exit(EXIT_FAILURE);
                        }
                        buff.push_back(consume());
                    }
                
                  
                    if (!peek().has_value() || peek().value() != '"') {
                        cerr << "String error: Closing quotes not found" << endl;
                        exit(EXIT_FAILURE);
                    }
                    consume(); 
                
                    cout << "Parsed string: " << buff << endl;
                
                    token.push_back({
                        .type = TokenType::literal,
                        .value = buff,
                        .variableType = varType::string_lit
                    });
                
                    continue;
                }
                else if(peek().value()=='('){
                    consume();
                    token.push_back({.type=TokenType :: openParam});
                }
                else if(peek().value()==')'){
                    consume();
                    token.push_back({.type=TokenType :: closedParam});
                }
                else if(peek().value()==';'){
                    consume();
                    token.push_back({.type=TokenType :: semi});
                    
                }
                 else if(peek().value()=='='){
                    consume();
                    token.push_back({.type=TokenType :: eq});
                   
                }
                else if(isspace(peek().value())){
                    consume();
                   
                }else if(peek().value() == '+'){
                    consume();
                    token.push_back({.type = TokenType :: plus});
                    continue;
                }else if(peek().value()=='*'){
                    consume();
                    token.push_back({.type = TokenType :: star});
                    continue;
                }else if(peek().value()=='{'){
                    consume();
                    token.push_back({.type =TokenType :: openCurly});
                }else if(peek().value()=='}'){
                    consume();
                    token.push_back({.type =TokenType :: closedCurly});
                }
                else if(peek().value()=='/'){
                    consume();
                    token.push_back({.type = TokenType :: div});
                    continue;
                }else if(peek().value()=='-'){
                    consume();
                    token.push_back({.type =TokenType :: sub});
                    continue;
                }
                else{
                     cerr<< "Error in tokenizer "  << endl;
                        exit(EXIT_FAILURE);
                }
         }

      m_index = 0;
      return token;
     }
    private:
   inline optional<char> peek(int ahed = 0) const {
        if(m_index+ahed >= m_src.length()){
            return {};
        }
        else {
            return m_src.at(m_index+ahed);
        }
    }
   
  inline  char consume(){
       return  m_src.at(m_index ++);

    }

    const string m_src;
    const int m_size = m_src.size();
    int m_index ;

};
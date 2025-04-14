#pragma once 
#include <variant>
#include "./arena.hpp"
#include "./tokenize.hpp"
using namespace std;



// enum TokenType{
//     _return,
//     int_lit,
//     semi,
// };

// struct Token {
//     TokenType type ;
//     optional < string> value;
// };
    struct NodeTermIdent{
        Token ident;
    };
    struct NodeTermLit{
        varType type_of;
        Token literal;
    };
    struct NodeExpr;

    struct BinExpAdd{
        NodeExpr *lhs;
        NodeExpr *rhs;
     };
     struct NodeTermParam{
        NodeExpr*var;
     };
     struct BinExpMulti{
        NodeExpr *lhs;
        NodeExpr *rhs;
     };
     struct BinExpSub{
        NodeExpr *lhs;
        NodeExpr *rhs;
     };
     struct BinExpDiv{
        NodeExpr *lhs;
        NodeExpr *rhs;
     };
    struct BinExp {
        std :: variant<BinExpAdd*,BinExpMulti*,BinExpDiv*,BinExpSub*>var;
        //  BinExpAdd*add;
    };
    struct NodeTerm{
        std :: variant<NodeTermIdent*,NodeTermLit*,NodeTermParam*>var;
    }; 
    struct NodeExpr{
        std :: variant<NodeTerm*,BinExp*> var;
    };
    struct NodeStmtExit{
        NodeExpr *expr;
    };
    struct NodeStmtOutput{
        varType type;
        NodeExpr *expr;
        
    };
    struct NodeStmtLet{
        Token idnt;
        NodeExpr *expr;
    };
    struct NodeStmt;
    struct NodeScope {
        vector<NodeStmt*>stmt;
    };
    struct NodeStmtIf{
        NodeExpr * expr;
        NodeScope *scope;
    };
    struct NodeStmt {
    variant<NodeStmtExit*, NodeStmtLet*,NodeScope*,NodeStmtOutput*,NodeStmtIf*>var;
    };
   
      struct NodeProg{
        std::vector<NodeStmt*> stmt;
    };
   





class Parser {
    public :
     inline explicit Parser(vector<Token> t) : m_tokens(move(t)), m_index(0),m_Allocator(1024*1024*4){}
     optional<NodeTermLit*>parse_Lit(){
        auto nodeLit = m_Allocator.alloc<NodeTermLit>();
        auto val = consume(); // Get the token
    
        if (!val.variableType.has_value()) {
            cerr << "Error: Missing variableType in token." << endl;
            exit(EXIT_FAILURE);
        }
    
        nodeLit->type_of = val.variableType.value();
        nodeLit->literal = val;
    
        if (val.variableType == varType::int_lit) {
            cout << "Parsed Integer Literal: " << val.value.value() << endl;
        } 
        else if (val.variableType == varType::string_lit) {
            cout << "Parsed String Literal: \"" << val.value.value() << "\"" << endl;
        }
    
        return nodeLit;
     }
    optional <NodeTerm*>parse_term (){
        if(peek().has_value() && peek().value().type == TokenType ::literal){
            // auto  node_term_int_lit = m_Allocator.alloc<NodeTermLit>();
            // node_term_int_lit->literal = consume();
            auto lit = parse_Lit();
            auto temp = lit.value()->type_of;
            cout << temp<<endl;
            auto term = m_Allocator.alloc<NodeTerm>();
            term->var = lit.value();
            return term;
        }
        else if(peek().has_value()&& peek().value().type == TokenType :: ident){
            auto node_term_ident = m_Allocator.alloc<NodeTermIdent>();
            node_term_ident->ident=consume();
            auto term = m_Allocator.alloc<NodeTerm>();
            term->var = node_term_ident;
            return term;
        }else if(peek().has_value()&& peek().value().type == TokenType :: openParam){
            consume();
            auto expr = parse_exp();
            if(!expr.has_value()){
                cerr<<"Expected value";
                exit(EXIT_FAILURE);
            }
            if(peek().has_value()&& peek().value().type == TokenType :: closedParam){
                consume();
                auto term_Paran = m_Allocator.alloc<NodeTermParam>();
                term_Paran->var = expr.value();
                auto term = m_Allocator.alloc<NodeTerm>();
                term->var = term_Paran;
                return term;

            }else{
                cerr<<"Expected ')'";
                exit(EXIT_FAILURE);
            }
        }
        else{
            return {};
        }
    }
    optional<NodeExpr*> parse_exp(int minPrec = 0) {
        std::optional<NodeTerm*> term_lhs = parse_term();
        if (!term_lhs.has_value()) return {};
    
        auto expLhs = m_Allocator.alloc<NodeExpr>();
        expLhs->var = term_lhs.value();
    
        while (true) {
            optional<Token> cur_token = peek();
            optional<int> prec;
    
            if (cur_token.has_value()) {
                prec = bin_prec(cur_token->type);
                if (!prec.has_value() || prec.value() < minPrec) {
                    break;
                }
            } 
            else {
                break;
            }
    
            std::cout << "Consuming token: " << static_cast<int>(cur_token->type) << "\n";
            Token op = consume();
    
            int next_minPrec = prec.value() + 1;
            auto expr_rhs = parse_exp(next_minPrec);
            if (!expr_rhs.has_value()) {
                std::cerr << "Unable to parse RHS of expression\n";
                exit(EXIT_FAILURE);
            }
    
            auto expr = m_Allocator.alloc<BinExp>();
            auto expr_lhs2 = m_Allocator.alloc<NodeExpr>();
    
            if (op.type == TokenType::plus) {
                auto add = m_Allocator.alloc<BinExpAdd>();
                expr_lhs2->var = expLhs->var;
                add->lhs = expr_lhs2;
                add->rhs = expr_rhs.value();
                expr->var = add;
            } else if (op.type == TokenType::star) {
                auto multi = m_Allocator.alloc<BinExpMulti>();
                expr_lhs2->var = expLhs->var;
                multi->lhs = expr_lhs2;
                multi->rhs = expr_rhs.value();
                expr->var = multi;
            } else if(op.type == TokenType :: sub){
                auto sub = m_Allocator.alloc<BinExpSub>();
                expr_lhs2->var = expLhs->var;
                sub->lhs = expr_lhs2;
                sub->rhs = expr_rhs.value();
                expr->var = sub;
            }else if(op.type == TokenType :: div){
                auto div = m_Allocator.alloc<BinExpDiv>();
                expr_lhs2->var = expLhs->var;
                div->lhs = expr_lhs2;
                div->rhs = expr_rhs.value();
                expr->var = div;
            }
    
            expLhs->var = expr;
        }
        return expLhs;
    }
    optional<NodeScope*>parse_scope(){
               auto  open = consume();
               if(open.type != TokenType :: openCurly){
                cerr<<"INVALID SYNTAX { required";
                exit(EXIT_FAILURE);
               }
                auto scope = m_Allocator.alloc<NodeScope>();
                while(auto stmt = parse_stmt()){
                    scope->stmt.push_back(stmt.value());
                }
                if(peek().has_value() &&peek().value().type == TokenType :: closedCurly){
                    consume();
                }else{
                    cerr<<"expected Close curly"<<endl;
                    exit(EXIT_FAILURE);
                }
               return scope;
    }
     
     optional <NodeStmt*> parse_stmt (){
     
             if(peek().has_value() &&peek().value().type == TokenType :: _return 
             && peek(1).has_value()&&peek(1).value().type ==  TokenType ::openParam){
                cout<<"in return"<<endl;
                consume();
                consume();
                auto stmt_exit = m_Allocator.alloc<NodeStmtExit>();
                if(auto node_expr = parse_exp()){
                    stmt_exit->expr = node_expr.value();
                }else{
                    cerr <<"Invaled epression";
                    exit(EXIT_FAILURE);
                }
                if(peek().has_value()&&peek().value().type == TokenType :: closedParam){
                consume();
            }
            else {
               cerr <<"Invaled : expected closeing params";
                    exit(EXIT_FAILURE);
            }
            if(peek().has_value()&&peek().value().type== TokenType ::semi){
                    consume(); 
            }
            else{
                cerr <<"Invaled : expected semicolon";
                    exit(EXIT_FAILURE);
            }
            auto stmt = m_Allocator.alloc<NodeStmt>();
            stmt->var = stmt_exit;
             return stmt;
            }
            else if(peek().has_value()&&peek().value().type == TokenType :: let&&
            peek(1).has_value()&&peek(1).value().type == TokenType :: ident&&
            peek(2).has_value()&&peek(2).value().type == TokenType :: eq){
                cout<<"in let"<<endl;
                consume();
                auto stmt_let = m_Allocator.alloc<NodeStmtLet>();
                stmt_let->idnt = consume();
                consume();
                if(auto exp = parse_exp()){
                    stmt_let->expr = exp.value();
                }else{
                    cerr<<"invaled expression "<<endl;
                    exit(EXIT_FAILURE);
                }
                if(peek().has_value() && peek().value().type == TokenType :: semi){
                    consume();
                }else{
                     cerr<<"expected semi"<<endl;
                    exit(EXIT_FAILURE);
                }
                auto stmt = m_Allocator.alloc<NodeStmt>();
                stmt->var=stmt_let;
                return stmt;
            }else if(peek().has_value() &&peek().value().type == TokenType :: openCurly){
               if(auto scope = parse_scope()){
                 auto stmt = m_Allocator.alloc<NodeStmt>();
                 stmt->var = scope.value();

                return stmt;
               }
               else{
                cerr<<"Error In scope"<<endl;
               exit(EXIT_FAILURE);
           }
            }
           
            else if(peek().has_value() &&peek().value().type == TokenType :: output
            && peek(1).has_value()&&peek(1).value().type ==  TokenType ::openParam){
                consume();
                consume();
                cout<<"in output"<<endl;
                auto stmt_input = m_Allocator.alloc<NodeStmtOutput>();
                if(auto expr = parse_exp()){

                    stmt_input->expr = expr.value();
                }else{
                    cerr<<"INVALID expression"<<endl;
                    exit(EXIT_FAILURE);
                }
                if(peek().has_value() && peek().value().type ==TokenType :: closedParam){
                    consume();
                }
                if(peek().has_value() && peek().value().type == TokenType :: semi){
                    consume();
                }else{
                     cerr<<"expected semi"<<endl;
                    exit(EXIT_FAILURE);
                }
                auto stmt = m_Allocator.alloc<NodeStmt>();
                stmt->var = stmt_input;
               return stmt;
            }else if(peek().has_value() &&peek().value().type == TokenType :: if_
            ){
                consume(); 
                auto open = consume();
                if (open.type != TokenType::openParam) {
                    cerr << "Expected '(' after agar" << endl;
                    exit(EXIT_FAILURE);
                }
            
                auto stmt_if = m_Allocator.alloc<NodeStmtIf>();
                if (auto expr = parse_exp()) {  // Just parse the expression, don't assign it to a variable
                    stmt_if->expr = expr.value();
                } else {
                    cerr << "Invalid expression inside agar()" << endl;
                    exit(EXIT_FAILURE);
                }
            
                auto close = consume();
                if (close.type != TokenType::closedParam) {
                    cerr << "Expected ')' after agar condition" << endl;
                    exit(EXIT_FAILURE);
                }
            
                if (auto scope = parse_scope()) {
                    stmt_if->scope = scope.value();
                } else {
                    cerr << "Invalid scope inside agar block" << endl;
                    exit(EXIT_FAILURE);
                }
            
                auto stmt = m_Allocator.alloc<NodeStmt>();
                stmt->var = stmt_if;
                return stmt;
            }
           else return {};
            
            
    }
     
     
      optional <NodeProg> parseProg(){
       NodeProg prog;
        while(peek().has_value()){    
            if(auto stmt = parse_stmt()){
                
                prog.stmt.push_back(stmt.value());
              
            }else{
                cerr <<"Invalid stmt"<<endl;
                exit(EXIT_FAILURE);
            }
        }
        return prog;
      }

    private :
     const vector<Token>m_tokens;
     size_t m_index;
     inline optional<Token> peek(int ahead = 0) {
    if (m_index + ahead >= m_tokens.size()) {
        return {};
    }
    return m_tokens.at(m_index + ahead); 
    }
     inline Token consume (){
        return m_tokens.at(m_index++);
     }
     ArenaAllocator m_Allocator ;

};
void veryIMportantFunction(){
    cout<<"very important work";
}
#pragma once
#include <map>
#include <cstddef>
#include <unordered_map>
#include <algorithm>
#include "./parser.hpp"

class Generator {
public:
    inline explicit Generator(NodeProg p) : m_prog(std::move(p)) {};
    string gen_term (const NodeTerm*term){
        struct TermVisitor {
            Generator* gen;
            TermVisitor(Generator* g) : gen(g) {}
            string operator()(const NodeTermLit* term_lit) {
                if (term_lit->literal.variableType == varType::string_lit) {
                    cout << "String encountered, still needs to be implemented: "
                         << term_lit->literal.value.value() << endl;
            
                    std::string stringLabel = "str_" + std::to_string(gen->m_varCount++);
            
                    // Add to the .data section (null-terminated for compatibility)
                    gen->m_dataSection += stringLabel + " db \"" + term_lit->literal.value.value() + "\", 0\n";
            
                    // Generate assembly to load the string address into RAX
                    return "    lea rax, [rel " + stringLabel + "]\n" + gen->push("rax");
                }
            
                std::string valueStr = term_lit->literal.value.value(); // Store value once
                int key;
            
                // Try to convert the string to an integer
                try {
                    key = std::stoi(valueStr);
                } catch (const std::invalid_argument&) {
                    cerr << "Error: Invalid integer format: " << valueStr << endl;
                    return ""; // Return empty string on error
                } catch (const std::out_of_range&) {
                    cerr << "Error: Integer out of range: " << valueStr << endl;
                    return "";
                }
            
                // Check if the key exists in the map
                if (gen->m_lookup.find(key) == gen->m_lookup.end()) {
                    cout << key << endl;
                    gen->m_lookup[key] = "value" + std::to_string(gen->m_varCount++);
                    gen->m_dataSection += gen->m_lookup[key] + " dq " + valueStr + "\n";
                }
            
                return "    mov rax, [rel " + gen->m_lookup[key] + "]\n" + gen->push("rax");
            }
           string operator()(const NodeTermIdent * term_ident){
            auto it = std::find_if(gen->m_vars.begin(),gen->m_vars.end(),[&](const Var & var){return var.name == term_ident->ident.value.value();});
                if(it == gen->m_vars.end()){
                    cerr <<"Undeclared identifire" << "   " <<term_ident->ident.value.value()<<endl;
                    exit(EXIT_FAILURE);
                }
                string address = "QWORD [rsp + " + to_string((gen->m_stack_size - it->stack_loc-1)*8) + "]";
                return gen->push(address);
            }
            string operator()(const NodeTermParam * term_param){
                return gen->gen_expr(term_param->var);
            }
          
        };
        TermVisitor visitor(this);
        return std::visit(visitor, term->var); 
    }
    string gen_bin_exp(const BinExp * binExp){
        struct binExpVisitor{
            Generator * gen;
            string operator ()(const BinExpAdd *add){
                string lhs = gen->gen_expr(add->lhs);
                string rhs = gen->gen_expr(add->rhs);
                string result= "";
                result +=lhs;
                result+= rhs;
                result += gen->pop("rax");
                result += gen->pop("rdx");
                result += "    add rdx, rax\n";
                result += gen->push("rdx");
                return result;
            }
            string operator ()(const BinExpSub *sub){
                string lhs = gen->gen_expr(sub->lhs);
                string rhs = gen->gen_expr(sub->rhs);
                string result= "";
                result +=lhs;
                result+= rhs;
                result += gen->pop("rax");
                result += gen->pop("rdx");
                result += "    sub rdx, rax\n";
                result += gen->push("rdx");
                return result;
            }
            string operator()(const BinExpMulti *multi){
                string lhs = gen->gen_expr(multi->lhs);
                string rhs = gen->gen_expr(multi->rhs);
                string result= "";
                result +=lhs;
                result+= rhs;
                result += gen->pop("rax");
                result += gen->pop("rbx");
                result += "    mul rbx\n";
                result += gen->push("rax");
                return result;
            }
            string operator()(const BinExpDiv *div) {
                string result = "";
                
                // Generate code for both operands
                string rhs = gen->gen_expr(div->rhs);  // Divisor
                string lhs = gen->gen_expr(div->lhs);  // Dividend
                
                // Append generated code
                result += lhs;  // Push dividend first
                result += rhs;  // Push divisor second
                
                // Pop in correct order (LIFO)
                result += gen->pop("rbx");  // Divisor (most recently pushed)
                result += gen->pop("rax");  // Dividend 
                
                // Zero extend rax into rdx:rax
                result += "    xor rdx, rdx\n";
                
                // Perform division
                result += "    div rbx\n";
                
                // Push result
                result += gen->push("rax");
                
                return result;

//I see the issue now. Let's analyze more carefully what's happening in this operator for generating x86-64 assembly code for division operations. The problem is with register order and the div instruction.
// The x86-64 div instruction requires:

// The dividend in rdx:rax (with rdx containing the high bits)
// The divisor in another register (in your case, rbx)

// There are several issues with your current implementation:

// No zero extension to rdx - The div instruction uses rdx:rax as a 128-bit dividend, so you need to zero out rdx before division.
// Pop order - You're popping in the wrong order based on your code generation sequence:

// You generate rhs (divisor) first, then lhs (dividend)
// But then pop the more recent value (dividend) into rax and the older value (divisor) into rbx
// This is backwards for the x86 stack (LIFO - Last In, First Out)
            }
        };
        binExpVisitor visitior{.gen = this} ;
       return  std :: visit(visitior,binExp->var);
    }
    string gen_expr(const NodeExpr* expr) {
        
        struct exprVisitor {
            Generator *gen;
            string operator()(const NodeTerm * term) {
              
              return gen->gen_term(term);
            }
            
            string operator() (const BinExp * bin_expr){
              return gen->gen_bin_exp(bin_expr);
               
            }
        };
        
         exprVisitor visitor{this};
         return std::visit(visitor, expr->var);
    }
    string gen_scope(const NodeScope*scope){
        string result = "";
         beginScope();
            
        for (const auto* stmt : scope->stmt) {  // Iterate over statements inside scope
            result += gen_stmt(stmt);  // Append to result, not m_output
        }
    
        result += endScope();  // End scope and adjust stack
        result += "    ;; /scope\n";  // End scope marker
        return result;

    }

    string gen_stmt(const NodeStmt* stmt) {
        struct stmtVisitor {
            Generator& gen;

            string operator()(const NodeStmtExit* stmt_exit)const {
               
                return gen.gen_expr(stmt_exit->expr) +
                gen.pop("rdi") +
                "    mov rax, 0x2000001\n"
                "    syscall\n";          
            }
           
            string operator()(const NodeStmtOutput* output) const {
                string result = gen.gen_expr(output->expr);
                result += gen.pop("rax");  // Pop computed value into RAX

                if (output->type == varType :: string_lit) { 
                   cout<<"string literal encountered"<<endl;
    
                    return " ";
                }
                // Preserve RAX value before integer-to-string conversion
                  // Pop computed value into RAX
                result += "    push rax\n"; // Preserve value in case needed later
            
                // Allocate buffer for conversion
                result += "    sub rsp, 32\n";  
                result += "    mov rcx, 31\n";  
                result += "    mov BYTE [rsp + rcx], 10\n"; // Newline
                result += "    dec rcx\n";
            
                // Handle zero case
                result += "    cmp rax, 0\n";
                result += "    jne .convert_" + std::to_string(gen.m_label_count) + "\n";
                result += "    mov BYTE [rsp + rcx], '0'\n";
                result += "    dec rcx\n";
                result += "    jmp .print_" + std::to_string(gen.m_label_count) + "\n";
            
                // Integer-to-string conversion
                result += ".convert_" + std::to_string(gen.m_label_count) + ":\n";
                result += "    xor rdx, rdx\n";  
                result += "    mov rbx, 10\n";  
                result += "    div rbx\n";  
                result += "    add dl, '0'\n";  
                result += "    mov BYTE [rsp + rcx], dl\n";  
                result += "    dec rcx\n";
                result += "    test rax, rax\n";  
                result += "    jnz .convert_" + std::to_string(gen.m_label_count) + "\n";
            
                // Print value
                result += ".print_" + std::to_string(gen.m_label_count) + ":\n";
                result += "    inc rcx\n";  
                result += "    mov rdx, 32\n";  
                result += "    sub rdx, rcx\n"; 
                 
                result += "    mov rax, 0x2000004\n";  // syscall write
                result += "    mov rdi, 1\n";  
                result += "    lea rsi, [rsp + rcx]\n";  
                result += "    syscall\n";
            
                // Restore stack
                result += "    add rsp, 32\n";  
                result += "    pop rax\n";  // Restore preserved value
            
                gen.m_label_count++;
                
                return result;
            }
            string operator()(NodeStmtLet* stmt_let) {
                // Get the current scope's starting position in m_vars
                size_t scope_start = gen.m_scopes.empty() ? 0 : gen.m_scopes.back();
                
                // Only check for duplicate variables within the current scope
                auto it = std::find_if(
                    gen.m_vars.begin() + scope_start,
                    gen.m_vars.end(),
                    [&](const Var & var){return var.name == stmt_let->idnt.value.value();}
                );
                
                if(it != gen.m_vars.end()){
                    cerr<<"Identifite already exists : "<< stmt_let->idnt.value.value()<<endl;
                    exit(EXIT_FAILURE);
                }
                
                gen.m_vars.push_back({stmt_let->idnt.value.value(), gen.m_stack_size});
                string result = gen.gen_expr(stmt_let->expr);
                return result;
            }
            string operator()(const NodeScope* scope) {
                string result = gen.gen_scope(scope);
                
                return result;
            }
            string operator ()(const NodeStmtIf * stmt_if){
                string result = "";
                // Generate condition evaluation code
                result += gen.gen_expr(stmt_if->expr);
                result += gen.pop("rax");
                
                // Create a unique label for the if-end
                string endLabel = gen.create_lable();
                
                // Test the condition result
                result += "    test rax, rax\n";
                result += "    jz "+endLabel+"\n";
                
                // Generate code for the if block body
                // This will process likhoBabu(x) and babuBus(x) statements
                result += gen.gen_scope(stmt_if->scope);
                
                // End label
                result += endLabel+":\n";
             
                return result;
            }
        };
        return std::visit(stmtVisitor{*this}, stmt->var);  
    }

    string gen_prog() {
       
        for (const NodeStmt* stmt : m_prog.stmt) {
            m_output += gen_stmt(stmt); 
        }

       
        m_output += "    mov rax, 0x2000001\n";
        m_output += "    mov rdi, 0\n";         
        m_output += "    syscall\n";             

        return m_dataSection+m_output;
    }

private:
    void assert (bool temp){}
    string push(string reg){
        m_stack_size++;
        return "    push "+reg+"\n";
       
    }
    string create_lable(){
        return "label" + to_string(m_label_count++);
    }
    string pop(string reg){
        m_stack_size--;
        return "    pop "+reg+"\n";
       
    }
    void beginScope(){
        m_scopes.push_back(m_vars.size());
    }
    string endScope() {
        if (m_scopes.empty()) return "";  // No scope to end
    
        const size_t pop_count = m_vars.size() - m_scopes.back();
        string output = "";
    
        if (pop_count != 0) {
            output = "    add rsp, " + std::to_string(pop_count * 8) + "\n";
        }
    
        m_stack_size -= pop_count;
        
        for (size_t i = 0; i < pop_count; i++) {
            m_vars.pop_back();
        }
    
        m_scopes.pop_back();  // Remove scope marker
        return output;
    }

    struct Var {
        string name;
        size_t stack_loc;
    };
    size_t m_stack_size = 0;
    mutable size_t m_label_count = 0;
    unordered_map<int,string>m_lookup;
    string m_dataSection ="section .data\n";
    int m_varCount =0;
    const NodeProg m_prog;  
    string m_output = "section .text\n    global _start\n_start:\n"; 
    vector<Var>m_vars;
    vector<size_t>m_scopes;
    // map<string,Var>m_vars {};
};
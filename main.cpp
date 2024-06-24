#include "ast.hpp"
#include "interpreter.hpp"
#include "parser.hpp"

#include <iostream>
#include <utility>

int main() {
	std::string source;
	std::getline(std::cin, source);
	Ast::Stmt* stmt = Parser::parse_stmt(std::move(source));
	Ast::dump_stmt(stmt);
	std::cout << "\n";

	Interpreter::Evaluator evaluator;
	evaluator.exec(stmt);
}

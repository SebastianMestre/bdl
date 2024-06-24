#include "ast.hpp"
#include "interpreter.hpp"
#include "parser.hpp"
#include "typechecker.hpp"

#include <iostream>
#include <utility>

int main() {
	std::string source;
	std::getline(std::cin, source);
	Ast::Stmt* stmt = Parser::parse_stmt(std::move(source));
	Ast::dump_stmt(stmt);
	std::cout << "\n";

	{
		Typecheck::Typechecker typechecker;
		typechecker.visit(stmt);
	}

	{
		Interpreter::Evaluator evaluator;
		evaluator.exec(stmt);
	}
}

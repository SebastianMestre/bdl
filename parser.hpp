#pragma once

#include "ast.hpp"

#include <vector>
#include <string>

#include <cstdlib>

namespace Parser {
using namespace Ast;

struct Parser {

	int cursor;
	std::string source;

	Stmt* parse_stmt() {

		skip_whitespace();
		if (match("let") && !eof(3) && isspace(source[cursor+3])) {
			cursor += 3;

			skip_whitespace();
			if (match("var") && !eof(3) && isspace(source[cursor+3])) {
				cursor += 3;

				skip_whitespace();
				if (!is_identifier_starter(source[cursor])) syntax_error();

				int start = cursor;
				cursor++;
				while (!eof() && is_identifier_char(source[cursor])) {
					cursor++;
				}
				int finish = cursor;
				std::string name = source.substr(start, finish - start);

				skip_whitespace();
				if (!eat('=')) syntax_error();

				skip_whitespace();
				Expr* expr = parse_expr();

				return new LetVar {std::move(name), expr};
			} else {
				int start = cursor;

				if (!is_identifier_starter(source[cursor])) syntax_error();

				cursor++;
				while (!eof() && is_identifier_char(source[cursor])) {
					cursor++;
				}
				int finish = cursor;

				std::string name = source.substr(start, finish - start);

				skip_whitespace();
				if (!eat('=')) syntax_error();

				skip_whitespace();
				Expr* expr = parse_expr();

				return new Let {std::move(name), expr};
			}
		}

		syntax_error();
	}

	Expr* parse_expr() {
		Expr* lhs = parse_small_expr();
		while (true) {
			skip_whitespace();
			if (eat('+')) {
				Expr* rhs = parse_small_expr();
				lhs = new Add {lhs, rhs};
				continue;
			}
			break;
		}
		return lhs;
	}

	Expr* parse_small_expr() {
		skip_whitespace();
		if (eof()) syntax_error();
		if (match('0')) {
			if (eof(1) || isspace(source[cursor+1])) {
				cursor++;
				return new Int {0};
			} else {
				syntax_error();
			}
		} else if (is_digit(source[cursor])) {
			int value = source[cursor] - '0';
			cursor++;
			while (!eof() && is_digit(source[cursor])) {
				value *= 10;
				value += source[cursor] - '0';
				cursor++;
			}
			return new Int {value};
		} else if (is_identifier_starter(source[cursor])) {
			int start = cursor;
			cursor++;
			while (!eof() && is_identifier_char(source[cursor])) {
				cursor++;
			}
			int finish = cursor;

			std::string name = source.substr(start, finish - start);
			return new Var {std::move(name)};
		} else if (eat('[')) {
			std::vector<Expr*> items;
			skip_whitespace();
			if (!eat(']')) while (true) {
				items.push_back(parse_expr());
				skip_whitespace();
				if (eat(',')) continue;
				if (eat(']')) break;
				syntax_error();
			}
			return new Array {std::move(items)};
		} else {
			syntax_error();
		}
	}

	[[noreturn]] void syntax_error() {
		std::cerr << "Syntax error!\n";
		exit(EXIT_FAILURE);
	}

	bool eat(char c) {
		if (match(c)) {
			cursor++;
			return true;
		}
		return false;
	}

	bool match(char const* c, int offset = 0) {
		for (; *c; c++) {
			if (!match(*c, offset)) return false;
			offset++;
		}
		return true;
	}

	bool match(char c, int offset = 0) {
		return !eof(offset) && source[cursor + offset] == c;
	}

	void skip_whitespace() {
		while (!eof() && isspace(source[cursor]))
			cursor++;
	}

	bool eof(int offset = 0) {
		return cursor + offset >= int(source.size());
	}

	static bool is_identifier_starter(char c) {
		return is_alpha(c) || (c == '_');
	}

	static bool is_identifier_char(char c) {
		return is_alpha(c) || (c == '_') || is_digit(c);
	}

	static bool is_alpha(char c) {
		return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
	}

	static bool is_digit(char c) {
		return ('0' <= c && c <= '9');
	}

};

Ast::Stmt* parse_stmt(std::string source) {
	Parser parser = {0, std::move(source)};
	return parser.parse_stmt();
}

} // namespace Parser

#pragma once

#include<vector>
#include<iostream>

namespace Ast {

struct Expr {
	enum class Tag { Int, Array, Var, Add };
	Expr(Tag tag_) : tag{tag_} {}

	Tag tag;
};

struct Int : Expr {
	Int(int value_)
	: Expr(Tag::Int)
	, value{value_}
	{}

	int value;
};

struct Array : Expr {
	Array(std::vector<Expr*> items_)
	: Expr(Tag::Array)
	, items{std::move(items_)}
	{}

	std::vector<Expr*> items;
};

struct Var : Expr {
	Var(std::string name_)
	: Expr(Tag::Var)
	, name{std::move(name_)}
	{}

	std::string name;
};

struct Add : Expr {
	Add(Expr* lhs_, Expr* rhs_)
	: Expr(Tag::Add)
	, lhs{lhs_}
	, rhs{rhs_}
	{}

	Expr* lhs;
	Expr* rhs;
};

struct Stmt {
	enum class Tag { Let, LetVar };
	Stmt(Tag tag_) : tag{tag_} {}

	Tag tag;
};

struct Let : Stmt {
	Let(std::string name_, Expr* expr_)
	: Stmt(Tag::Let)
	, name{std::move(name_)}
	, expr{expr_}
	{}

	std::string name;
	Expr* expr;
};

struct LetVar : Stmt {
	LetVar(std::string name_, Expr* expr_)
	: Stmt(Tag::LetVar)
	, name{std::move(name_)}
	, expr{expr_}
	{}

	std::string name;
	Expr* expr;
};

void dump_expr(Expr* expr) {
	switch (expr->tag) {
	case Expr::Tag::Int: {
		auto e = static_cast<Int*>(expr);
		std::cout << "Int {" << e->value << "}";
	} break;
	case Expr::Tag::Array: {
		auto e = static_cast<Array*>(expr);
		std::cout << "Array {";
		auto sep = "";
		for (auto item : e->items) {
			std::cout << sep;
			sep = "; ";
			dump_expr(item);
		}
		std::cout << "}";
	} break;
	case Expr::Tag::Var: {
		auto e = static_cast<Var*>(expr);
		std::cout << "Var {" << e->name << "}";
	} break;
	case Expr::Tag::Add: {
		auto e = static_cast<Add*>(expr);
		std::cout << "Add {";
		dump_expr(e->lhs);
		std::cout << ", ";
		dump_expr(e->rhs);
		std::cout << "}";
	} break;
	}
}

void dump_stmt(Stmt* stmt) {
	switch (stmt->tag) {
	case Stmt::Tag::Let: {
		auto e = static_cast<Let*>(stmt);
		std::cout << "Let {" << e->name << ", ";
		dump_expr(e->expr);
		std::cout << "}";
	} break;
	case Stmt::Tag::LetVar: {
		auto e = static_cast<LetVar*>(stmt);
		std::cout << "LetVar {" << e->name << ", ";
		dump_expr(e->expr);
		std::cout << "}";
	} break;
	}
}


} // namespace Ast

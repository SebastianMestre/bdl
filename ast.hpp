#pragma once

#include<vector>
#include<iostream>

namespace Ast {


#if 1 // Types
struct Type {
	enum class Tag { Int, Array };
	Type(Tag tag_) : tag{tag_} {}

	Tag tag;
};

struct IntTy : Type {
	IntTy()
	: Type(Tag::Int)
	{}
};

struct ArrayTy : Type {
	ArrayTy(Type* item_type_)
	: Type(Tag::Array)
	, item_type{item_type_}
	{}

	Type* item_type;
};
#endif

#if 1 // Expressions
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
#endif

#if 1 // Statements
struct Stmt {
	enum class Tag { Let, LetVar, Block };
	Stmt(Tag tag_) : tag{tag_} {}

	Tag tag;
};

struct Let : Stmt {
	Let(std::string name_, Type* type_, Expr* expr_)
	: Stmt(Tag::Let)
	, name{std::move(name_)}
	, type{type_}
	, expr{expr_}
	{}

	std::string name;
	Type* type;
	Expr* expr;
};

struct LetVar : Stmt {
	LetVar(std::string name_, Type* type_, Expr* expr_)
	: Stmt(Tag::LetVar)
	, name{std::move(name_)}
	, type{type_}
	, expr{expr_}
	{}

	std::string name;
	Type* type;
	Expr* expr;
};

struct Block : Stmt {
	Block(std::vector<Stmt*> items_)
	: Stmt(Tag::Block)
	, items{std::move(items_)}
	{}

	std::vector<Stmt*> items;
};
#endif

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
			sep = ", ";
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

void dump_type(Type* type) {
	switch (type->tag) {
	case Type::Tag::Int: {
		auto e = static_cast<IntTy*>(type);
		std::cout << "IntTy {}";
	} break;
	case Type::Tag::Array: {
		auto e = static_cast<ArrayTy*>(type);
		std::cout << "ArrayTy {";
		dump_type(e->item_type);
		std::cout << "}";
	} break;
	}
}

void dump_stmt(Stmt* stmt) {
	switch (stmt->tag) {
	case Stmt::Tag::Let: {
		auto e = static_cast<Let*>(stmt);
		std::cout << "Let {" << e->name << ", ";
		dump_type(e->type);
		std::cout << ", ";
		dump_expr(e->expr);
		std::cout << "}";
	} break;
	case Stmt::Tag::LetVar: {
		auto e = static_cast<LetVar*>(stmt);
		std::cout << "LetVar {" << e->name << ", ";
		dump_type(e->type);
		std::cout << ", ";
		dump_expr(e->expr);
		std::cout << "}";
	} break;
	case Stmt::Tag::Block: {
		auto e = static_cast<Block*>(stmt);
		std::cout << "Block {";
		auto sep = "";
		for (auto item : e->items) {
			std::cout << sep;
			sep = ", ";
			dump_stmt(item);
		}
		std::cout << "}";
	}
	}
}


} // namespace Ast

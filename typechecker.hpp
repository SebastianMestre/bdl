#pragma once

#include "ast.hpp"

#include <utility>

namespace Typecheck {

struct Typechecker {

	std::vector<std::pair<std::string, Ast::Type*>> bindings;

// Bidirectional type checking.
//
// void check(Expr* expr, Type* type);
// Type* infer(Expr* expr);
//
// check and infer are mutually-recursive functions. When the type of
// subexpressions can be inferred from the outter expression, we use 'check'.
// When it can't, we use 'infer' and try to extract information to check other
// parts of the expression.
//
// For example, given 'let x:int = f(y)' we check that f(y) is an int.

	void check(Ast::Expr* expr, Ast::Type* type) {
		switch (expr->tag) {
		case Ast::Expr::Tag::Array: {
			auto e = static_cast<Ast::Array*>(expr);
			if (type->tag != Ast::Type::Tag::Array) type_error();
			auto array_type = static_cast<Ast::ArrayTy*>(type);
			auto item_type = array_type->item_type;
			for (auto item : e->items) {
				check(item, item_type);
			}
			return;
		} break;
		default: {
			auto actual_type = infer(expr);
			if (not equals(type, actual_type)) type_error();
			return;
		} break;
		}
		assert(0);
	}

	Ast::Type* infer(Ast::Expr* expr) {
		switch (expr->tag) {
		case Ast::Expr::Tag::Int: {
			return new Ast::IntTy {};
		} break;
		case Ast::Expr::Tag::Array: {
			auto e = static_cast<Ast::Array*>(expr);
			if (e->items.empty()) type_error();
			auto item_type = infer(e->items[0]);
			for (int i = 1; i != int(e->items.size()); ++i) {
				check(e->items[i], item_type);
			}
			return new Ast::ArrayTy {item_type};
		} break;
		case Ast::Expr::Tag::Var: {
			auto e = static_cast<Ast::Var*>(expr);
			int i = bindings.size();
			while (i >= 0 && bindings[i].first != e->name) {
				i--;
			}
			if (i < 0) type_error();
			return bindings[i].second;
		} break;
		case Ast::Expr::Tag::Add: {
			auto e = static_cast<Ast::Add*>(expr);
			check(e->lhs, new Ast::IntTy {});
			check(e->rhs, new Ast::IntTy {});
			return new Ast::IntTy {};
		} break;
		}
		assert(0);
	}

	void visit(Ast::Stmt* stmt) {
		switch (stmt->tag) {
		case Ast::Stmt::Tag::Let: {
			auto e = static_cast<Ast::Let*>(stmt);
			check(e->expr, e->type);
			bindings.emplace_back(e->name, e->type);
			return;
		} break;
		case Ast::Stmt::Tag::LetVar: {
			auto e = static_cast<Ast::LetVar*>(stmt);
			check(e->expr, e->type);
			bindings.emplace_back(e->name, e->type);
			return;
		} break;
		case Ast::Stmt::Tag::Block: {
			auto e = static_cast<Ast::Block*>(stmt);
			int const binding_count_on_enter = bindings.size();
			for (auto item : e->items) {
				visit(item);
			}
			while (bindings.size() > binding_count_on_enter) {
				bindings.pop_back();
			}
			return;
		}
		}
		assert(0);
	}

	bool equals(Ast::Type* a, Ast::Type* b) {
		if (a->tag == Ast::Type::Tag::Int && b->tag == Ast::Type::Tag::Int) {
			return true;
		}
		if (a->tag == Ast::Type::Tag::Array && b->tag == Ast::Type::Tag::Array) {
			return equals(static_cast<Ast::ArrayTy*>(a)->item_type, static_cast<Ast::ArrayTy*>(b)->item_type);
		}
		return false;
	}

	[[noreturn]] void type_error() {
		std::cerr << "Type error!\n";
		exit(EXIT_FAILURE);
	}

};

} // namespace Typecheck

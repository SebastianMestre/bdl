#pragma once

#include "ast.hpp"

#include <iostream>
#include <vector>

#include <cassert>

namespace Interpreter {

struct Value {
	enum class Tag { Int, Array };
	Tag tag;
	union {
		int as_int;
		std::vector<Value>* as_array;
	};

	Value() {
		tag = Tag::Int;
		as_int = 0;
	}

	Value(Value&& other) {
		switch (other.tag) {
		case Tag::Int: {
			tag = Tag::Int;
			as_int = other.as_int;
			other.as_int = 0;
		} break;
		case Tag::Array: {
			tag = Tag::Array;
			as_array = other.as_array;
			other.tag = Tag::Int;
			other.as_int = 0;
		} break;
		}
	}

	Value& operator= (Value&& other) {
		switch (tag) {
		case Tag::Int: {
		} break;
		case Tag::Array: {
			delete as_array;
		} break;
		}

		switch (other.tag) {
		case Tag::Int: {
			tag = Tag::Int;
			as_int = other.as_int;
			other.as_int = 0;
		} break;
		case Tag::Array: {
			tag = Tag::Array;
			as_array = other.as_array;
			other.tag = Tag::Int;
			other.as_int = 0;
		} break;
		}

		return *this;
	}

	Value(Value const&) = delete;
	Value& operator= (Value const&) = delete;

	static Value Int(int value) {
		Value result;
		result.tag = Tag::Int;
		result.as_int = value;
		return result;
	}

	static Value Array(std::vector<Value> value) {
		Value result;
		result.tag = Tag::Array;
		result.as_array = new std::vector<Value>(std::move(value));
		return result;
	}

	static Value clone(Value const& other) {
		switch (other.tag) {
		case Tag::Int: {
			return Value::Int(other.as_int);
		} break;
		case Tag::Array: {
			std::vector<Value> items;
			for (auto const& item : *other.as_array) {
				items.push_back(clone(item));
			}
			return Value::Array(std::move(items));
		} break;
		}
	}

	~Value() {
		switch (tag) {
		case Tag::Int: {
		} break;
		case Tag::Array: {
			delete as_array;
		} break;
		}
	}

};

void dump_value(Value const& v) {
#if 0
	switch (v.tag) {
	case Value::Tag::Int: {
		std::cout << "Int {" << v.as_int << "}";
		return;
	} break;
	case Value::Tag::Array: {
		std::cout << "Array {";
		auto sep = "";
		for (auto const& item : *v.as_array) {
			std::cout << sep;
			sep = "; ";
			dump_value(item);
		}
		std::cout << "}";
		return;
	} break;
	}
#else
	switch (v.tag) {
	case Value::Tag::Int: {
		std::cout << v.as_int;
		return;
	} break;
	case Value::Tag::Array: {
		std::cout << "[";
		auto sep = "";
		for (auto const& item : *v.as_array) {
			std::cout << sep;
			sep = ", ";
			dump_value(item);
		}
		std::cout << "]";
		return;
	} break;
	}
#endif
	assert(0);
}

struct Evaluator {

	static constexpr int max_stack = 1000;

	struct Variable {
		Value value;
	};

	Variable variable_storage[max_stack];
	Variable* variable_ptr;
	Variable* frame_ptr;
	std::vector<Variable*> frames;

	Value temp_storage[max_stack];
	Value* temp_ptr;

	Evaluator() {
		temp_ptr = temp_storage;
		frame_ptr = variable_ptr = variable_storage;
	}

	void push(Value x) {
		*temp_ptr++ = std::move(x);
	}

	Value pop() {
		return std::move(*--temp_ptr);
	}

	void push_variable(Value x) {
		*variable_ptr++ = {std::move(x)};
	}

	void pop_variable() {
		--variable_ptr;
	}

	Variable& get_variable(int frame_offset) {
		return frame_ptr[frame_offset];
	}

	void push_frame() {
		frames.push_back(frame_ptr);
		frame_ptr = variable_ptr;
	}

	void pop_frame() {
		variable_ptr = frame_ptr;
		frame_ptr = frames.back();
		frames.pop_back();
	}

	void eval(Ast::Expr* expr) {
		switch (expr->tag) {
		case Ast::Expr::Tag::Int: {
			auto e = static_cast<Ast::Int*>(expr);
			return push(Value::Int(e->value));
		} break;
		case Ast::Expr::Tag::Array: {
			auto e = static_cast<Ast::Array*>(expr);
			std::vector<Value> items;
			for (auto item : e->items) {
				eval(item);
				items.push_back(pop());
			}
			return push(Value::Array(std::move(items)));
		} break;
		case Ast::Expr::Tag::Var: {
			/* TODO: implement */
		} break;
		case Ast::Expr::Tag::Add: {
			auto e = static_cast<Ast::Add*>(expr);
			eval(e->lhs);
			eval(e->rhs);
			auto rhs = pop();
			auto lhs = pop();
			return push(Value::Int(lhs.as_int + rhs.as_int));
		} break;
		}
		assert(0);
	}

	void exec(Ast::Stmt* stmt) {
		switch (stmt->tag) {
		case Ast::Stmt::Tag::Let: {
			auto e = static_cast<Ast::Let*>(stmt);
			eval(e->expr);
			auto value = pop();
			dump_value(value);
			std::cout << "\n";
			push_variable(std::move(value));
		} break;
		case Ast::Stmt::Tag::LetVar: {
			auto e = static_cast<Ast::LetVar*>(stmt);
			eval(e->expr);
			auto value = pop();
			dump_value(value);
			std::cout << "\n";
			push_variable(std::move(value));
		} break;
		}
	}

};

};

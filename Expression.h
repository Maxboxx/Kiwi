#pragma once

#include "Ptr.h"
#include "Node.h"

namespace Kiwi {
	class Value;

	class Expression : public Node {
	public:
		virtual void Interpret(Interpreter::InterpreterData& data) final override {
			throw Boxx::Error("Call Evaluate instead");
		}

		/// Evaluates the expression.
		virtual Ptr<Interpreter::Value> Evaluate(Interpreter::InterpreterData& data) {
			return nullptr;
		}

		virtual void BuildString(Boxx::StringBuilder& builder) override {
			builder += "unknown expression";
		}
	};

	/// A unary kiwi expression.
	class UnaryExpression : public Expression {
	public:
		/// The value.
		Ptr<Value> value;
	};

	/// A binary kiwi expression.
	class BinaryExpression : public Expression {
	public:
		/// The first operand.
		Ptr<Value> value1;

		/// The second operand.
		Ptr<Value> value2;
	};

	/// A call expression.
	class CallExpression : public Expression {
	public:
		/// The function to call.
		Boxx::String func;

		/// The functin arguments.
		Boxx::List<Ptr<Value>> args;

		CallExpression(const Boxx::String& func) {
			this->func = func;
		}

		virtual Ptr<Interpreter::Value> Evaluate(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};

	/// An add expression.
	class AddExpression : public BinaryExpression {
	public:
		AddExpression(Ptr<Value> value1, Ptr<Value> value2) {
			this->value1 = value1;
			this->value2 = value2;
		}

		virtual Ptr<Interpreter::Value> Evaluate(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};
}

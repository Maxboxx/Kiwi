#pragma once

#include "Ptr.h"
#include "Node.h"

#include "Boxx/Boxx/Array.h"

///N Kiwi

namespace Kiwi {
	class Value;
	class Variable;

	/// A kiwi expression.
	class Expression : public Node {
	public:
		virtual void Interpret(Interpreter::InterpreterData& data) final override {
			throw Interpreter::KiwiInterpretError("Call Evaluate instead");
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

		/// The function arguments.
		Boxx::List<Ptr<Value>> args;

		CallExpression(const Boxx::String& func) {
			this->func = func;
		}

		virtual Ptr<Interpreter::Value> Evaluate(Interpreter::InterpreterData& data) override;
		Boxx::Array<Ptr<Interpreter::Value>> EvaluateAll(Interpreter::InterpreterData& data);
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};

	/// An alloc instruction.
	class AllocExpression : public Expression {
	public:
		Boxx::String type;

		AllocExpression(Boxx::String type) {
			this->type = type;
		}

		virtual Ptr<Interpreter::Value> Evaluate(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};

	/// A unary expression for numbers.
	class UnaryNumberExpression : public UnaryExpression {
	public:
		UnaryNumberExpression(Boxx::String instructionName, Ptr<Value> value) {
			this->instructionName = instructionName;
			this->value = value;
		}

		virtual Ptr<Interpreter::Value> Evaluate(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;

	protected:
		Boxx::String instructionName;

		virtual Boxx::Long Evaluate(Boxx::Long a) = 0;
	};

	/// A binary expression for numbers.
	class BinaryNumberExpression : public BinaryExpression {
	public:
		BinaryNumberExpression(Boxx::String instructionName, Ptr<Value> value1, Ptr<Value> value2) {
			this->instructionName = instructionName;
			this->value1 = value1;
			this->value2 = value2;
		}

		virtual Ptr<Interpreter::Value> Evaluate(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;

	protected:
		Boxx::String instructionName;

		virtual Boxx::Long Evaluate(Boxx::Long a, Boxx::Long b) = 0;
	};

	/// An negation expression.
	class NegExpression : public UnaryNumberExpression {
	public:
		NegExpression(Ptr<Value> value) : UnaryNumberExpression("neg", value) {

		}

		virtual Boxx::Long Evaluate(Boxx::Long a) override {
			return -a;
		}
	};

	/// A bitwise not expression.
	class BitNotExpression : public UnaryNumberExpression {
	public:
		BitNotExpression(Ptr<Value> value) : UnaryNumberExpression("not", value) {

		}

		virtual Boxx::Long Evaluate(Boxx::Long a) override {
			return ~a;
		}
	};

	/// An add expression.
	class AddExpression : public BinaryNumberExpression {
	public:
		AddExpression(Ptr<Value> value1, Ptr<Value> value2) : BinaryNumberExpression("add", value1, value2) {
			
		}

		virtual Boxx::Long Evaluate(Boxx::Long a, Boxx::Long b) override {
			return a + b;
		}
	};

	/// A subtract expression.
	class SubExpression : public BinaryNumberExpression {
	public:
		SubExpression(Ptr<Value> value1, Ptr<Value> value2) : BinaryNumberExpression("sub", value1, value2) {
			
		}

		virtual Boxx::Long Evaluate(Boxx::Long a, Boxx::Long b) override {
			return a - b;
		}
	};

	/// A multiplication expression.
	class MulExpression : public BinaryNumberExpression {
	public:
		MulExpression(Ptr<Value> value1, Ptr<Value> value2) : BinaryNumberExpression("mul", value1, value2) {

		}

		virtual Boxx::Long Evaluate(Boxx::Long a, Boxx::Long b) override {
			return a * b;
		}
	};

	/// A division expression.
	class DivExpression : public BinaryNumberExpression {
	public:
		DivExpression(Ptr<Value> value1, Ptr<Value> value2) : BinaryNumberExpression("div", value1, value2) {

		}

		virtual Boxx::Long Evaluate(Boxx::Long a, Boxx::Long b) override {
			return a / b;
		}
	};

	/// A modulus expression.
	class ModExpression : public BinaryNumberExpression {
	public:
		ModExpression(Ptr<Value> value1, Ptr<Value> value2) : BinaryNumberExpression("mod", value1, value2) {

		}

		virtual Boxx::Long Evaluate(Boxx::Long a, Boxx::Long b) override {
			return a % b;
		}
	};

	/// A bitwise or expression.
	class BitOrExpression : public BinaryNumberExpression {
	public:
		BitOrExpression(Ptr<Value> value1, Ptr<Value> value2) : BinaryNumberExpression("or", value1, value2) {

		}

		virtual Boxx::Long Evaluate(Boxx::Long a, Boxx::Long b) override {
			return a | b;
		}
	};

	/// A bitwise and expression.
	class BitAndExpression : public BinaryNumberExpression {
	public:
		BitAndExpression(Ptr<Value> value1, Ptr<Value> value2) : BinaryNumberExpression("and", value1, value2) {

		}

		virtual Boxx::Long Evaluate(Boxx::Long a, Boxx::Long b) override {
			return a & b;
		}
	};

	/// A bitwise xor expression.
	class BitXorExpression : public BinaryNumberExpression {
	public:
		BitXorExpression(Ptr<Value> value1, Ptr<Value> value2) : BinaryNumberExpression("xor", value1, value2) {

		}

		virtual Boxx::Long Evaluate(Boxx::Long a, Boxx::Long b) override {
			return a ^ b;
		}
	};

	/// A left shift expression.
	class LeftShiftExpression : public BinaryNumberExpression {
	public:
		LeftShiftExpression(Ptr<Value> value1, Ptr<Value> value2) : BinaryNumberExpression("shl", value1, value2) {

		}

		virtual Boxx::Long Evaluate(Boxx::Long a, Boxx::Long b) override {
			return a << b;
		}
	};

	/// A right shift expression.
	class RightShiftExpression : public BinaryNumberExpression {
	public:
		RightShiftExpression(Ptr<Value> value1, Ptr<Value> value2) : BinaryNumberExpression("shr", value1, value2) {

		}

		virtual Boxx::Long Evaluate(Boxx::Long a, Boxx::Long b) override {
			return a >> b;
		}
	};

	/// An equals expression.
	class EqualExpression : public BinaryNumberExpression {
	public:
		EqualExpression(Ptr<Value> value1, Ptr<Value> value2) : BinaryNumberExpression("eq", value1, value2) {

		}

		virtual Boxx::Long Evaluate(Boxx::Long a, Boxx::Long b) override {
			return a == b;
		}
	};

	/// A not equals expression.
	class NotEqualExpression : public BinaryNumberExpression {
	public:
		NotEqualExpression(Ptr<Value> value1, Ptr<Value> value2) : BinaryNumberExpression("ne", value1, value2) {

		}

		virtual Boxx::Long Evaluate(Boxx::Long a, Boxx::Long b) override {
			return a != b;
		}
	};

	/// A less than expression.
	class LessExpression : public BinaryNumberExpression {
	public:
		LessExpression(Ptr<Value> value1, Ptr<Value> value2) : BinaryNumberExpression("lt", value1, value2) {

		}

		virtual Boxx::Long Evaluate(Boxx::Long a, Boxx::Long b) override {
			return a < b;
		}
	};

	/// A greater than expression.
	class GreaterExpression : public BinaryNumberExpression {
	public:
		GreaterExpression(Ptr<Value> value1, Ptr<Value> value2) : BinaryNumberExpression("gt", value1, value2) {

		}

		virtual Boxx::Long Evaluate(Boxx::Long a, Boxx::Long b) override {
			return a > b;
		}
	};

	/// A less than or equal expression.
	class LessEqualExpression : public BinaryNumberExpression {
	public:
		LessEqualExpression(Ptr<Value> value1, Ptr<Value> value2) : BinaryNumberExpression("le", value1, value2) {

		}

		virtual Boxx::Long Evaluate(Boxx::Long a, Boxx::Long b) override {
			return a <= b;
		}
	};

	/// A greater than or equal expression.
	class GreaterEqualExpression : public BinaryNumberExpression {
	public:
		GreaterEqualExpression(Ptr<Value> value1, Ptr<Value> value2) : BinaryNumberExpression("ge", value1, value2) {

		}

		virtual Boxx::Long Evaluate(Boxx::Long a, Boxx::Long b) override {
			return a >= b;
		}
	};
}

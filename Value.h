#pragma once

#include "Boxx/Boxx/String.h"

#include "Expression.h"

///N Kiwi

namespace Kiwi {
	/// A Kiwi value.
	class Value : public Expression {
	public:
		virtual void BuildString(Boxx::StringBuilder& builder) override {
			builder += "unknown value";
		}
	};

	/// A Kiwi variable.
	class Variable : public Value {
	public:
		/// The variable name.
		Boxx::String name;

		Variable(const Boxx::String& name) {
			this->name = name;
		}

		virtual Ptr<Interpreter::Value> Evaluate(Interpreter::InterpreterData& data) override;

		virtual void BuildString(Boxx::StringBuilder& builder) override {
			if (name.Length() > 0) {
				builder += name;
			}
			else {
				builder += "invalid variable";
			}
		}
	};

	/// A Kiwi integer.
	class Integer : public Value {
	public:
		/// The integer value.
		Boxx::Long value;

		Integer(const Boxx::Long value) {
			this->value = value;
		}

		virtual Ptr<Interpreter::Value> Evaluate(Interpreter::InterpreterData& data) override;

		virtual void BuildString(Boxx::StringBuilder& builder) override {
			builder += Boxx::String::ToString(value);
		}
	};
}

#pragma once

#include "Boxx/Boxx/String.h"

#include "Expression.h"

namespace Kiwi {
	class Value : public Expression {
	public:
		virtual void BuildString(Boxx::StringBuilder& builder) override {
			builder += "unknown value";
		}
	};

	class Variable : public Value {
	public:
		Boxx::String name;

		Variable(const Boxx::String& name) {
			this->name = name;
		}

		virtual Ptr<Interpreter::Value> Evaluate(Interpreter::InterpreterData& data) override;

		virtual void BuildString(Boxx::StringBuilder& builder) override {
			builder += name;
		}
	};

	class Integer : public Value {
	public:
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

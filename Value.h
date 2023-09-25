#pragma once

#include "Boxx/Boxx/String.h"

#include "Expressions.h"

///N Kiwi

namespace Kiwi {
	/// A Kiwi value.
	class Value : public Expression {
	public:
		virtual void BuildString(Boxx::StringBuilder& builder) override {
			builder += "unknown value";
		}

		/// Copies the value.
		virtual Ptr<Value> CopyValue() const = 0;
	};

	/// A Kiwi variable.
	class Variable : public Value {
	public:
		/// The variable name.
		Boxx::String name;

		Variable(const Boxx::String& name) {
			this->name = name;
		}

		virtual Ptr<Value> CopyValue() const override final {
			return Copy();
		}

		/// Copies the variable.
		virtual Ptr<Variable> Copy() const {
			return new Variable(name);
		}

		/// Gets the type of the variable.
		virtual Type GetType(Interpreter::InterpreterData& data) const override {
			return data.frame->GetVarType(name);
		}
		
		/// Gets the ref of the variable.
		virtual Interpreter::DataPtr EvaluateRef(Interpreter::InterpreterData& data) const;

		virtual Interpreter::Data Evaluate(Interpreter::InterpreterData& data) override;

		virtual void BuildString(Boxx::StringBuilder& builder) override {
			builder += Name::ToKiwi(name);
		}
	};

	/// A Kiwi sub variable.
	class SubVariable : public Variable {
	public:
		/// The variable to access a sub variable of.
		Ptr<Variable> var;

		SubVariable(Ptr<Variable> var, const Boxx::String& subVar) : Variable(subVar) {
			this->var = var;
		}

		virtual Ptr<Variable> Copy() const override {
			return new SubVariable(var->Copy(), name);
		}

		virtual Type GetType(Interpreter::InterpreterData& data) const override;

		virtual Interpreter::DataPtr EvaluateRef(Interpreter::InterpreterData& data) const override;

		virtual Interpreter::Data Evaluate(Interpreter::InterpreterData& data) override;

		virtual void BuildString(Boxx::StringBuilder& builder) override {
			var->BuildString(builder);
			builder += '.';
			builder += Name::ToKiwi(name);
		}
	};

	/// A variable that is dereferenced.
	class DerefVariable : public Variable {
	public:
		DerefVariable(const Boxx::String& var) : Variable(var) {}

		virtual Ptr<Variable> Copy() const override {
			return new DerefVariable(name);
		}

		virtual Type GetType(Interpreter::InterpreterData& data) const override {
			Type t = Variable::GetType(data);
			t.pointers--;
			return t;
		}

		virtual Interpreter::DataPtr EvaluateRef(Interpreter::InterpreterData& data) const override;

		virtual Interpreter::Data Evaluate(Interpreter::InterpreterData& data) override;

		virtual void BuildString(Boxx::StringBuilder& builder) override {
			builder += '*';
			builder += Name::ToKiwi(name);
		}
	};

	/// A ref value.
	class RefValue : public Value {
	public:
		/// The variable to reference.
		Ptr<Variable> var;

		RefValue(Ptr<Variable> var) {
			this->var = var;
		}

		virtual Ptr<Value> CopyValue() const override {
			return new RefValue(var->Copy());
		} 

		virtual Type GetType(Interpreter::InterpreterData& data) const override {
			Type t = var->GetType(data);
			t.pointers++;
			return t;
		}

		virtual Interpreter::Data Evaluate(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};

	/// A Kiwi integer.
	class Integer : public Value {
	public:
		/// The integer value.
		Boxx::Long value;

		/// The integer type.
		Type type;

		Integer(Type type, const Boxx::Long value) {
			this->type  = type;
			this->value = value;
		}

		virtual Ptr<Value> CopyValue() const override {
			return new Integer(type, value);
		}

		virtual Type GetType(Interpreter::InterpreterData& data) const override {
			return type;
		}

		virtual Interpreter::Data Evaluate(Interpreter::InterpreterData& data) override;

		virtual void BuildString(Boxx::StringBuilder& builder) override {
			builder += Boxx::String::ToString(value);
		}
	};

	/// A Kiwi string.
	class StringValue : public Value {
	public:
		/// The string value.
		Boxx::String value;

		StringValue(const Boxx::String& value) {
			this->value = value;
		}

		virtual Ptr<Value> CopyValue() const override {
			return new StringValue(value);
		}

		virtual Type GetType(Interpreter::InterpreterData& data) const override {
			return Type(1, "u8");
		}

		virtual Interpreter::Data Evaluate(Interpreter::InterpreterData& data) override;

		virtual void BuildString(Boxx::StringBuilder& builder) override {
			builder += '"';
			builder += value.Escape();
			builder += '"';
		}
	};
}

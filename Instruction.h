#pragma once

#include "Ptr.h"
#include "Node.h"
#include "Value.h"

#include "Boxx/Boxx/String.h"
#include "Boxx/Boxx/Pointer.h"
#include "Boxx/Boxx/Optional.h"

///N Kiwi

namespace Kiwi {
	/// A kiwi instruction.
	class Instruction : public Node {
	public:
		/// {true} if the instruction reads from the specified variable.
		virtual bool ReadsFromVariable(const Boxx::String& var) const {
			return false;
		}

		/// {true} if the instruction writes to the specified variable.
		virtual bool WritesToVariable(const Boxx::String& var) const {
			return false;
		}

		/// {true} if the instruction reads from or writes to the specified variable.
		bool UsesVariable(const Boxx::String& var) const {
			return ReadsFromVariable(var) || WritesToVariable(var);
		}

		virtual void BuildString(Boxx::StringBuilder& builder) override {
			builder += "unknown instruction";
		}
	};

	/// An assignment instruction.
	class AssignInstruction : public Instruction {
	public:
		/// The type of the variable.
		Boxx::Optional<Boxx::String> type;

		/// The variable to assign to.
		Boxx::String var;

		/// The assign expression.
		Ptr<Expression> expression;

		AssignInstruction(const Boxx::String& var, Ptr<Expression> expression) {
			this->type = nullptr;
			this->var  = var;
			this->expression = expression;
		}

		AssignInstruction(const Boxx::String& type, const Boxx::String& var, Ptr<Expression> expression) {
			this->type = type;
			this->var  = var;
			this->expression = expression;
		}

		AssignInstruction(Weak<Variable> var, Ptr<Expression> expression) {
			this->type = nullptr;
			this->var  = var ? var->name : Boxx::String("");
			this->expression = expression;
		}

		AssignInstruction(const Boxx::String& type, Weak<Variable> var, Ptr<Expression> expression) {
			this->type = type;
			this->var  = var ? var->name : Boxx::String("");
			this->expression = expression;
		}

		virtual void Interpret(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};

	/// A call instruction.
	class CallInstruction : public Instruction {
	public:
		/// The call.
		Ptr<CallExpression> call;

		CallInstruction(Ptr<CallExpression> call) {
			this->call = call;
		}

		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};


	/// Base for instructions used for debugging.
	class DebugInstruction : public Instruction {
	public:
	
	};

	/// A print instruction for debugging.
	class DebugPrintInstruction : public DebugInstruction {
	public:
		/// The value to print.
		Ptr<Value> value;

		DebugPrintInstruction(Ptr<Value> value) {
			this->value = value;
		}

		virtual void Interpret(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};
}

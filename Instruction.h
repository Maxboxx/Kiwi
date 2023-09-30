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

	class MultiAssignInstruction;

	/// An assignment instruction.
	class AssignInstruction : public Instruction {
	public:
		/// The type of the variable.
		Boxx::Optional<Kiwi::Type> type;

		/// The variable to assign to.
		Ptr<Variable> var;

		/// The assign expression.
		Ptr<Expression> expression;

		AssignInstruction(const Boxx::String& var, Ptr<Expression> expression) {
			this->type = nullptr;
			this->var  = new Kiwi::Variable(var);
			this->expression = expression;
		}

		AssignInstruction(const Kiwi::Type& type, const Boxx::String& var, Ptr<Expression> expression) {
			this->type = type;
			this->var  = new Kiwi::Variable(var);
			this->expression = expression;
		}

		AssignInstruction(Ptr<Variable> var, Ptr<Expression> expression) {
			this->type = nullptr;
			this->var  = var;
			this->expression = expression;
		}

		AssignInstruction(const Kiwi::Type& type, Ptr<Variable> var, Ptr<Expression> expression) {
			this->type = type;
			this->var  = var;
			this->expression = expression;
		}

		virtual void Interpret(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;

	private:
		Ptr<MultiAssignInstruction> ToMultiAssign();
		void FreeMultiAssign(Ptr<MultiAssignInstruction> assign);
	};

	/// A multi assignment instruction.
	class MultiAssignInstruction : public Instruction {
	public:
		/// The types of the variables.
		Boxx::List<Boxx::Optional<Kiwi::Type>> types;

		/// The variable to assign to.
		Boxx::List<Ptr<Variable>> vars;

		/// The assign expression.
		Boxx::List<Ptr<Expression>> expressions;

		MultiAssignInstruction(const Boxx::List<Ptr<Variable>>& vars, const Boxx::List<Ptr<Expression>>& expressions) {
			this->types = Boxx::List<Boxx::Optional<Kiwi::Type>>();
			this->vars  = vars;
			this->expressions = expressions;
		}

		MultiAssignInstruction(const Boxx::List<Boxx::Optional<Kiwi::Type>>& types, const Boxx::List<Ptr<Variable>>& vars, const Boxx::List<Ptr<Expression>>& expressions) {
			this->types = types;
			this->vars  = vars;
			this->expressions = expressions;
		}

		virtual void Interpret(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};

	/// An assignment instruction to an offset.
	class OffsetAssignInstruction : public Instruction {
	public:
		Ptr<Variable> var;
		Ptr<Expression> expression;

		Boxx::Optional<Type> type;
		Ptr<Value> offset;

		OffsetAssignInstruction(const Ptr<Variable> var, Ptr<Expression> expression, Boxx::UInt offset) {
			this->var = var;
			this->expression = expression;
			this->offset = new Kiwi::Integer(Type("u32"), offset);
		}

		OffsetAssignInstruction(const Ptr<Variable> var, Ptr<Expression> expression, Type type, Boxx::UInt offset) {
			this->var = var;
			this->expression = expression;
			this->type = type;
			this->offset = new Kiwi::Integer(Type("u32"), offset);
		}

		OffsetAssignInstruction(const Ptr<Variable> var, Ptr<Expression> expression, Type type, Ptr<Value> offset) {
			this->var = var;
			this->expression = expression;
			this->type = type;
			this->offset = offset;
		}

		virtual void Interpret(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};

	/// A copy instruction.
	class CopyInstruction : public Instruction {
	public:
		Ptr<Value> dst, src, size;

		CopyInstruction(Ptr<Value> dst, Ptr<Value> src, Ptr<Value> size) {
			this->dst  = dst;
			this->src  = src;
			this->size = size;
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

		virtual void Interpret(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};

	/// A return instruction.
	class ReturnInstruction : public Instruction {
	public:
		ReturnInstruction() {}

		virtual void Interpret(Interpreter::InterpreterData& data) override {
			data.ret = true;
		}

		virtual void BuildString(Boxx::StringBuilder& builder) override {
			builder += "ret";
		}
	};

	/// A goto instruction.
	class GotoInstruction : public Instruction {
	public:
		/// The label to go to.
		Boxx::String label;

		GotoInstruction(const Boxx::String& label) {
			this->label = label;
		}

		virtual void Interpret(Interpreter::InterpreterData& data) override;

		virtual void BuildString(Boxx::StringBuilder& builder) override {
			builder += "goto ";
			builder += Name::ToKiwi(label);
		}
	};

	/// An if instruction.
	class IfInstruction : public Instruction {
	public:
		/// The condition
		Ptr<Expression> condition;

		/// The label to use if the condition is true.
		Boxx::Optional<Boxx::String> trueLabel;

		/// The label to use if the condition is false
		Boxx::Optional<Boxx::String> falseLabel;

		IfInstruction(Ptr<Expression> condition, const Boxx::String& label);
		IfInstruction(Ptr<Expression> condition, const Boxx::Optional<Boxx::String>& trueLabel, const Boxx::Optional<Boxx::String>& falseLabel);

		virtual void Interpret(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};

	/// Instruction for freeing up memory.
	class FreeInstruction : public Instruction {
	public:
		/// The condition
		Ptr<Value> value;

		FreeInstruction(Ptr<Value> value);

		virtual void Interpret(Interpreter::InterpreterData& data) override;
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

		/// The print type.
		Boxx::Optional<Boxx::String> type;

		DebugPrintInstruction(Ptr<Value> value) {
			this->value = value;
		}

		virtual void Interpret(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};
}

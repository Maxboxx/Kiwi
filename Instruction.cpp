#include "Instruction.h"

#include "Boxx/Boxx/Console.h"

using namespace Boxx;

using namespace Kiwi;

void AssignInstruction::Interpret(Interpreter::InterpreterData& data) {
	Weak<SubVariable> subVar = var.As<SubVariable>();

	if (type && !subVar) {
		data.frame->CreateVariable(var->name, *type);
	}

	if (!expression) {
		if (!type) {
			throw Interpreter::KiwiInterpretError("invalid expression to assign to '" + var->name + "'");
		}
		else {
			data.frame->SetVarValue(var->name, new Interpreter::StructValue(*type, data.program));
			return;
		}
	}

	Ptr<Interpreter::Value> value = expression->Evaluate(data);

	if (subVar) {
		Weak<Interpreter::StructValue> struct_ = subVar->var->EvaluateRef(data).As<Interpreter::StructValue>();

		if (struct_) {
			struct_->SetValue(subVar->name, value);
		}
		else {
			throw Interpreter::KiwiInterpretError("struct assign error");
		}
	}
	else if (Weak<DerefVariable> deref = var.As<DerefVariable>()) {
		Weak<Interpreter::Value> ptr = deref->EvaluateRef(data);

		if (!ptr->SetValue(value)) {
			throw Interpreter::KiwiInterpretError("deref assign error");
		}
	}
	else {
		data.frame->SetVarValue(var->name, value);
	}
}

void AssignInstruction::BuildString(Boxx::StringBuilder& builder) {
	if (type) {
		builder += (*type).ToKiwi();
		builder += ": ";
	}

	if (var) {
		var->BuildString(builder);
	}
	else {
		builder += "invalid variable";
	}
	
	if (type && !expression) return;

	builder += " = ";

	if (expression) {
		expression->BuildString(builder);
	}
	else {
		builder += "invalid expression";
	}
}

void CallInstruction::Interpret(Interpreter::InterpreterData& data) {
	call->Evaluate(data);
}

void CallInstruction::BuildString(Boxx::StringBuilder& builder) {
	call->BuildString(builder);
}

void DebugPrintInstruction::Interpret(Interpreter::InterpreterData& data) {
	if (Ptr<Interpreter::Value> value = this->value->Evaluate(data)) {
		Console::Print(value->ToString());
	}
	else {
		Console::Print("null");
	}
}

void DebugPrintInstruction::BuildString(StringBuilder& builder) {
	builder += "_print ";

	if (value) {
		value->BuildString(builder);
	}
	else {
		builder += "invalid value";
	}
}


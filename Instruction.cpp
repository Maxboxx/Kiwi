#include "Instruction.h"

#include "Boxx/Boxx/Console.h"

using namespace Boxx;

using namespace Kiwi;

void AssignInstruction::Interpret(Interpreter::InterpreterData& data) {
	if (type) {
		data.frame->SetVarType(var, *type);
	}

	Ptr<Interpreter::Value> value = expression->Evaluate(data);

	if (value) {
		String valueType = value->GetType();

		value = value->ConvertTo(data.frame->GetVarType(var));
		
		if (!value) {
			throw Interpreter::KiwiInterpretError("can not convert value of type '" + valueType + "' to '" + data.frame->GetVarType(var) + "'");
		}
	}

	data.frame->SetVarValue(var, value);
}

void AssignInstruction::BuildString(Boxx::StringBuilder& builder) {
	if (type) {
		builder += *type;
		builder += ": ";
	}

	builder += Name::ToKiwi(var);
	
	builder += " = ";

	if (expression) {
		expression->BuildString(builder);
	}
	else {
		builder += "invalid expression";
	}
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
	value->BuildString(builder);
}


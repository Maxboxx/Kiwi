#include "Instruction.h"

#include "Boxx/Boxx/Console.h"

using namespace Boxx;

using namespace Kiwi;

void AssignInstruction::Interpret(Interpreter::InterpreterData& data) {
	data.frame->SetVarValue(var, expression->Evaluate(data));
}

void AssignInstruction::BuildString(Boxx::StringBuilder& builder) {
	if (type) {
		builder += *type;
		builder += ": ";
	}

	builder += var;
	builder += " = ";

	expression->BuildString(builder);
}


void CallInstruction::BuildString(Boxx::StringBuilder& builder) {
	call->BuildString(builder);
}

void DebugPrintInstruction::Interpret(Interpreter::InterpreterData& data) {
	if (Ptr<Interpreter::Value> value = this->value->Evaluate(data)) {
		Console::Print(value->ToString());
	}
}

void DebugPrintInstruction::BuildString(StringBuilder& builder) {
	builder += "_print ";
	value->BuildString(builder);
}


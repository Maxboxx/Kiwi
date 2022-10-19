#include "Expression.h"

#include "Value.h"
#include "KiwiProgram.h"

#include "Boxx/Boxx/Array.h"

using namespace Boxx;

using namespace Kiwi;

Ptr<Interpreter::Value> CallExpression::Evaluate(Interpreter::InterpreterData& data) {
	Weak<Function> function = data.program->functions[func];

	Array<Ptr<Interpreter::Value>> argValues(args.Size());

	for (int i = 0; i < args.Size(); i++) {
		argValues[i] = args[i]->Evaluate(data);
	}

	data.PushFrame();

	for (int i = 0; i < args.Size(); i++) {
		data.frame->SetVarValue(function->arguments[i].value2, argValues[i]);
	}

	function->block->Interpret(data);

	Ptr<Interpreter::Value> ret = data.frame->GetVarValueCopy(function->returnValues[0].value2);

	data.PopFrame();

	return ret;
}

void CallExpression::BuildString(StringBuilder& builder) {
	builder += "call ";
	builder += func;
	builder += '(';

	for (Int i = 0; i < args.Size(); i++) {
		if (i > 0) builder += ", ";
		args[i]->BuildString(builder);
	}

	builder += ')';
}

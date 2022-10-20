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
		Tuple<String, String> arg = function->arguments[i];
		data.frame->SetVarType(arg.value2, arg.value1);
		data.frame->SetVarValue(arg.value2, argValues[i]);
	}

	for (int i = 0; i < function->returnValues.Size(); i++) {
		Tuple<String, String> value = function->returnValues[i];
		data.frame->SetVarType(value.value2, value.value1);
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


Ptr<Interpreter::Value> AddExpression::Evaluate(Interpreter::InterpreterData& data) {
	Ptr<Interpreter::Value> a = value1->Evaluate(data); 
	Ptr<Interpreter::Value> b = value2->Evaluate(data); 

	if (Weak<Interpreter::Integer> int1 = a.As<Interpreter::Integer>()) {
		if (Weak<Interpreter::Integer> int2 = b.As<Interpreter::Integer>()) {
			return new Interpreter::Int64(int1->ToLong() + int2->ToLong());
		}
	}

	return nullptr;
}

void AddExpression::BuildString(StringBuilder& builder) {
	builder += "add ";
	value1->BuildString(builder);
	builder += ", ";
	value2->BuildString(builder);
}

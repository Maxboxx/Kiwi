#include "Expressions.h"

#include "Value.h"
#include "KiwiProgram.h"

#include "Boxx/Boxx/Array.h"

using namespace Boxx;

using namespace Kiwi;

Ptr<Interpreter::Value> CallExpression::Evaluate(Interpreter::InterpreterData& data) {
	Array<Ptr<Interpreter::Value>> ret = EvaluateAll(data);

	if (ret.Length() > 0) {
		return ret[0];
	}

	return nullptr;
}

Array<Ptr<Interpreter::Value>> CallExpression::EvaluateAll(Interpreter::InterpreterData& data) {
	Weak<Function> function = data.program->functions[func];

	Array<Ptr<Interpreter::Value>> argValues(args.Count());

	for (int i = 0; i < args.Count(); i++) {
		argValues[i] = args[i]->Evaluate(data);
	}

	data.PushFrame();

	for (int i = 0; i < args.Count(); i++) {
		Tuple<Type, String> arg = function->arguments[i];
		data.frame->CreateVariable(arg.value2, arg.value1);
		data.frame->SetVarValue(arg.value2, argValues[i]);
	}

	for (int i = 0; i < function->returnValues.Count(); i++) {
		Tuple<Type, String> value = function->returnValues[i];
		data.frame->CreateVariable(value.value2, value.value1);

		if (value.value1.pointers == 0) {
			if (data.program->structs.Contains(value.value1.name)) {
				data.frame->SetVarValue(value.value2, new Interpreter::StructValue(value.value1, data.program));
			}
		}
	}

	function->block->Interpret(data);

	Array<Ptr<Interpreter::Value>> ret = Array<Ptr<Interpreter::Value>>(function->returnValues.Count());

	for (UInt i = 0; i < function->returnValues.Count(); i++) {
		ret[i] = data.frame->GetVarValueCopy(function->returnValues[i].value2);
	}

	data.PopFrame();

	return ret;
}

void CallExpression::BuildString(StringBuilder& builder) {
	builder += "call ";
	builder += Name::ToKiwi(func);
	builder += '(';

	for (Int i = 0; i < args.Count(); i++) {
		if (i > 0) builder += ", ";
		args[i]->BuildString(builder);
	}

	builder += ')';
}

Ptr<Interpreter::Value> AllocExpression::Evaluate(Interpreter::InterpreterData& data) {
	Ptr<Interpreter::StructValue> value = new Interpreter::StructValue(Kiwi::Type(type), data.program);
	Weak<Interpreter::StructValue> weak = value;
	UInt adr = data.heap->Alloc(value);
	return new Interpreter::PtrValue(weak->type, weak);
}

void AllocExpression::BuildString(StringBuilder& builder) {
	builder += "alloc ";
	builder += Name::ToKiwi(type);
}

Ptr<Interpreter::Value> UnaryNumberExpression::Evaluate(Interpreter::InterpreterData& data) {
	Ptr<Interpreter::Value> a = value->Evaluate(data); 

	if (Weak<Interpreter::Integer> int1 = a.As<Interpreter::Integer>()) {
		return new Interpreter::Int64(Evaluate(int1->ToLong()));
	}

	return nullptr;
}

void UnaryNumberExpression::BuildString(StringBuilder& builder) {
	builder += instructionName;
	builder += " ";
	value->BuildString(builder);
}

Ptr<Interpreter::Value> BinaryNumberExpression::Evaluate(Interpreter::InterpreterData& data) {
	Ptr<Interpreter::Value> a = value1->Evaluate(data); 
	Ptr<Interpreter::Value> b = value2->Evaluate(data); 

	if (Weak<Interpreter::Integer> int1 = a.As<Interpreter::Integer>()) {
		if (Weak<Interpreter::Integer> int2 = b.As<Interpreter::Integer>()) {
			return new Interpreter::Int64(Evaluate(int1->ToLong(), int2->ToLong()));
		}
	}

	return nullptr;
}

void BinaryNumberExpression::BuildString(StringBuilder& builder) {
	builder += instructionName;
	builder += " ";
	value1->BuildString(builder);
	builder += ", ";
	value2->BuildString(builder);
}

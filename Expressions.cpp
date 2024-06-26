#include "Expressions.h"

#include "Value.h"
#include "KiwiProgram.h"

#include "Boxx/Boxx/Array.h"

using namespace Boxx;

using namespace Kiwi;

Type CallExpression::GetType(Interpreter::InterpreterData& data) const {
	Weak<Function> function = data.program->functions[func];

	if (function->returnValues.IsEmpty()) return Type();
	return function->returnValues[0].value1;
}

Interpreter::Data CallExpression::Evaluate(Interpreter::InterpreterData& data) {
	Array<Interpreter::Data> ret = EvaluateAll(data);

	if (ret.Length() > 0) {
		return ret[0];
	}

	return Interpreter::Data();
}

Array<Interpreter::Data> CallExpression::EvaluateAll(Interpreter::InterpreterData& data) {
	String funcName = func;

	if (funcPtr) {
		Interpreter::Data ptr = funcPtr->Evaluate(data);
		funcName = data.funcNameMap[ptr.GetNumber(KiwiProgram::ptrSize)];
	}

	Weak<Function> function = data.program->functions[funcName];

	Array<Interpreter::Data> argValues(args.Count());

	for (UInt i = 0; i < args.Count(); i++) {
		argValues[i] = args[i]->Evaluate(data);
	}

	data.PushFrame();

	for (UInt i = 0; i < args.Count(); i++) {
		Tuple<Type, String> arg = function->arguments[i];
		data.frame->CreateVariable(arg.value2, arg.value1);
		data.frame->SetVarValue(arg.value2, argValues[i]);
	}

	for (UInt i = 0; i < function->returnValues.Count(); i++) {
		Tuple<Type, String> value = function->returnValues[i];
		data.frame->CreateVariable(value.value2, value.value1);

		if (value.value1.pointers == 0) {
			if (data.program->structs.Contains(value.value1.name)) {
				data.frame->SetVarValue(value.value2, Interpreter::Data(Type::SizeOf(value.value1, data.program)));
			}
		}
	}

	function->block->Interpret(data);

	Array<Interpreter::Data> ret = Array<Interpreter::Data>(function->returnValues.Count());

	for (UInt i = 0; i < function->returnValues.Count(); i++) {
		ret[i] = data.frame->GetVarValueCopy(function->returnValues[i].value2);
	}

	data.PopFrame();

	return ret;
}

void CallExpression::BuildString(StringBuilder& builder) {
	builder += "call ";

	if (funcPtr) {
		funcPtr->BuildString(builder);
	}
	else {
		builder += Name::ToKiwi(func);
	}

	builder += '(';

	for (UInt i = 0; i < args.Count(); i++) {
		if (i > 0) builder += ", ";
		args[i]->BuildString(builder);
	}

	builder += ')';
}

Type AllocExpression::GetType(Interpreter::InterpreterData& data) const {
	Type type = this->type.ValueOr(Type());
	type.pointers++;
	return type;
}

Interpreter::Data AllocExpression::Evaluate(Interpreter::InterpreterData& data) {
	UInt size = this->size;

	if (type) {
		size = Type::SizeOf(Type(*type), data.program);
	}

	if (var) {
		Interpreter::Data varData = var->Evaluate(data);
		size = (UInt)varData.GetNumber(varData.Size());
	}

	Interpreter::Data allocData = data.heap->Alloc(size);
	Interpreter::DataPtr ptr = allocData.Ptr();
	Interpreter::Data value  = Interpreter::Data(KiwiProgram::ptrSize);
	value.Set(ptr);
	return value;
}

void AllocExpression::BuildString(StringBuilder& builder) {
	builder += "alloc ";

	if (type) {
		builder += type->ToKiwi();
	}
	else if (var) {
		var->BuildString(builder);
	}
	else {
		builder += String::ToString(size);
	}
}

OffsetExpression::OffsetExpression(Ptr<Variable> var, Type type, Ptr<Value> offset) {
	this->var = var;
	this->offset = offset;
	this->type = type;
}

OffsetExpression::OffsetExpression(Ptr<Variable> var, Type type, Type offsetType, Ptr<Value> offset) {
	this->var = var;
	this->offset = offset;
	this->type = type;
	this->offsetType = offsetType;
}

Type OffsetExpression::GetType(Interpreter::InterpreterData& data) const {
	return type;
}

Interpreter::Data OffsetExpression::Evaluate(Interpreter::InterpreterData& data) {
	Interpreter::DataPtr ptr = var->EvaluateRef(data);

	UInt offsetSize = offsetType ? Type::SizeOf(*offsetType, data.program) : 1;
	ptr += offsetSize * offset->Evaluate(data).GetNumber(Type::SizeOf(offset->GetType(data), data.program));

	Interpreter::Data d = Interpreter::Data(ptr, Type::SizeOf(var->GetType(data), data.program));
	return d;
}

void OffsetExpression::BuildString(StringBuilder& builder) {
	var->BuildString(builder);
	builder += '[';

	if (offsetType) {
		builder += (*offsetType).ToKiwi();
		builder += ' ';
	}

	offset->BuildString(builder);
	builder += ']';
}

Type UnaryNumberExpression::GetType(Interpreter::InterpreterData& data) const {
	return value->GetType(data);
}

Interpreter::Data UnaryNumberExpression::Evaluate(Interpreter::InterpreterData& data) {
	return value->Evaluate(data);
}

void UnaryNumberExpression::BuildString(StringBuilder& builder) {
	builder += instructionName;
	builder += " ";
	value->BuildString(builder);
}

Type BinaryNumberExpression::GetType(Interpreter::InterpreterData& data) const {
	Type t1 = value1->GetType(data);
	Type t2 = value2->GetType(data);
	return Type::SizeOf(t1, data.program) > Type::SizeOf(t2, data.program) ? t1 : t2;
}

Interpreter::Data BinaryNumberExpression::Evaluate(Interpreter::InterpreterData& data) {
	Interpreter::Data a = value1->Evaluate(data); 
	Interpreter::Data b = value2->Evaluate(data); 

	return Interpreter::Data::Number(
		Type::SizeOf(GetType(data), data.program),
		Evaluate(
			a.GetNumber(Type::SizeOf(value1->GetType(data), data.program)),
			b.GetNumber(Type::SizeOf(value2->GetType(data), data.program))
		)
	);
}

void BinaryNumberExpression::BuildString(StringBuilder& builder) {
	builder += instructionName;
	builder += " ";
	value1->BuildString(builder);
	builder += ", ";
	value2->BuildString(builder);
}

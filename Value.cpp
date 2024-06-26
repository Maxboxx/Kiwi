#include "Value.h"
#include "KiwiProgram.h"

using namespace Boxx;

using namespace Kiwi;

Interpreter::DataPtr Variable::EvaluateRef(Interpreter::InterpreterData& data) const {
	return data.frame->GetVarValue(name).Ptr();
}

Interpreter::Data Variable::Evaluate(Interpreter::InterpreterData& data) {
	Interpreter::Data d;
	UInt id;

	if (data.staticData.Contains(name, d)) {
		Interpreter::Data sd = Interpreter::Data(KiwiProgram::ptrSize);
		sd.Set(d.Ptr());
		return sd;
	}
	else if (data.funcIdMap.Contains(name, id)) {
		return Interpreter::Data::Number(KiwiProgram::ptrSize, id);
	}
	else if (data.frame) {
		return data.frame->GetVarValueCopy(name);
	}

	return d;
}

Type SubVariable::GetType(Interpreter::InterpreterData& data) const {
	if (!data.program->structs.Contains(var->GetType(data).name)) {
		return Type();
	}

	return data.program->structs[var->GetType(data).name]->VarType(name);
}

Interpreter::DataPtr SubVariable::EvaluateRef(Interpreter::InterpreterData& data) const {
	Interpreter::DataPtr struct_ = var->EvaluateRef(data);

	Type type = var->GetType(data);

	if (data.program->structs.Contains(type.name)) {
		return struct_ + data.program->structs[type.name]->VarOffset(name, data.program);
	}

	return nullptr;
}

Interpreter::Data SubVariable::Evaluate(Interpreter::InterpreterData& data) {
	Interpreter::DataPtr ptr = EvaluateRef(data);
	return Interpreter::Data(ptr, Type::SizeOf(GetType(data), data.program));
}

Interpreter::DataPtr DerefVariable::EvaluateRef(Interpreter::InterpreterData& data) const {
	return data.frame->GetVarValue(name).Get<Interpreter::DataPtr>();
}

Interpreter::Data DerefVariable::Evaluate(Interpreter::InterpreterData& data) {
	Interpreter::DataPtr ptr = data.frame->GetVarValue(name).Get<Interpreter::DataPtr>();
	return Interpreter::Data(ptr, Type::SizeOf(GetType(data), data.program));
}

Interpreter::Data RefValue::Evaluate(Interpreter::InterpreterData& data) {
	if (!var) {
		throw Interpreter::KiwiInterpretError("invalid value to reference");
	}

	Interpreter::DataPtr value = var->EvaluateRef(data);
	Interpreter::Data d = Interpreter::Data(KiwiProgram::ptrSize);
	d.Set(value);
	return d;
}

void RefValue::BuildString(StringBuilder& builder) {
	builder += '&';

	if (var) {
		var->BuildString(builder);
	}
	else {
		builder += "invalid value";
	}
}

Interpreter::Data Integer::Evaluate(Interpreter::InterpreterData& data) {
	return Interpreter::Data::Number(Type::SizeOf(type, data.program), value);
}

Interpreter::Data Kiwi::StringValue::Evaluate(Interpreter::InterpreterData& data) {
	Interpreter::Data str = data.heap->Alloc(value.Length());
	std::memcpy(str.Ptr(), (const char*)value, value.Length());

	Interpreter::Data strPtr = Interpreter::Data(KiwiProgram::ptrSize);
	strPtr.Set(str.Ptr());
	return strPtr;
}

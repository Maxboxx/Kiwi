#include "Value.h"

using namespace Boxx;

using namespace Kiwi;

Weak<Interpreter::Value> Variable::EvaluateRef(Interpreter::InterpreterData& data) const {
	return data.frame->GetVarValue(name);
}

Ptr<Interpreter::Value> Variable::Evaluate(Interpreter::InterpreterData& data) {
	return data.frame->GetVarValueCopy(name);
}

Weak<Interpreter::Value> SubVariable::EvaluateRef(Interpreter::InterpreterData& data) const {
	Weak<Interpreter::StructValue> struct_ = var->EvaluateRef(data).As<Interpreter::StructValue>();

	if (struct_->data.Contains(name)) {
		return struct_->data[name].As<Interpreter::StructValue>();
	}
	
	return nullptr;
}

Ptr<Interpreter::Value> SubVariable::Evaluate(Interpreter::InterpreterData& data) {
	Ptr<Interpreter::StructValue> var = this->var->Evaluate(data).AsPtr<Interpreter::StructValue>();

	if (var) {
		Weak<Interpreter::Value> value = var->GetValue(name);

		if (value) {
			return value->Copy();
		}
	}

	return nullptr;
}

Weak<Interpreter::Value> DerefVariable::EvaluateRef(Interpreter::InterpreterData& data) const {
	Weak<Interpreter::PtrValue> ptr = data.frame->GetVarValue(name).As<Interpreter::PtrValue>();

	if (ptr) {
		return ptr->value;
	}

	return nullptr;
}

Ptr<Interpreter::Value> DerefVariable::Evaluate(Interpreter::InterpreterData& data) {
	Weak<Interpreter::PtrValue> ptr = data.frame->GetVarValue(name).As<Interpreter::PtrValue>();

	if (ptr) {
		return ptr->value->Copy();
	}

	return nullptr;
}

Ptr<Interpreter::Value> RefValue::Evaluate(Interpreter::InterpreterData& data) {
	if (!var) {
		throw Interpreter::KiwiInterpretError("invalid value to reference");
	}

	Weak<Interpreter::Value> value = var->EvaluateRef(data);

	if (!value) {
		StringBuilder builder;
		var->BuildString(builder);
		throw Interpreter::KiwiInterpretError("can not reference '" + builder.ToString() + "'");
	}

	Type type = value->GetType();
	type.pointers++;
	return new Interpreter::PtrValue(type, value);
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

Ptr<Interpreter::Value> Integer::Evaluate(Interpreter::InterpreterData& data) {
	return new Interpreter::Int64(value);
}

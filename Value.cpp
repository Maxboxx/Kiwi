#include "Value.h"

using namespace Boxx;

using namespace Kiwi;

Weak<Interpreter::StructValue> Variable::GetStructRef(Interpreter::InterpreterData& data) const {
	return data.frame->GetVarValue(name).As<Interpreter::StructValue>();
}

Ptr<Interpreter::Value> Variable::Evaluate(Interpreter::InterpreterData& data) {
	return data.frame->GetVarValueCopy(name);
}

Weak<Interpreter::StructValue> SubVariable::GetStructRef(Interpreter::InterpreterData& data) const {
	Weak<Interpreter::StructValue> struct_ = var->GetStructRef(data);

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

Ptr<Interpreter::Value> Integer::Evaluate(Interpreter::InterpreterData& data) {
	return new Interpreter::Int64(value);
}

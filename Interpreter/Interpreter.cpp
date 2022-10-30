#include "Interpreter.h"

#include "../KiwiProgram.h"

using namespace Kiwi;
using namespace Kiwi::Interpreter;

void StructValue::SetValue(const Boxx::String& var, Ptr<Value> value) {
	for (const Boxx::Tuple<Type, Boxx::String>& name : program->structs[type.name]->vars) {
		if (name.value2 == var) {
			Ptr<Value> cvalue = value->ConvertTo(name.value1);

			if (!cvalue) {
				throw Interpreter::KiwiInterpretError("can not convert value of type '" + value->GetType().ToKiwi() + "' to '" + name.value1.ToKiwi() + "'");
			}

			data.Set(var, cvalue);
			return;
		}
	}

	throw Interpreter::KiwiInterpretError("struct '" + type.ToKiwi() + "' has no member '" + var + "'");
}

Boxx::String StructValue::ToString() const {
	Boxx::StringBuilder builder;
	builder += "{\n";

	for (const Boxx::Tuple<Type, Boxx::String>& name : program->structs[type.name]->vars) {
		builder += "  ";
		builder += name.value2;
		builder += " = ";
		
		if (Weak<Value> value = GetValue(name.value2)) {
			builder += value->ToString();
		}
		else {
			builder += "null";
		}

		builder += '\n';
	}

	builder += '}';

	return builder.ToString();
}

Boxx::String PtrValue::ToString() const {
	return '*' + value->ToString();
}

#include "Interpreter.h"

#include "../KiwiProgram.h"

using namespace Kiwi;
using namespace Kiwi::Interpreter;

void StructValue::SetValue(const Boxx::String& var, Ptr<Value> value) {
	for (const Boxx::Tuple<Boxx::String, Boxx::String>& name : program->structs[type]->vars) {
		if (name.value2 == var) {
			Ptr<Value> cvalue = value->ConvertTo(name.value1);

			if (!cvalue) {
				throw Interpreter::KiwiInterpretError("can not convert value of type '" + value->GetType() + "' to '" + name.value1 + "'");
			}

			data.Set(var, cvalue);
			return;
		}
	}

	throw Interpreter::KiwiInterpretError("struct '" + type + "' has no member '" + var + "'");
}

Boxx::String StructValue::ToString() const {
	Boxx::StringBuilder builder;
	builder += "{\n";

	for (const Boxx::Tuple<Boxx::String, Boxx::String>& name : program->structs[type]->vars) {
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

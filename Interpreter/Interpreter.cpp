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
	return ToString(1);
}

Boxx::String StructValue::ToString(Boxx::UInt indent) const {
	Boxx::StringBuilder builder;
	builder += "{\n";

	for (const Boxx::Tuple<Type, Boxx::String>& name : program->structs[type.name]->vars) {
		builder += Boxx::String("  ").Repeat(indent);
		builder += name.value2;
		builder += " = ";
		
		if (Weak<Value> value = GetValue(name.value2)) {
			if (Weak<StructValue> sv = value.As<StructValue>()) {
				builder += sv->ToString(indent + 1);
			}
			else {
				builder += value->ToString();
			}
		}
		else {
			builder += "null";
		}

		builder += '\n';
	}

	builder += Boxx::String("  ").Repeat(indent - 1);
	builder += '}';

	return builder.ToString();
}

Boxx::String PtrValue::ToString() const {
	return '*' + value->ToString();
}

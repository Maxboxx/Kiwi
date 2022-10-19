#include "Value.h"

using namespace Boxx;

using namespace Kiwi;

Ptr<Interpreter::Value> Variable::Evaluate(Interpreter::InterpreterData& data) {
	return data.frame->GetVarValueCopy(name);
}

Ptr<Interpreter::Value> Integer::Evaluate(Interpreter::InterpreterData& data) {
	return new Interpreter::Int64(value);
}

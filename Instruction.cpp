#include "Instruction.h"

#include "KiwiProgram.h"

#include "Boxx/Boxx/Console.h"

using namespace Boxx;

using namespace Kiwi;

void AssignInstruction::Interpret(Interpreter::InterpreterData& data) {
	Weak<SubVariable> subVar = var.As<SubVariable>();

	if (type && !subVar) {
		data.frame->CreateVariable(var->name, *type);
	}

	if (!expression) {
		if (!type) {
			throw Interpreter::KiwiInterpretError("invalid expression to assign to '" + var->name + "'");
		}
		else {
			data.frame->SetVarValue(var->name, new Interpreter::StructValue(*type, data.program));
			return;
		}
	}

	Ptr<Interpreter::Value> value = expression->Evaluate(data);

	if (subVar) {
		Weak<Interpreter::StructValue> struct_ = subVar->var->EvaluateRef(data).As<Interpreter::StructValue>();

		if (struct_) {
			struct_->SetValue(subVar->name, value);
		}
		else {
			throw Interpreter::KiwiInterpretError("struct assign error");
		}
	}
	else if (Weak<DerefVariable> deref = var.As<DerefVariable>()) {
		Weak<Interpreter::Value> ptr = deref->EvaluateRef(data);

		if (!ptr->SetValue(value)) {
			throw Interpreter::KiwiInterpretError("deref assign error");
		}
	}
	else {
		data.frame->SetVarValue(var->name, value);
	}
}

void AssignInstruction::BuildString(Boxx::StringBuilder& builder) {
	if (type) {
		builder += (*type).ToKiwi();
		builder += ": ";
	}

	if (var) {
		var->BuildString(builder);
	}
	else {
		builder += "invalid variable";
	}
	
	if (type && !expression) return;

	builder += " = ";

	if (expression) {
		expression->BuildString(builder);
	}
	else {
		builder += "invalid expression";
	}
}

void CallInstruction::Interpret(Interpreter::InterpreterData& data) {
	call->Evaluate(data);
}

void CallInstruction::BuildString(Boxx::StringBuilder& builder) {
	call->BuildString(builder);
}

void GotoInstruction::Interpret(Interpreter::InterpreterData& data) {
	data.gotoLabel = label;
}

IfInstruction::IfInstruction(Ptr<Expression> condition, const Boxx::String& label) {
	this->condition  = condition;
	this->trueLabel  = label;
	this->falseLabel = nullptr;
}

IfInstruction::IfInstruction(Ptr<Expression> condition, const Boxx::String& trueLabel, const Boxx::String& falseLabel) {
	this->condition  = condition;
	this->trueLabel  = trueLabel;
	this->falseLabel = falseLabel;
}

void IfInstruction::Interpret(Interpreter::InterpreterData& data) {
	Ptr<Interpreter::Value> value = condition->Evaluate(data);

	if (Weak<Interpreter::Integer> integer = value.As<Interpreter::Integer>()) {
		if (integer->ToLong() != 0) {
			data.gotoLabel = trueLabel;
		}
		else if (falseLabel) {
			data.gotoLabel = *falseLabel;
		}
	}
	else {
		throw Interpreter::KiwiInterpretError("invalid condition");
	}
}

void IfInstruction::BuildString(Boxx::StringBuilder& builder) {
	builder += "if ";
	condition->BuildString(builder);
	builder += ": ";
	builder += Name::ToKiwi(trueLabel);

	if (falseLabel) {
		builder += ", ";
		builder += Name::ToKiwi(*falseLabel);
	}
}

void DebugPrintInstruction::Interpret(Interpreter::InterpreterData& data) {
	if (Ptr<Interpreter::Value> value = this->value->Evaluate(data)) {
		Console::Print(value->ToString());
	}
	else {
		Console::Print("null");
	}
}

void DebugPrintInstruction::BuildString(StringBuilder& builder) {
	builder += "_print ";

	if (value) {
		value->BuildString(builder);
	}
	else {
		builder += "invalid value";
	}
}


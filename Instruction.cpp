#include "Instruction.h"

#include "KiwiProgram.h"

#include "Boxx/Boxx/Console.h"

using namespace Boxx;

using namespace Kiwi;

Ptr<MultiAssignInstruction> AssignInstruction::ToMultiAssign() {
	Ptr<MultiAssignInstruction> assign;

	List<Ptr<Variable>> vars;
	vars.Add(var->Copy());

	List<Ptr<Expression>> expressions;

	if (expression) {
		expressions.Add(expression);
	}

	if (type) {
		List<Optional<Type>> types;
		types.Add(type);
		assign = new MultiAssignInstruction(types, vars, expressions);
	}
	else {
		assign = new MultiAssignInstruction(vars, expressions);
	}

	return assign;
}

void AssignInstruction::FreeMultiAssign(Ptr<MultiAssignInstruction> assign) {
	if (assign->expressions.Count() > 0) {
		expression = assign->expressions[0];
	}
}

void AssignInstruction::Interpret(Interpreter::InterpreterData& data) {
	Ptr<MultiAssignInstruction> assign = ToMultiAssign();
	assign->Interpret(data);
	FreeMultiAssign(assign);
}

void AssignInstruction::BuildString(Boxx::StringBuilder& builder) {
	Ptr<MultiAssignInstruction> assign = ToMultiAssign();
	assign->BuildString(builder);
	FreeMultiAssign(assign);
}

void MultiAssignInstruction::Interpret(Interpreter::InterpreterData& data) {
	Array<Interpreter::Data> extraValues;

	for (UInt i = 0; i < vars.Count(); i++) {
		Optional<Type> type = nullptr;
		
		if (types.Count() > 0) {
			type = types.Count() == 1 ? types[0] : types[i];
		}

		Weak<Variable> var = vars[i];
		Weak<SubVariable> subVar = var.As<SubVariable>();

		Weak<Expression> expression = nullptr;

		if (expressions.Count() > 0) {
			if (i < expressions.Count()) {
				expression = expressions[i];
			}
			else {
				expression = expressions.Last();
			}
		}

		if (type && !subVar) {
			data.frame->CreateVariable(var->name, *type);
		}

		if (!expression) {
			if (!type) {
				throw Interpreter::KiwiInterpretError("invalid expression to assign to '" + var->name + "'");
			}
			else {
				data.frame->SetVarValue(var->name, Interpreter::Data(Type::SizeOf(*type, data.program)));
				return;
			}
		}

		Interpreter::Data value;
		
		if (i < expressions.Count()) {
			if (i == expressions.Count() - 1 && expression.Is<CallExpression>()) {
				extraValues = expression.As<CallExpression>()->EvaluateAll(data);

				if (extraValues.Length() > 0) {
					value = extraValues[0];
				}
			}
			else {
				value = expression->Evaluate(data);
			}
		}
		else if (extraValues.Length() > expressions.Count() - i) {
			value = extraValues[expressions.Count() - i + 1];
		}

		if (subVar) {
			Interpreter::DataPtr struct_ = subVar->EvaluateRef(data);
			Interpreter::Data::Set(struct_, value);
		}
		else if (Weak<DerefVariable> deref = var.As<DerefVariable>()) {
			Interpreter::DataPtr struct_ = subVar->EvaluateRef(data);
			Interpreter::Data::Set(struct_, value);
		}
		else {
			data.frame->SetVarValue(var->name, value);
		}
	}
}

void MultiAssignInstruction::BuildString(Boxx::StringBuilder& builder) {
	if (types.Count() > 0) {
		for (UInt i = 0; i < types.Count(); i++) {
			if (i > 0) builder += ", ";

			if (Optional<Type> type = types[i]) {
				builder += type->ToKiwi();
			}
			else {
				builder += '_';
			}
		}

		builder += ": ";
	}

	for (UInt i = 0; i < vars.Count(); i++) {
		if (i > 0) builder += ", ";

		if (Weak<Variable> var = vars[i]) {
			var->BuildString(builder);
		}
		else {
			builder += "invalid variable";
		}
	}

	if (expressions.IsEmpty()) return;

	builder += " = ";

	for (UInt i = 0; i < expressions.Count(); i++) {
		if (i > 0) builder += ", ";

		if (Weak<Expression> expression = expressions[i]) {
			expression->BuildString(builder);
		}
		else {
			builder += "invalid expression";
		}
	}
}

void OffsetAssignInstruction::Interpret(Interpreter::InterpreterData& data) {
	Interpreter::DataPtr ptr = var->EvaluateRef(data);

	if (type) {
		ptr += offset * Type::SizeOf(*type, data.program);
	}
	else {
		ptr += offset;
	}

	Interpreter::Data::Set(ptr, expression->Evaluate(data));
}

void OffsetAssignInstruction::BuildString(Boxx::StringBuilder& builder) {
	var->BuildString(builder);
	builder += '[';

	if (type) {
		builder += (*type).ToKiwi();
		builder += ' ';
	}

	builder += String::ToString(offset);
	builder += "] = ";

	expression->BuildString(builder);
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

IfInstruction::IfInstruction(Ptr<Expression> condition, const Boxx::Optional<Boxx::String>& trueLabel, const Boxx::Optional<Boxx::String>& falseLabel) {
	this->condition  = condition;
	this->trueLabel  = trueLabel;
	this->falseLabel = falseLabel;
}

void IfInstruction::Interpret(Interpreter::InterpreterData& data) {
	Interpreter::Data value = condition->Evaluate(data);

	if (value.GetNumber(Type::SizeOf(condition->GetType(data), data.program)) != 0) {
		if (trueLabel) {
			data.gotoLabel = *trueLabel;
		}
	}
	else {
		if (falseLabel) {
			data.gotoLabel = *falseLabel;
		}
	}
}

void IfInstruction::BuildString(Boxx::StringBuilder& builder) {
	builder += "if ";
	condition->BuildString(builder);
	builder += ": ";

	if (trueLabel) {
		builder += Name::ToKiwi(*trueLabel);
	}
	else {
		builder += '_';
	}

	if (falseLabel) {
		builder += ", ";
		builder += Name::ToKiwi(*falseLabel);
	}
}

void DebugPrintInstruction::Interpret(Interpreter::InterpreterData& data) {
	Interpreter::Data value = this->value->Evaluate(data);
	Interpreter::DataPtr ptr = value.Ptr();

	Type type = this->value->GetType(data);

	UInt size = value.Size();

	if (type.pointers > 0) {
		Interpreter::DataPtr ptr2 = value.Get<Interpreter::DataPtr>();

		if (data.heap->IsAllocated(ptr2)) {
			Console::Write('*');
			size = data.heap->GetSize(ptr2);
			ptr = ptr2;
		}
	}

	for (UInt i = 0; i < size; i++) {
		Console::Write((UInt)ptr[i]);
		Console::Write(' ');
	}

	Console::Print();
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


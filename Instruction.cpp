#include "Instruction.h"

#include "KiwiProgram.h"

#include "Boxx/Boxx/Console.h"

using namespace Boxx;

using namespace Kiwi;

Ptr<MultiAssignInstruction> AssignInstruction::ToMultiAssign() {
	Ptr<MultiAssignInstruction> assign;

	List<Ptr<Variable>> vars;
	vars.Add(var->Copy());

	List<Weak<Expression>> expressions;

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

		if (weakExpressions.Count() > 0) {
			if (i < weakExpressions.Count()) {
				expression = weakExpressions[i];
			}
			else {
				expression = weakExpressions.Last();
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
		
		if (i < weakExpressions.Count()) {
			if (i == weakExpressions.Count() - 1 && expression.Is<CallExpression>()) {
				extraValues = expression.As<CallExpression>()->EvaluateAll(data);

				if (extraValues.Length() > 0) {
					value = extraValues[0];
				}
			}
			else {
				value = expression->Evaluate(data);
			}
		}
		else if (extraValues.Length() > weakExpressions.Count() - i) {
			value = extraValues[weakExpressions.Count() - i + 1];
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
			Interpreter::Data val = Interpreter::Data(value, Type::SizeOf(data.frame->GetVarType(var->name), data.program));
			data.frame->SetVarValue(var->name, val);
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

	if (weakExpressions.IsEmpty()) return;

	builder += " = ";

	for (UInt i = 0; i < weakExpressions.Count(); i++) {
		if (i > 0) builder += ", ";

		if (Weak<Expression> expression = weakExpressions[i]) {
			expression->BuildString(builder);
		}
		else {
			builder += "invalid expression";
		}
	}
}

void OffsetAssignInstruction::Interpret(Interpreter::InterpreterData& data) {
	Interpreter::DataPtr ptr = var->EvaluateRef(data);

	Interpreter::Data offsetData = offset->Evaluate(data);
	UInt offsetNum = (UInt)offsetData.GetNumber(Type::SizeOf(offset->GetType(data), data.program));

	if (type) {
		ptr += offsetNum * Type::SizeOf(*type, data.program);
	}
	else {
		ptr += offsetNum;
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

	offset->BuildString(builder);
	builder += "] = ";

	expression->BuildString(builder);
}

void CopyInstruction::Interpret(Interpreter::InterpreterData& data) {
	Interpreter::Data dstData  = dst->Evaluate(data);
	Interpreter::Data srcData  = src->Evaluate(data);
	Interpreter::Data sizeData = size->Evaluate(data);

	Interpreter::DataPtr dstPtr = dstData.Get<Interpreter::DataPtr>();
	Interpreter::DataPtr srcPtr = srcData.Get<Interpreter::DataPtr>();

	std::memcpy(dstPtr, srcPtr, sizeData.GetNumber(sizeData.Size()));
}

void CopyInstruction::BuildString(Boxx::StringBuilder& builder) {
	builder += "copy ";
	dst->BuildString(builder);
	builder += ", ";
	src->BuildString(builder);
	builder += ", ";
	size->BuildString(builder);
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

FreeInstruction::FreeInstruction(Ptr<Value> value) {
	this->value = value;
}

void FreeInstruction::Interpret(Interpreter::InterpreterData& data) {
	Interpreter::Data value = this->value->Evaluate(data);
	data.heap->Free(value.Get<Interpreter::DataPtr>());
}

void FreeInstruction::BuildString(Boxx::StringBuilder& builder) {
	builder += "free ";
	value->BuildString(builder);
}

void DebugPrintInstruction::Interpret(Interpreter::InterpreterData& data) {
	Interpreter::Data value = this->value->Evaluate(data);

	if (type == "str") {
		Interpreter::DataPtr ptr = value.Get<Interpreter::DataPtr>();

		if (data.heap->IsAllocated(ptr)) {
			UInt size = data.heap->GetSize(ptr);
			
			char* str = new char[size + 1];
			std::memcpy(str, ptr, size);
			str[size] = '\0';

			Console::Print(str);

			delete[] str;
		}
		else {
			Console::Print(ptr);
		}

		return;
	}
	else if (type == "chr") {
		Console::Print(value.Get<char>());
		return;
	}

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

	if (type) {
		builder += *type;
		builder += " ";
	}

	if (value) {
		value->BuildString(builder);
	}
	else {
		builder += "invalid value";
	}
}


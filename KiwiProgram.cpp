#include "KiwiProgram.h"

using namespace Boxx;

using namespace Kiwi;

void KiwiProgram::AddCodeBlock(Ptr<InstructionBlock> block) {
	blocks.Add(block);
}

void KiwiProgram::AddStruct(Ptr<Struct> struct_) {
	structs.Add(struct_->name, struct_);
}

void KiwiProgram::AddFunction(Ptr<Function> function) {
	functions.Add(function->name, function);
}

void KiwiProgram::Interpret(Interpreter::InterpreterData& data) {
	for (Weak<InstructionBlock> block : blocks) {
		data.PushFrame();
		block->Interpret(data);
		data.PopFrame();
	}
}

void KiwiProgram::BuildString(StringBuilder& builder) {
	for (Weak<InstructionBlock> block : blocks) {
		builder += "code:\n";
		block->BuildString(builder);
	}

	for (const Pair<String, Ptr<Struct>>& struct_ : structs) {
		struct_.value->BuildString(builder);
	}

	for (const Pair<String, Ptr<Function>>& func : functions) {
		func.value->BuildString(builder);
	}
}


void InstructionBlock::AddInstruction(Ptr<Instruction> instruction) {
	instructions.Add(instruction);
}

void InstructionBlock::Interpret(Interpreter::InterpreterData& data) {
	for (Weak<Instruction> instruction : instructions) {
		instruction->Interpret(data);
	}
}

void InstructionBlock::BuildString(StringBuilder& builder) {
	for (Weak<Instruction> instruction : instructions) {
		builder += '\t';
		instruction->BuildString(builder);
		builder += '\n';
	}

	builder += '\n';
}


void Function::AddReturnValue(const Type& type, const String& name) {
	returnValues.Add(Tuple<>::Create(type, name));
}

void Function::AddArgument(const Type& type, const String& name) {
	arguments.Add(Tuple<>::Create(type, name));
}

void Function::AddInstruction(Ptr<Instruction> instruction) {
	block->AddInstruction(instruction);
}

void Function::Interpret(Interpreter::InterpreterData& data) {
	
}

void Function::BuildString(StringBuilder& builder) {
	builder += "function ";

	if (returnValues.Count() > 0) {
		bool singleType = true;

		for (Int i = 1; i < returnValues.Count(); i++) {
			if (returnValues[0].value1 != returnValues[i].value1) {
				singleType = false;
				break;
			}
		}

		if (singleType) {
			builder += returnValues[0].value1.ToKiwi();
		}
		else for (Int i = 0; i < returnValues.Count(); i++) {
			if (i > 0) builder += ", ";
			builder += returnValues[i].value1.ToKiwi();
		}

		builder += ": ";

		for (Int i = 0; i < returnValues.Count(); i++) {
			if (i > 0) builder += ", ";
			builder += Name::ToKiwi(returnValues[i].value2);
		}

		builder += ' ';
	}

	builder += Name::ToKiwi(name);
	builder += '(';

	for (Int i = 0; i < arguments.Count(); i++) {
		if (i > 0) builder += ", ";
		builder += arguments[i].value1.ToKiwi();
		builder += ": ";
		builder += Name::ToKiwi(arguments[i].value2);
	}

	builder += "):\n";

	block->BuildString(builder);
}

void Struct::AddVariable(const Type& type, const String& var) {
	vars.Add(Tuple<>::Create(type, var));
}

void Struct::BuildString(StringBuilder& builder) {
	builder += "struct ";
	builder += Name::ToKiwi(name);
	builder += ":\n";

	for (const Tuple<Type, String>& var : vars) {
		builder += '\t';
		builder += var.value1.ToKiwi();
		builder += ": ";
		builder += Name::ToKiwi(var.value2);
		builder += '\n';
	}

	builder += '\n';
}

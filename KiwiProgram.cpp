#include "KiwiProgram.h"

using namespace Boxx;

using namespace Kiwi;

void KiwiProgram::AddCodeBlock(Ptr<InstructionBlock> block) {
	blocks.Add(block);
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


void Function::AddReturnValue(const String& type, const String& name) {
	returnValues.Add(Tuple<>::Create(type, name));
}

void Function::AddArgument(const String& type, const String& name) {
	arguments.Add(Tuple<>::Create(type, name));
}

void Function::AddInstruction(Ptr<Instruction> instruction) {
	block->AddInstruction(instruction);
}

void Function::Interpret(Interpreter::InterpreterData& data) {
	
}

void Function::BuildString(StringBuilder& builder) {
	builder += "function ";

	if (returnValues.Size() > 0) {
		bool singleType = true;

		for (Int i = 1; i < returnValues.Size(); i++) {
			if (returnValues[0].value1 != returnValues[i].value1) {
				singleType = false;
				break;
			}
		}

		if (singleType) {
			builder += returnValues[0].value1;
		}
		else for (Int i = 0; i < returnValues.Size(); i++) {
			if (i > 0) builder += ", ";
			builder += returnValues[i].value1;
		}

		builder += ": ";

		for (Int i = 0; i < returnValues.Size(); i++) {
			if (i > 0) builder += ", ";
			builder += returnValues[i].value2;
		}

		builder += ' ';
	}

	builder += name;
	builder += '(';

	for (Int i = 0; i < arguments.Size(); i++) {
		if (i > 0) builder += ", ";
		builder += arguments[i].value1;
		builder += ": ";
		builder += arguments[i].value2;
	}

	builder += "):\n";

	block->BuildString(builder);
}

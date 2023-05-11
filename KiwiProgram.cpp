#include "KiwiProgram.h"

using namespace Boxx;

using namespace Kiwi;

void KiwiProgram::AddCodeBlock(Ptr<CodeBlock> block) {
	blocks.Add(block);
}

void KiwiProgram::AddStruct(Ptr<Struct> struct_) {
	structs.Add(struct_->name, struct_);
}

void KiwiProgram::AddFunction(Ptr<Function> function) {
	functions.Add(function->name, function);
}

void KiwiProgram::Interpret(Interpreter::InterpreterData& data) {
	for (Weak<CodeBlock> block : blocks) {
		data.PushFrame();
		block->Interpret(data);
		data.PopFrame();
	}
}

void KiwiProgram::BuildString(StringBuilder& builder) {
	for (Weak<CodeBlock> block : blocks) {
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

CodeBlock::CodeBlock() {
	mainBlock = new InstructionBlock("");
}

void CodeBlock::AddInstructionBlock(Ptr<InstructionBlock> block) {
	blocks.Add(block);
}

void CodeBlock::Interpret(Interpreter::InterpreterData& data) {
	data.gotoLabel = nullptr;
	data.ret = false;
	mainBlock->Interpret(data);

	if (data.ret) {
		data.ret = false;
		return;
	}

	if (!data.gotoLabel && blocks.Count() > 0) {
		data.gotoLabel = blocks[0]->label;
	}

	while (data.gotoLabel) {
		bool found = false;

		for (UInt i = 0; i < blocks.Count(); i++) {
			if (blocks[i]->label == data.gotoLabel) {
				found = true;
				data.gotoLabel = nullptr;
				blocks[i]->Interpret(data);

				if (data.ret) {
					data.ret = false;
					return;
				}

				if (!data.gotoLabel && i + 1 < blocks.Count()) {
					data.gotoLabel = blocks[i + 1]->label;
				}
				else {
					break;
				}
			}
		}

		if (!found && data.gotoLabel) {
			throw Interpreter::KiwiInterpretError("label '" + Name::ToKiwi(*data.gotoLabel) + "' not found");
		}
	}
}

void CodeBlock::BuildString(StringBuilder& builder) {
	mainBlock->BuildStringNoLabel(builder);

	for (Weak<InstructionBlock> block : blocks) {
		block->BuildString(builder);
	}

	builder += '\n';
}

void InstructionBlock::AddInstruction(Ptr<Instruction> instruction) {
	instructions.Add(instruction);
}

void InstructionBlock::Interpret(Interpreter::InterpreterData& data) {
	for (Weak<Instruction> instruction : instructions) {
		if (data.gotoLabel) {
			return;
		}

		instruction->Interpret(data);

		if (data.ret) break;
	}
}

void InstructionBlock::BuildString(StringBuilder& builder) {
	builder += Name::ToKiwi(label);
	builder += ":\n";

	BuildStringNoLabel(builder);
}

void InstructionBlock::BuildStringNoLabel(StringBuilder& builder) {
	for (Weak<Instruction> instruction : instructions) {
		builder += '\t';
		instruction->BuildString(builder);
		builder += '\n';
	}
}


void Function::AddReturnValue(const Type& type, const String& name) {
	returnValues.Add(Tuple<>::Create(type, name));
}

void Function::AddArgument(const Type& type, const String& name) {
	arguments.Add(Tuple<>::Create(type, name));
}

void Function::AddInstruction(Ptr<Instruction> instruction) {
	block->mainBlock->AddInstruction(instruction);
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

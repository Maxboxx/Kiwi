#include "KiwiProgram.h"

using namespace Boxx;

using namespace Kiwi;

void KiwiProgram::AddCodeBlock(Ptr<CodeBlock> block) {
	blocks.Add(block);
}

void KiwiProgram::AddStruct(Ptr<Struct> struct_) {
	structs.Set(struct_->name, struct_);
}

void KiwiProgram::AddStatic(Ptr<StaticData> data) {
	staticData.Set(data->name, data);
}

void KiwiProgram::AddFunction(Ptr<Function> function) {
	functions.Add(function->name, function);
}

void KiwiProgram::Interpret(Interpreter::InterpreterData& data) {
	for (const Pair<String, Ptr<Function>>& f : functions) {
		UInt id = data.funcIdMap.Count();

		data.funcIdMap.Add(f.key, id);
		data.funcNameMap.Add(id, f.key);
	}

	for (const Pair<String, Ptr<StaticData>>& sd : staticData) {
		Interpreter::Data d = data.heap->Alloc(sd.value->Size(data.program));
		Interpreter::DataPtr ptr = d.Ptr();
		
		for (const Tuple<Type, String, Ptr<Value>>& value : sd.value->data) {
			UInt size = Type::SizeOf(value.value1, data.program);

			Interpreter::Data valData = value.value3->Evaluate(data);
			std::memcpy(ptr, valData.Ptr(), size);
			ptr += size;
		}

		data.staticData.Add(sd.key, d);
	}

	for (Weak<CodeBlock> block : blocks) {
		data.PushFrame();
		block->Interpret(data);
		data.PopFrame();
	}

	for (const Pair<String, Interpreter::Data>& sd : data.staticData) {
		data.heap->Free(sd.value);
	}
}

void KiwiProgram::BuildString(StringBuilder& builder) {
	for (const Pair<String, Ptr<StaticData>>& data : staticData) {
		data.value->BuildString(builder);
	}

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

		for (UInt i = 1; i < returnValues.Count(); i++) {
			if (returnValues[0].value1 != returnValues[i].value1) {
				singleType = false;
				break;
			}
		}

		if (singleType) {
			builder += returnValues[0].value1.ToKiwi();
		}
		else for (UInt i = 0; i < returnValues.Count(); i++) {
			if (i > 0) builder += ", ";
			builder += returnValues[i].value1.ToKiwi();
		}

		builder += ": ";

		for (UInt i = 0; i < returnValues.Count(); i++) {
			if (i > 0) builder += ", ";
			builder += Name::ToKiwi(returnValues[i].value2);
		}

		builder += ' ';
	}

	builder += Name::ToKiwi(name);
	builder += '(';

	for (UInt i = 0; i < arguments.Count(); i++) {
		if (i > 0) builder += ", ";
		builder += arguments[i].value1.ToKiwi();
		builder += ": ";
		builder += Name::ToKiwi(arguments[i].value2);
	}

	builder += "):\n";

	block->BuildString(builder);
}

void Struct::AddVariable(const Type& type, const String& var, bool replace) {
	for (Tuple<Type, String>& v : vars) {
		if (v.value2 == var) {
			if (replace) {
				v.value1 = type;
			}

			return;
		}
	}

	vars.Add(Tuple<>::Create(type, var));
}

UInt Struct::Size(Weak<KiwiProgram> program) {
	UInt size = 0;

	for (UInt i = 0; i < vars.Count(); i++) {
		size += Type::SizeOf(vars[i].value1, program);
	}

	return size;
}

UInt Struct::VarOffset(String var, Weak<KiwiProgram> program) {
	UInt size = 0;

	for (UInt i = 0; i < vars.Count(); i++) {
		if (vars[i].value2 == var) return size;

		size += Type::SizeOf(vars[i].value1, program);
	}

	return size;
}

Type Struct::VarType(String var) {
	for (UInt i = 0; i < vars.Count(); i++) {
		if (vars[i].value2 == var) return vars[i].value1;
	}

	return Type();
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

void StaticData::AddValue(const Type& type, const String& name, const Ptr<Value>& value, bool replace) {
	for (Tuple<Type, String, Ptr<Value>>& d : data) {
		if (d.value2 == name) {
			if (replace) {
				d.value1 = type;
				d.value3 = value;
			}

			return;
		}
	}

	data.Add(Tuple<>::Create(type, name, value));
}

UInt StaticData::Size(Weak<KiwiProgram> program) {
	UInt size = 0;

	for (UInt i = 0; i < data.Count(); i++) {
		size += Type::SizeOf(data[i].value1, program);
	}

	return size;
}

void StaticData::BuildString(StringBuilder& builder) {
	builder += "static ";
	builder += Name::ToKiwi(name);
	builder += ":\n";

	for (const Tuple<Type, String, Ptr<Value>>& value : data) {
		builder += '\t';
		builder += value.value1.ToKiwi();
		builder += ": ";
		builder += Name::ToKiwi(value.value2);
		builder += " = ";
		value.value3->BuildString(builder);
		builder += '\n';
	}

	builder += '\n';
}

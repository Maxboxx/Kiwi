#pragma once

#include "Instruction.h"
#include "Node.h"

#include "Boxx/Boxx/List.h"
#include "Boxx/Boxx/Map.h"
#include "Boxx/Boxx/Tuple.h"
#include "Boxx/Boxx/StringBuilder.h"

namespace Kiwi {
	class InstructionBlock;
	class Function;

	/// The root node for kiwi programs.
	class KiwiProgram : public Node {
	public:
		Boxx::List<Ptr<InstructionBlock>> blocks;
		Boxx::Map<Boxx::String, Ptr<Function>> functions;

		/// Adds a code block.
		void AddCodeBlock(Ptr<InstructionBlock> block);

		/// Adds a function.
		void AddFunction(Ptr<Function> function);

		virtual void Interpret(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};

	/// A block of instructions.
	class InstructionBlock : public Node {
	public:
		Boxx::List<Ptr<Instruction>> instructions;

		/// Adds an instruction to the block.
		void AddInstruction(Ptr<Instruction> instruction);

		virtual void Interpret(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};

	/// A kiwi function.
	class Function : public Node {
	public:
		Boxx::String name;

		Boxx::List<Boxx::Tuple<Boxx::String, Boxx::String>> returnValues;
		Boxx::List<Boxx::Tuple<Boxx::String, Boxx::String>> arguments;

		Ptr<InstructionBlock> block = new InstructionBlock();

		Function(const Boxx::String& name) {
			this->name = name;
		}

		/// Adds a return value to the function.
		void AddReturnValue(const Boxx::String& type, const Boxx::String& name);

		/// Adds an argument to the function.
		void AddArgument(const Boxx::String& type, const Boxx::String& name);

		/// Adds an instruction to the function body.
		void AddInstruction(Ptr<Instruction> instruction);

		virtual void Interpret(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};
}

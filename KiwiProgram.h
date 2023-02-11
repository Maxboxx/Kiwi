#pragma once

#include "Instruction.h"
#include "Node.h"

#include "Boxx/Boxx/List.h"
#include "Boxx/Boxx/Map.h"
#include "Boxx/Boxx/Tuple.h"
#include "Boxx/Boxx/StringBuilder.h"

///N Kiwi

namespace Kiwi {
	class CodeBlock;
	class Function;
	class Struct;

	/// The root node for kiwi programs.
	class KiwiProgram : public Node {
	public:
		/// The byte size for pointers.
		static const Boxx::UInt ptrSize = 8;

		/// All code blocks.
		Boxx::List<Ptr<CodeBlock>> blocks;

		/// All structs.
		Boxx::Map<Boxx::String, Ptr<Struct>> structs;

		/// All functions.
		Boxx::Map<Boxx::String, Ptr<Function>> functions;

		/// Adds a code block.
		void AddCodeBlock(Ptr<CodeBlock> block);

		/// Adds a struct.
		void AddStruct(Ptr<Struct> struct_);

		/// Adds a function.
		void AddFunction(Ptr<Function> function);

		virtual void Interpret(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};

	class InstructionBlock;

	/// A code block.
	class CodeBlock : public Node {
	public:
		/// The main instruction block.
		Ptr<InstructionBlock> mainBlock;

		/// The instruction blocks.
		Boxx::List<Ptr<InstructionBlock>> blocks;

		CodeBlock();

		/// Adds an instruction block.
		void AddInstructionBlock(Ptr<InstructionBlock> subBlock);

		virtual void Interpret(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};

	/// A block of instructions.
	class InstructionBlock : public Node {
	public:
		/// The label for the sub block.
		Boxx::String label;

		/// All instructions.
		Boxx::List<Ptr<Instruction>> instructions;

		InstructionBlock(const Boxx::String& label) {
			this->label = label;
		}

		/// Adds an instruction to the sub block.
		void AddInstruction(Ptr<Instruction> instruction);

		virtual void Interpret(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
		void BuildStringNoLabel(Boxx::StringBuilder& builder);
	};

	/// A kiwi function.
	class Function : public Node {
	public:
		/// The function name.
		Boxx::String name;

		/// The return values.
		Boxx::List<Boxx::Tuple<Type, Boxx::String>> returnValues;

		/// The arguments.
		Boxx::List<Boxx::Tuple<Type, Boxx::String>> arguments;

		/// The function body.
		Ptr<CodeBlock> block = new CodeBlock();

		Function(const Boxx::String& name) {
			this->name = name;
		}

		/// Adds a return value to the function.
		void AddReturnValue(const Type& type, const Boxx::String& name);

		/// Adds an argument to the function.
		void AddArgument(const Type& type, const Boxx::String& name);

		/// Adds an instruction to the function body.
		void AddInstruction(Ptr<Instruction> instruction);

		virtual void Interpret(Interpreter::InterpreterData& data) override;
		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};

	/// A kiwi struct.
	class Struct : public Node {
	public:
		/// The struct name.
		Boxx::String name;

		/// The struct variables.
		Boxx::List<Boxx::Tuple<Type, Boxx::String>> vars;

		Struct(const Boxx::String& name) {
			this->name = name;
		}

		/// Adds a variable to the struct.
		void AddVariable(const Type& type, const Boxx::String& var);

		virtual void BuildString(Boxx::StringBuilder& builder) override;
	};
}

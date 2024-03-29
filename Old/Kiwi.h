#pragma once

#include "Boxx/Boxx/String.h"
#include "Boxx/Boxx/List.h"
#include "Boxx/Boxx/File.h"
#include "Boxx/Boxx/Pair.h"
#include "Boxx/Boxx/Pointer.h"
#include "Boxx/Boxx/Logger.h"
#include "Boxx/Boxx/StaticArray.h"
#include "Boxx/Boxx/Either.h"
#include "Boxx/Boxx/Regex.h"

///N KiwiOld

namespace KiwiOld {
	///B ArgumentType
	/// The argument type for a <code>Argument</code>
	enum class ArgumentType : Boxx::UByte {
		///T Values
		///M
		None,
		Register,
		Number,
		Memory,
		Label,
		Name
		///M
	};

	///B RegisterType
	/// The type of register for a <code>Register</code>
	enum class RegisterType : Boxx::UByte {
		///T Values
		///M
		Register,
		Stack,
		Name
		///M
	};

	///B InstructionType
	/// The instruction type for a <code>Instruction</code>
	enum class InstructionType : Boxx::UByte {
		///T Values
		///M
		Nop,
		Mov,
		Jmp,
		Call,
		Ret,
		Adr,
		Exit,
		Push,
		Pop,

		Add,
		Sub,
		Neg,

		Mul,
		Div,
		Mod,

		Not,
		Or,
		And,
		Xor,
		Nor,
		Nand,
		Xnor,
		ShL,
		ShR,

		Eq,
		Ne,
		Le,
		Ge,
		Lt,
		Gt,

		Custom,
		Label,
		Function,
		Code,
		Static,
		Byte,
		Short,
		Int,
		Long,
		Zero
		///M
	};

	///B Register
	/// A register for <code>Argument</code>
	struct Register {
		///H Members

		///T Register type
		RegisterType type{};

		///T Register index
		/// The index of the register
		/// Only used if <code>type</code> is <code>RegisterType::Register</code>
		Boxx::UByte index = 0;

		///T Register name
		/// The name of the register
		/// Only used if <code>type</code> is <code>RegisterType::Name</code>
		Boxx::String name;

		///H Constructors

		///T Constructors
		///M
		Register();
		Register(const Boxx::UByte index);
		Register(const RegisterType type, const Boxx::UByte index = 0);
		Register(const Boxx::String& name);
		///M

		~Register();

		///H Methods

		///H Operators

		///T Equality operators
		///M
		bool operator==(const Register& reg) const;
		bool operator!=(const Register& reg) const;
		///M

		bool operator<(const Register& reg) const;
	};

	inline Register::Register() {

	}

	inline Register::Register(const Boxx::UByte index) {
		this->type = RegisterType::Register;
		this->index = index;
	}

	inline Register::Register(const RegisterType type, const Boxx::UByte index) {
		this->type = type;
		this->index = index;
	}

	inline Register::Register(const Boxx::String& name) {
		this->type = RegisterType::Name;
		this->name = name;
	}

	inline Register::~Register() {

	}

	inline bool Register::operator==(const Register& reg) const {
		if (type != reg.type) return false;

		if (type == RegisterType::Register) {
			return index == reg.index;
		}
		else if (type == RegisterType::Name) {
			return name == reg.name;
		}
		else if (type == RegisterType::Stack) {
			return true;
		}

		return false;
	}

	inline bool Register::operator!=(const Register& reg) const {
		return !operator==(reg);
	}

	inline bool Register::operator<(const Register& reg) const {
		if (type != reg.type) {
			return type < reg.type;
		}

		switch (type) {
			case RegisterType::Stack: {
				return false;
			}

			case RegisterType::Register: {
				return index < reg.index;
			}

			case RegisterType::Name: {
				return name < reg.name;
			}
		}

		return false;
	}

	///B MemoryLocation
	/// A value on the stack for <code>Argument</code>
	struct MemoryLocation {
		///T Offset
		/// The stack offset from the memory pointer
		Boxx::Int offset{};

		///T Memory Pointer
		/// The register or memory label to use as the memory pointer
		Boxx::Either<Register, Boxx::String> memptr = Register(RegisterType::Stack);

		///T Constructors
		///M
		MemoryLocation();
		MemoryLocation(const Boxx::Int offset);
		///M

		~MemoryLocation();

		///T Equality operators
		///M
		bool operator==(const MemoryLocation& stack) const;
		bool operator!=(const MemoryLocation& stack) const;
		///M

		bool operator<(const MemoryLocation& stack) const;
	};

	inline MemoryLocation::MemoryLocation() {

	}

	inline MemoryLocation::MemoryLocation(const Boxx::Int offset) {
		this->offset = offset;
	}

	inline MemoryLocation::~MemoryLocation() {

	}

	inline bool MemoryLocation::operator==(const MemoryLocation& stack) const {
		return memptr == stack.memptr && offset == stack.offset;
	}

	inline bool MemoryLocation::operator!=(const MemoryLocation& stack) const {
		return !operator==(stack);
	}

	inline bool MemoryLocation::operator<(const MemoryLocation& stack) const {
		if (memptr != stack.memptr) return memptr < stack.memptr;
		return offset < stack.offset;
	}

	///B Argument
	/// Used as an argument to <code>Instruction</code>
	struct Argument {
		///H Members

		///T Argument type
		ArgumentType type = ArgumentType::None;

		///T Register
		/// Only used if <code>type</code> is <code>ArgumentType::Register</code>
		Register reg;

		///T Memory Location
		/// Only used if <code>type</code> is <code>ArgumentType::Memory</code>
		MemoryLocation mem;

		///T Number
		/// Used for immediate integers
		/// Only used if <code>type</code> is <code>ArgumentType::Number</code>
		Boxx::Long number{};

		///T Label
		/// Only used if <code>type</code> is <code>ArgumentType::Label</code> or <code>ArgumentType::Function</code>
		Boxx::String label;

		///H Constructors

		///T Constructors
		///M
		Argument();
		Argument(const Boxx::Long number);
		Argument(const ArgumentType type, const Boxx::Long number);
		Argument(const ArgumentType type, const Boxx::String& label);
		Argument(const Register r);
		Argument(const MemoryLocation s);
		///M

		~Argument();

		///H Operators

		///T Equality
		///M
		bool operator==(const Argument& arg) const;
		bool operator!=(const Argument& arg) const;
		///M

		bool operator<(const Argument& arg) const;
	};

	inline Argument::Argument() {

	}

	inline Argument::Argument(const Boxx::Long number) {
		this->type = ArgumentType::Number;
		this->number = number;
	}

	inline Argument::Argument(const ArgumentType type, const Boxx::Long number) {
		this->type = type;
		this->label = (type == ArgumentType::Label ? Boxx::String("L") : Boxx::String("F")) + (Boxx::UInt)number;
	}

	inline Argument::Argument(const ArgumentType type, const Boxx::String& label) {
		this->type = type;
		this->label = label;
	}

	inline Argument::Argument(const Register r) {
		this->type = ArgumentType::Register;
		this->reg = r;
	}

	inline Argument::Argument(const MemoryLocation s) {
		this->type = ArgumentType::Memory;
		this->mem = s;
	}

	inline Argument::~Argument() {

	}

	inline bool Argument::operator==(const Argument& arg) const {
		if (type != arg.type) return false;

		if (type == ArgumentType::Number || type == ArgumentType::Label || type == ArgumentType::Name) {
			return number == arg.number;
		}
		else if (type == ArgumentType::Register) {
			return reg == arg.reg;
		}
		else if (type == ArgumentType::Memory) {
			return mem == arg.mem;
		}

		return false;
	}

	inline bool Argument::operator!=(const Argument& arg) const {
		return !operator==(arg);
	}

	inline bool Argument::operator<(const Argument& arg) const {
		if (type != arg.type) return type < arg.type;

		if (type == ArgumentType::Number || type == ArgumentType::Label || type == ArgumentType::Name) {
			return number < arg.number;
		}
		else if (type == ArgumentType::Register) {
			return reg < arg.reg;
		}
		else if (type == ArgumentType::Memory) {
			return mem < arg.mem;
		}

		return false;
	}

	///B Instruction
	/// An instruction for the kiwi language
	struct Instruction {
		///H Members

		///T Type
		/// The type of the instruction
		InstructionType type{};

		///T Arguments
		/// A list of arguments for the instruction
		Boxx::List<Argument> arguments{3};

		///T Argument sizes
		/// The byte size of each argument of the instruction
		Boxx::StaticArray<Boxx::UByte, 3> sizes;

		///T Argument signs
		/// Whether the arguments of the instruction are signed or not
		Boxx::StaticArray<bool, 3> signs;

		///T Instruction name
		/// Used if <code>type</code> is <code>InstructionName::Custom</code>
		Boxx::String instructionName;

		///H Constructors

		///T Constructors
		///M
		Instruction();
		Instruction(const InstructionType name, const Boxx::UByte size, const bool sign = true);
		Instruction(const InstructionType name);
		Instruction(const Boxx::String& name, const Boxx::UByte size, const bool sign = true);
		Instruction(const Boxx::String& name);
		///M

		~Instruction();

		///H Methods

		///T Copy
		/// Copies the instruction
		Instruction Copy() const;

		///T Get largest size
		/// Get the size of largest argument
		Boxx::UByte GetLargestSize() const;

		bool operator==(const Instruction& inst) const;
		bool operator!=(const Instruction& inst) const;

		///H Static functions

		///T Is comparison
		/// Checks if the instruction type is a comparison instruction
		static bool IsComp(const InstructionType type);

		///T Label
		/// Creates a label
		static Instruction Label(const Boxx::UInt index);

		///T Function
		/// Creates a function
		static Instruction Function(const Boxx::UInt index);

		///T No size
		/// Checks if an instruction type has no size
		static bool NoSize(const InstructionType type);
	};

	inline Instruction::Instruction() {

	}

	inline Instruction::Instruction(const InstructionType name, const Boxx::UByte size, const bool sign) {
		this->type = name;
		this->sizes[0] = size;
		this->sizes[1] = size;
		this->sizes[2] = size;
		this->signs[0] = sign;
		this->signs[1] = sign;
		this->signs[2] = sign;
	}

	inline Instruction::Instruction(const InstructionType name) {
		this->type = name;
		this->sizes[0] = 0;
		this->sizes[1] = 0;
		this->sizes[2] = 0;
		this->signs[0] = true;
		this->signs[1] = true;
		this->signs[2] = true;
	}

	inline Instruction::Instruction(const Boxx::String& name, const Boxx::UByte size, const bool sign) {
		this->type = InstructionType::Custom;
		this->instructionName = name;
		this->sizes[0] = size;
		this->sizes[1] = size;
		this->sizes[2] = size;
		this->signs[0] = sign;
		this->signs[1] = sign;
		this->signs[2] = sign;
	}

	inline Instruction::Instruction(const Boxx::String& name) {
		this->type = InstructionType::Custom;
		this->instructionName = name;
	}

	inline Instruction::~Instruction() {

	}

	inline Instruction Instruction::Copy() const {
		Instruction inst = *this;
		inst.sizes = Boxx::StaticArray<Boxx::UByte, 3>();
		inst.signs = Boxx::StaticArray<bool, 3>();
		inst.arguments = Boxx::List<Argument>();

		for (Boxx::UInt i = 0; i < inst.sizes.Length(); i++) {
			inst.sizes[i] = this->sizes[i];
			inst.signs[i] = this->signs[i];
		}

		for (const Argument arg : this->arguments)
			inst.arguments.Add(arg);

		return inst;
	}

	inline Boxx::UByte Instruction::GetLargestSize() const {
		Boxx::UByte best = 0;

		for (const Boxx::UByte size : sizes) {
			if (size > best) {
				best = size;
			}
		}

		return best;
	}

	inline bool Instruction::operator==(const Instruction& inst) const {
		if (type != inst.type) return false;
		if (arguments.Count() != inst.arguments.Count()) return false;

		for (Boxx::UInt i = 0; i < arguments.Count(); i++) {
			if (arguments[i] != inst.arguments[i]) return false;
		}

		for (Boxx::UInt i = 0; i < sizes.Length(); i++) {
			if (sizes[i] != inst.sizes[i]) return false;
			if (signs[i] != inst.signs[i]) return false;
		}

		return true;
	}

	inline bool Instruction::operator!=(const Instruction& inst) const {
		return !operator==(inst);
	}

	inline bool Instruction::IsComp(const InstructionType name) {
		return
			name == InstructionType::Eq ||
			name == InstructionType::Ne ||
			name == InstructionType::Le ||
			name == InstructionType::Ge ||
			name == InstructionType::Lt ||
			name == InstructionType::Gt;
	}

	inline Instruction Instruction::Label(const Boxx::UInt index) {
		Instruction inst;
		inst.type = InstructionType::Label;
		inst.instructionName = Boxx::String("L") + (Boxx::Int)index;
		return inst;
	}

	inline Instruction Instruction::Function(const Boxx::UInt index) {
		Instruction inst;
		inst.type = InstructionType::Function;
		inst.instructionName = Boxx::String("F") + (Boxx::Int)index;
		return inst;
	}

	inline bool Instruction::NoSize(const InstructionType name) {
		return
			name == InstructionType::Nop ||
			name == InstructionType::Custom ||
			name == InstructionType::Label ||
			name == InstructionType::Function ||
			name == InstructionType::Static ||
			name == InstructionType::Code ||
			name == InstructionType::Jmp ||
			name == InstructionType::Call ||
			name == InstructionType::Ret ||
			name == InstructionType::Byte ||
			name == InstructionType::Short ||
			name == InstructionType::Int ||
			name == InstructionType::Long ||
			name == InstructionType::Zero;
	}

	///B Converter
	/// Base class for converting kiwi instructions to other languages
	class Converter {
	protected:
		Boxx::Logger logger;

		friend class KiwiLang;

	public:
		Converter() {}

		Converter(const Boxx::Logger& logger) {
			this->logger = logger;
		}

		~Converter() {

		}

		///T Convert to file
		/// Converts instructions and writes them to a file
		virtual void ConvertToFile(Boxx::FileWriter& file, const Boxx::List<Instruction>& instructions);

		///T Convert instruction
		/// Converts an instruction to a string
		virtual Boxx::String ConvertInstruction(const Instruction& instruction) {
			Boxx::String inst = "";

			switch (instruction.type) {
				case InstructionType::Nop:  inst = "Nop "; break;
				case InstructionType::Mov:  inst = "Mov "; break;
				case InstructionType::Jmp:  inst = "Jmp "; break;
				case InstructionType::Call: inst = "Call"; break;
				case InstructionType::Ret:  inst = "Ret "; break;
				case InstructionType::Adr:  inst = "Adr "; break;
				case InstructionType::Exit: inst = "Exit"; break;
				case InstructionType::Push: inst = "Push"; break;
				case InstructionType::Pop:  inst = "Pop "; break;

				case InstructionType::Add:  inst = "Add "; break;
				case InstructionType::Sub:  inst = "Sub "; break;
				case InstructionType::Neg:  inst = "Neg "; break;
				case InstructionType::Mul:  inst = "Mul "; break;
				case InstructionType::Div:  inst = "Div "; break;
				case InstructionType::Mod:  inst = "Mod "; break;

				case InstructionType::Not:  inst = "Not "; break;
				case InstructionType::Or:   inst = "Or  "; break;
				case InstructionType::And:  inst = "And "; break;
				case InstructionType::Xor:  inst = "Xor "; break;
				case InstructionType::Nor:  inst = "Nor "; break;
				case InstructionType::Nand: inst = "Nand"; break;
				case InstructionType::Xnor: inst = "Xnor"; break;
				case InstructionType::ShL:  inst = "ShL "; break;
				case InstructionType::ShR:  inst = "ShR "; break;

				case InstructionType::Eq:   inst = "Eq  "; break;
				case InstructionType::Ne:   inst = "Ne  "; break;
				case InstructionType::Le:   inst = "Le  "; break;
				case InstructionType::Ge:   inst = "Ge  "; break;
				case InstructionType::Lt:   inst = "Lt  "; break;
				case InstructionType::Gt:   inst = "Gt  "; break;

				case InstructionType::Label:    inst = instruction.instructionName; break;
				case InstructionType::Function: inst = "\nfunction " + ConvertName(instruction.instructionName, 8); break;
				case InstructionType::Static:   inst = "\nstatic " + ConvertName(instruction.instructionName, 8); break;
				case InstructionType::Code:     inst = "\ncode"; break;

				case InstructionType::Byte:  inst = "byte";  break;
				case InstructionType::Short: inst = "short"; break;
				case InstructionType::Int:   inst = "int";   break;
				case InstructionType::Long:  inst = "long";  break;

				default: inst = "Undefined"; logger.Error("undefined instruction type"); break;
			}

			if (
				instruction.type != InstructionType::Label && 
				instruction.type != InstructionType::Function &&
				instruction.type != InstructionType::Static &&
				instruction.type != InstructionType::Code
				) {
				inst = "\t" + inst;
			}
			else {
				inst += ":";
			}

			if (!Instruction::NoSize(instruction.type)) {
				Boxx::UByte size = instruction.sizes[0];
				bool sign = instruction.signs[0];
				bool eq = true;

				for (Boxx::UInt i = 1; i < instruction.arguments.Count(); i++) {
					if (instruction.arguments[i].type == ArgumentType::Label) break;

					if (instruction.sizes[i] != size) {
						eq = false;
						break;
					}

					if (instruction.signs[i] != sign) {
						eq = false;
						break;
					}
				}

				if (eq) {
					inst += " " + Boxx::String::ToString((Boxx::UInt)size) + (sign ? "" : "U");
				}
				else {
					for (Boxx::UInt i = 0; i < instruction.arguments.Count(); i++) {
						if (instruction.arguments[i].type == ArgumentType::Label) break;
						if (i > 0) inst += ",";
						inst += " " + Boxx::String::ToString((Boxx::UInt)instruction.sizes[i]) + (instruction.signs[i] ? "" : "U");
					}
				}

				if (!instruction.arguments.IsEmpty()) {
					inst += ":";
				}
			}

			for (Boxx::UInt i = 0; i < instruction.arguments.Count(); i++) {
				if (i > 0) {
					inst += ",";
				}

				inst += " " + ConvertArgument(instruction.arguments[i], instruction.sizes[i]);
			}

			return inst + "\n";
		}

		///T Convert argument
		/// Converts an argument to a string
		virtual Boxx::String ConvertArgument(const Argument& argument, const Boxx::UByte size) {
			switch (argument.type) {
				case ArgumentType::Register: return ConvertRegister(argument.reg, size);
				case ArgumentType::Memory:   return ConvertMemoryLocation(argument.mem, size);
				case ArgumentType::Number:   return ConvertNumber(argument.number, size);
				case ArgumentType::Name:     return ConvertName(argument.label, size);
				case ArgumentType::Label:    return ConvertLabel(argument.label, size);
				default: logger.Error("undefined instruction argument"); return "undefined argument";
			}
		}

		///T Convert register
		/// Converts a register to a string
		virtual Boxx::String ConvertRegister(const Register& reg, const Boxx::UByte size) {
			switch (reg.type) {
				case RegisterType::Stack: return "SP";
				case RegisterType::Register: return "R" + Boxx::String::ToString((int)reg.index);
				default: logger.Error("undefined register"); return "undefined register";
			}
		}

		///T Convert stack value
		/// Converts a stack value to a string
		virtual Boxx::String ConvertMemoryLocation(const MemoryLocation& stackValue, const Boxx::UByte size) {
			if (stackValue.memptr.IsLeft()) {
				return ConvertRegister(stackValue.memptr.GetLeft(), size) + "[" + Boxx::String::ToString((int)stackValue.offset) + "]";
			}
			else {
				return ConvertName(stackValue.memptr.GetRight(), size) + "[" + Boxx::String::ToString((int)stackValue.offset) + "]";;
			}
		}

		///T Convert number
		/// Converts a number constant to a string
		virtual Boxx::String ConvertNumber(const Boxx::Long number, const Boxx::UByte size) {
			return Boxx::String::ToString(number);
		}

		///T Convert function
		/// Converts a function label to a string
		virtual Boxx::String ConvertName(const Boxx::String& label, const Boxx::UByte size) {
			static Boxx::Regex simplePattern = Boxx::Regex("^[_%a]%w*$");

			if (simplePattern.Match(label)) {
				// Check for reserved names 
				return label;
			}
			else {
				return "<" + label.Replace(">", "\\>") + ">";
			}
		}

		///T Convert label
		/// Converts a label to a string
		virtual Boxx::String ConvertLabel(const Boxx::String& label, const Boxx::UByte size) {
			return label;
		}
	};

	///B KiwiLang
	/// Main kiwi class
	class KiwiLang {
	public:
		static const Boxx::UByte maxSize = 8; 

		struct ErrorInfo {
			Boxx::String file = "";
			Boxx::UInt line = 0;
			Boxx::UInt startLine = 0;

			Boxx::String CreateMessage(const Boxx::String& message) const {
				return file + ":" + Boxx::String::ToString(line + startLine) + ": " + message;
			}

			Boxx::String InvalidArgs() const {
				return CreateMessage("invalid arguments");
			}

			Boxx::String FewArgs() const {
				return CreateMessage("not enough arguments");
			}

			Boxx::String ManyArgs() const {
				return CreateMessage("too many arguments");
			}

			~ErrorInfo() {}
		};

		///T Write to file
		/// Writes a list of kiwi instructions to a file
		/// The instructions will not be converted if an instruction contains errors
		static void WriteToFile(const Boxx::String& filename, const Boxx::Pointer<Converter>& converter, const Boxx::List<Instruction>& instructions) {
			KiwiLang::ErrorInfo errInfo;
			errInfo.file = filename;
			errInfo.startLine = 1;

			if (!KiwiLang::Validate(instructions, converter->logger, errInfo)) return;

			Boxx::FileWriter file = Boxx::FileWriter(filename);
			converter->ConvertToFile(file, instructions);
		}

		///T Write to file
		/// Writes a list of kiwi instructions to a kiwi file
		static void WriteToFile(const Boxx::String& filename, const Boxx::Logger& logger, const Boxx::List<Instruction>& instructions) {
			KiwiLang::ErrorInfo errInfo;
			errInfo.file = filename;
			errInfo.startLine = 1;

			Boxx::Pointer<Converter> converter = new Converter(logger);

			KiwiLang::Validate(instructions, converter->logger, errInfo);

			Boxx::FileWriter file = Boxx::FileWriter(filename);
			converter->ConvertToFile(file, instructions);
		}

		///T Parse file
		/// Parses a kiwi file into a list of instructions
		static Boxx::List<Instruction> ParseFile(const Boxx::String& filename);

		///T Is valid size
		/// Checks if the size is a valid size
		static bool IsValidSize(const Boxx::UInt size) {
			Boxx::UByte s = 1;

			while (s <= size && s <= maxSize) {
				if (s == size) return true;
				s *= 2;
			}

			return false;
		}

		///T Validate instruction
		/// Validates a kiwi instruction
		static bool Validate(const Instruction& instruction, Boxx::Logger& logger, const ErrorInfo& errInfo = ErrorInfo()) {
			Instruction inst = instruction.Copy();

			switch (inst.type) {
				case InstructionType::Mov:
				case InstructionType::Adr: {
					return ValidateMov(inst, logger, errInfo);
				}

				case InstructionType::Add:
				case InstructionType::Sub:
				case InstructionType::Mul:
				case InstructionType::Div:
				case InstructionType::Mod:
				case InstructionType::Or:
				case InstructionType::And:
				case InstructionType::Xor:
				case InstructionType::Nor:
				case InstructionType::Nand:
				case InstructionType::Xnor:
				case InstructionType::ShL:
				case InstructionType::ShR: {
					return ValidateBinOp(inst, logger, errInfo);
				}

				case InstructionType::Neg:
				case InstructionType::Not: {
					return ValidateUnaryOp(inst, logger, errInfo);
				}

				case InstructionType::Label:
				case InstructionType::Function:
				case InstructionType::Static: {
					return ValidateLabel(inst, logger, errInfo);
				}

				case InstructionType::Code: {
					// TODO: fix
					return true;
				}

				case InstructionType::Byte:
				case InstructionType::Short:
				case InstructionType::Int:
				case InstructionType::Long:
				case InstructionType::Zero: {
					// TODO: Fix
					return true;
				}

				case InstructionType::Call: {
					return ValidateCall(inst, logger, errInfo);
				}

				case InstructionType::Jmp: {
					return ValidateJmp(inst, logger, errInfo);
				}

				case InstructionType::Eq:
				case InstructionType::Ne:
				case InstructionType::Lt:
				case InstructionType::Gt:
				case InstructionType::Le:
				case InstructionType::Ge: {
					return ValidateComp(inst, logger, errInfo);
				}

				case InstructionType::Exit: {
					return ValidateNumArg(inst, logger, errInfo);
				}

				case InstructionType::Ret: {
					return ValidateNoArg(inst, logger, errInfo);
				}

				case InstructionType::Push:
				case InstructionType::Pop: {
					return ValidatePushPop(inst, logger, errInfo);
				}

				default: {
					logger.Error(errInfo.CreateMessage("invalid instruction"));
					return false;
				}
			}

			return false;
		}

		///T Validate
		/// Validates a list of kiwi instructions
		static bool Validate(const Boxx::List<Instruction>& instructions, Boxx::Logger& logger, const ErrorInfo& errInfo = ErrorInfo()) {
			bool valid = true;

			for (Boxx::UInt i = 0; i < instructions.Count(); i++) {
				ErrorInfo err = errInfo;
				err.line = i;

				if (!Validate(instructions[i], logger, err)) {
					valid = false;
				}
			}

			return valid;
		}

	private:
		static bool ValidateMov(const Instruction& instruction, Boxx::Logger& logger, const ErrorInfo& errInfo) {
			if (instruction.arguments.Count() < 2) {
				logger.Error(errInfo.FewArgs());
				return false;
			}
			else if (instruction.arguments.Count() > 2) {
				logger.Error(errInfo.ManyArgs());
				return false;
			}

			if (instruction.type == InstructionType::Adr && instruction.arguments[1].type != ArgumentType::Memory) {
				logger.Error(errInfo.InvalidArgs());
				return false;
			}

			if (
				instruction.arguments[0].type != ArgumentType::Memory &&
				instruction.arguments[0].type != ArgumentType::Register
				) {
				logger.Error(errInfo.InvalidArgs());
				return false;
			}

			if (
				instruction.arguments[1].type != ArgumentType::Memory &&
				instruction.arguments[1].type != ArgumentType::Register &&
				instruction.arguments[1].type != ArgumentType::Number
				) {
				logger.Error(errInfo.InvalidArgs());
				return false;
			}

			return true;
		}

		static bool ValidateBinOp(const Instruction& instruction, Boxx::Logger& logger, const ErrorInfo& errInfo) {
			if (instruction.arguments.Count() == 2) {
				if (
					instruction.arguments[0].type != ArgumentType::Memory &&
					instruction.arguments[0].type != ArgumentType::Register
					) {
					logger.Error(errInfo.InvalidArgs());
					return false;
				}

				if (
					instruction.arguments[1].type != ArgumentType::Memory &&
					instruction.arguments[1].type != ArgumentType::Register &&
					instruction.arguments[1].type != ArgumentType::Number
					) {
					logger.Error(errInfo.InvalidArgs());
					return false;
				}

				return true;
			}
			else if (instruction.arguments.Count() == 3) {
				if (
					instruction.arguments[0].type != ArgumentType::Memory &&
					instruction.arguments[0].type != ArgumentType::Register &&
					instruction.arguments[0].type != ArgumentType::Number
					) {
					logger.Error(errInfo.InvalidArgs());
					return false;
				}

				if (
					instruction.arguments[1].type != ArgumentType::Memory &&
					instruction.arguments[1].type != ArgumentType::Register &&
					instruction.arguments[1].type != ArgumentType::Number
					) {
					logger.Error(errInfo.InvalidArgs());
					return false;
				}

				if (
					instruction.arguments[2].type != ArgumentType::Memory &&
					instruction.arguments[2].type != ArgumentType::Register
					) {
					logger.Error(errInfo.InvalidArgs());
					return false;
				}

				return true;
			}
			else if (instruction.arguments.Count() < 2) {
				logger.Error(errInfo.FewArgs());
				return false;
			}
			else if (instruction.arguments.Count() > 3) {
				logger.Error(errInfo.ManyArgs());
				return false;
			}

			return false;
		}

		static bool ValidateUnaryOp(const Instruction& instruction, Boxx::Logger& logger, const ErrorInfo& errInfo) {
			if (instruction.arguments.Count() == 1) {
				if (
					instruction.arguments[0].type != ArgumentType::Memory &&
					instruction.arguments[0].type != ArgumentType::Register
					) {
					logger.Error(errInfo.InvalidArgs());
					return false;
				}

				return true;
			}
			else if (instruction.arguments.Count() == 2) {
				if (
					instruction.arguments[0].type != ArgumentType::Memory &&
					instruction.arguments[0].type != ArgumentType::Register &&
					instruction.arguments[0].type != ArgumentType::Number
					) {
					logger.Error(errInfo.InvalidArgs());
					return false;
				}

				if (
					instruction.arguments[1].type != ArgumentType::Memory &&
					instruction.arguments[1].type != ArgumentType::Register
					) {
					logger.Error(errInfo.InvalidArgs());
					return false;
				}

				return true;
			}
			else if (instruction.arguments.Count() < 1) {
				logger.Error(errInfo.FewArgs());
				return false;
			}
			else if (instruction.arguments.Count() > 2) {
				logger.Error(errInfo.ManyArgs());
				return false;
			}

			return false;
		}

		static bool ValidateLabel(const Instruction& instruction, Boxx::Logger& logger, const ErrorInfo& errInfo) {
			if (!instruction.arguments.IsEmpty()) {
				logger.Error(errInfo.ManyArgs());
				return false;
			}

			if (instruction.instructionName.IsEmpty()) {
				logger.Error(errInfo.CreateMessage("invalid name"));
				return false;
			}

			return true;
		}

		static bool ValidateCall(const Instruction& instruction, Boxx::Logger& logger, const ErrorInfo& errInfo) {
			if (instruction.arguments.Count() < 1) {
				logger.Error(errInfo.FewArgs());
				return false;
			}
			else if (instruction.arguments.Count() > 1) {
				logger.Error(errInfo.ManyArgs());
				return false;
			}

			if (instruction.arguments[0].type != ArgumentType::Name) {
				logger.Error(errInfo.InvalidArgs());
				return false;
			}

			return true;
		}

		static bool ValidateJmp(const Instruction& instruction, Boxx::Logger& logger, const ErrorInfo& errInfo) {
			if (instruction.arguments.Count() < 1) {
				logger.Error(errInfo.FewArgs());
				return false;
			}
			else if (instruction.arguments.Count() > 1) {
				logger.Error(errInfo.ManyArgs());
				return false;
			}

			if (instruction.arguments[0].type != ArgumentType::Label) {
				logger.Error(errInfo.InvalidArgs());
				return false;
			}

			return true;
		}

		static bool ValidateNumArg(const Instruction& instruction, Boxx::Logger& logger, const ErrorInfo& errInfo) {
			if (instruction.arguments.Count() < 1) {
				logger.Error(errInfo.FewArgs());
				return false;
			}
			else if (instruction.arguments.Count() > 1) {
				logger.Error(errInfo.ManyArgs());
				return false;
			}

			if (
				instruction.arguments[0].type != ArgumentType::Register &&
				instruction.arguments[0].type != ArgumentType::Memory &&
				instruction.arguments[0].type != ArgumentType::Number
				) {
				logger.Error(errInfo.InvalidArgs());
				return false;
			}

			return true;
		}

		static bool ValidateNoArg(const Instruction& instruction, Boxx::Logger& logger, const ErrorInfo& errInfo) {
			if (instruction.arguments.Count() > 0) {
				logger.Error(errInfo.ManyArgs());
				return false;
			}

			return true;
		}

		static bool ValidateComp(const Instruction& instruction, Boxx::Logger& logger, const ErrorInfo& errInfo) {
			if (instruction.arguments.Count() == 2) {
				if (
					instruction.arguments[0].type != ArgumentType::Memory &&
					instruction.arguments[0].type != ArgumentType::Register
					) {
					logger.Error(errInfo.InvalidArgs());
					return false;
				}

				if (
					instruction.arguments[1].type != ArgumentType::Memory &&
					instruction.arguments[1].type != ArgumentType::Register &&
					instruction.arguments[1].type != ArgumentType::Number
					) {
					logger.Error(errInfo.InvalidArgs());
					return false;
				}

				return true;
			}
			else if (instruction.arguments.Count() == 3) {
				if (
					instruction.arguments[0].type != ArgumentType::Memory &&
					instruction.arguments[0].type != ArgumentType::Register &&
					instruction.arguments[0].type != ArgumentType::Number
					) {
					logger.Error(errInfo.InvalidArgs());
					return false;
				}

				if (
					instruction.arguments[1].type != ArgumentType::Memory &&
					instruction.arguments[1].type != ArgumentType::Register &&
					instruction.arguments[1].type != ArgumentType::Number
					) {
					logger.Error(errInfo.InvalidArgs());
					return false;
				}

				if (
					instruction.arguments[2].type != ArgumentType::Memory &&
					instruction.arguments[2].type != ArgumentType::Register &&
					instruction.arguments[2].type != ArgumentType::Label
					) {
					logger.Error(errInfo.InvalidArgs());
					return false;
				}

				return true;
			}
			else if (instruction.arguments.Count() < 2) {
				logger.Error(errInfo.FewArgs());
				return false;
			}
			else if (instruction.arguments.Count() > 3) {
				logger.Error(errInfo.ManyArgs());
				return false;
			}

			return false;
		}

		static bool ValidatePushPop(const Instruction& instruction, Boxx::Logger& logger, const ErrorInfo& errInfo) {
			const bool isPush = instruction.type == InstructionType::Push;

			if (instruction.arguments.Count() > 0) {
				logger.Warning(errInfo.CreateMessage(Boxx::String(isPush ? "Push" : "Pop") + " instruction might not work"));
				return false;
			}

			if (instruction.sizes[0] == 0) {
				logger.Error(errInfo.CreateMessage("size can not be 0"));
			}

			return true;
		}
	};

	inline void Converter::ConvertToFile(Boxx::FileWriter& file, const Boxx::List<Instruction>& instructions) {
		for (const Instruction& inst : instructions) {
			file.Write(ConvertInstruction(inst));
		}
	}
}
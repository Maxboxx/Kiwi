#pragma once

#include "Boxx/Boxx/String.h"
#include "Boxx/Boxx/Logger.h"
#include "Boxx/Boxx/List.h"
#include "Boxx/Boxx/Pointer.h"
#include "Boxx/Boxx/File.h"
#include "Boxx/Boxx/Regex.h"

#include "x86_64Converter.h"

namespace KiwiOld {
	class x86_64Tester {
	private:
		static Boxx::String argType(const ArgumentType type) {
			switch (type) {
				case ArgumentType::Register: return "R";
				case ArgumentType::Memory:   return "S";
				case ArgumentType::Number:   return "N";
				case ArgumentType::Label:    return "L";
				case ArgumentType::Function: return "F";
				default: return "";
			}
		}

	public:

		static void TestAssembly() {
			using namespace Boxx;
			Logger logger = Logger("tests/test.log");

			TestBinOp(logger, InstructionType::Add, "tests/binop");
			TestBinOp(logger, InstructionType::Sub, "tests/binopnoswap");
			TestBinOp(logger, InstructionType::Nor, "tests/binopbitnot");
			TestBinOp(logger, InstructionType::ShL, "tests/binopshift");
			TestBinOp(logger, InstructionType::Mul, "tests/binopmul");
			TestBinOp(logger, InstructionType::Div, "tests/binopdiv");
			TestBinOp(logger, InstructionType::Mod, "tests/binopmod");

			TestBinOpThree(logger, InstructionType::Xor, "tests/binopthree");
			TestBinOpThree(logger, InstructionType::Mul, "tests/binopthreemul");
			TestBinOpThree(logger, InstructionType::Div, "tests/binopthreediv");
			TestBinOpThree(logger, InstructionType::Mod, "tests/binopthreemod");

			TestUnOp(logger, InstructionType::Neg, "tests/unop");

			TestComp(logger, InstructionType::Lt, "tests/comp");
			TestCompThree(logger, InstructionType::Ge, "tests/compthree");
		
			//*/
		}

	private:
		static void TestBinOp(Boxx::Logger& logger, const InstructionType type, const Boxx::String& file) {
			using namespace Boxx;
	
			List<UByte> sizes;
			sizes.Add(1);
			sizes.Add(2);
			sizes.Add(4);
			sizes.Add(8);

			List<Argument> args1;
			args1.Add(Register(0));
			args1.Add(MemoryLocation(-1));

			List<Argument> args2;
			args2.Add(Register(1));
			args2.Add(MemoryLocation(-2));
			args2.Add(Argument(Math::ULongMax()));

			List<List<Argument>> args;
			args.Add(args1);
			args.Add(args2);

			TestInsts(sizes, args, logger, type, file);
		}

		static void TestBinOpThree(Boxx::Logger& logger, const InstructionType type, const Boxx::String& file) {
			using namespace Boxx;
	
			List<UByte> sizes;
			sizes.Add(1);
			sizes.Add(4);
			sizes.Add(8);

			List<Argument> args1;
			args1.Add(Register(0));
			args1.Add(MemoryLocation(-1));
			args1.Add(Argument(Math::ULongMax()));

			List<Argument> args2;
			args2.Add(Register(1));
			args2.Add(MemoryLocation(-2));
			args2.Add(Argument(Math::ULongMax()));

			List<Argument> args3;
			args3.Add(Register(2));
			args3.Add(MemoryLocation(-3));

			List<List<Argument>> args;
			args.Add(args1);
			args.Add(args2);
			args.Add(args3);

			TestInsts(sizes, args, logger, type, file);
		}

		static void TestUnOp(Boxx::Logger& logger, const InstructionType type, const Boxx::String& file) {
			using namespace Boxx;

			UByte sign = 0;

			List<Instruction> insts;
	
			List<UByte> sizes;
			sizes.Add(1);
			sizes.Add(2);
			sizes.Add(4);
			sizes.Add(8);

			List<Argument> args1;
			args1.Add(Register(0));
			args1.Add(MemoryLocation(-1));
			args1.Add(Argument(Math::ULongMax()));

			List<Argument> args2;
			args2.Add(Register(1));
			args2.Add(MemoryLocation(-2));

			for (const UByte size1 : sizes) {
				for (const Argument arg1 : args2) {
					Instruction lbl = Instruction(InstructionType::Label);
					lbl.instructionName = argType(arg1.type) + String::ToString(size1) + (sign & 1 ? "U" : "");
					insts.Add(lbl);

					Instruction inst = Instruction(type);
					inst.arguments.Add(arg1);
					inst.sizes[0] = size1;
					inst.signs[0] = sign & 1;
					insts.Add(inst);

					sign++;
				
					if (sign > 1) sign = 0;
				}
			}

			for (const UByte size1 : sizes) {
				for (const UByte size2 : sizes) {
					for (const Argument arg1 : args1) {
						for (const Argument arg2 : args2) {
							Instruction lbl = Instruction(InstructionType::Label);
							lbl.instructionName = argType(arg1.type) + String::ToString(size1) + (sign & 1 ? "U_" : "_") + argType(arg2.type) + String::ToString(size2) + (sign & 2 ? "U" : "");
							insts.Add(lbl);

							Instruction inst = Instruction(type);
							inst.arguments.Add(arg1);
							inst.arguments.Add(arg2);
							inst.sizes[0] = size1;
							inst.sizes[1] = size2;
							inst.signs[0] = sign & 1;
							inst.signs[1] = (sign & 2) >> 1;
							insts.Add(inst);

							sign++;

							if (sign > 3) sign = 0;
						}
					}
				}
			}
		
			Pointer<Converter> kiwiConverter = new Converter(logger);
			Pointer<x86_64Converter> asmConverter = new x86_64Converter(x86_64Syntax::Intel, logger);
			Pointer<x86_64Converter> sConverter = new x86_64Converter(x86_64Syntax::ATnT, logger);

			KiwiLang::WriteToFile("tests/temp.kiwi", kiwiConverter, insts);
			KiwiLang::WriteToFile("tests/temp.asm", asmConverter, insts);
			KiwiLang::WriteToFile("tests/temp.s", sConverter, insts);

			CompareFiles("tests/temp.asm", file + ".asm", logger);
			CompareFiles("tests/temp.s", file + ".s", logger);
		}

		static void TestComp(Boxx::Logger& logger, const InstructionType type, const Boxx::String& file) {
			using namespace Boxx;
	
			List<UByte> sizes;
			sizes.Add(1);
			sizes.Add(2);
			sizes.Add(4);
			sizes.Add(8);

			List<Argument> args1;
			args1.Add(Register(0));
			args1.Add(MemoryLocation(-1));

			List<Argument> args2;
			args2.Add(Register(1));
			args2.Add(MemoryLocation(-2));
			args2.Add(Argument(Math::ULongMax()));

			List<List<Argument>> args;
			args.Add(args1);
			args.Add(args2);

			TestInsts(sizes, args, logger, type, file);
		}

		static void TestCompThree(Boxx::Logger& logger, const InstructionType type, const Boxx::String& file) {
			using namespace Boxx;
	
			List<UByte> sizes;
			sizes.Add(1);
			sizes.Add(4);
			sizes.Add(8);

			List<Argument> args1;
			args1.Add(Register(0));
			args1.Add(MemoryLocation(-1));
			args1.Add(Argument(Math::ULongMax()));

			List<Argument> args2;
			args2.Add(Register(1));
			args2.Add(MemoryLocation(-2));
			args2.Add(Argument(Math::ULongMax()));

			List<Argument> args3;
			args3.Add(Register(2));
			args3.Add(MemoryLocation(-3));
			args3.Add(Argument(ArgumentType::Label, 3));

			List<List<Argument>> args;
			args.Add(args1);
			args.Add(args2);
			args.Add(args3);

			TestInsts(sizes, args, logger, type, file);
		}

		static void TestOneArg(Boxx::List<Instruction>& insts, const Boxx::List<Boxx::UByte>& sizes, const Boxx::List<Argument>& args1, const InstructionType type) {
			using namespace Boxx;

			bool sign = true;

			for (const UByte size1 : sizes) {
				for (const Argument arg1 : args1) {
					Instruction lbl = Instruction(InstructionType::Label);
					lbl.instructionName = argType(arg1.type) + String::ToString(size1);
					insts.Add(lbl);

					Instruction inst = Instruction(type);
					inst.arguments.Add(arg1);
					inst.sizes[0] = size1;
					inst.signs[0] = sign;
					insts.Add(inst);

					sign = !sign;
				}
			}
		}

		static void TestTwoArg(Boxx::List<Instruction>& insts, const Boxx::List<Boxx::UByte>& sizes, const Boxx::List<Argument>& args1, const Boxx::List<Argument>& args2, const InstructionType type) {
			using namespace Boxx;

			UByte sign = 0;

			for (const UByte size1 : sizes) {
				for (const UByte size2 : sizes) {
					for (const Argument arg1 : args1) {
						for (const Argument arg2 : args2) {
							Instruction lbl = Instruction(InstructionType::Label);
							lbl.instructionName = argType(arg1.type) + String::ToString(size1) + (sign & 1 ? "_" : "U_") + argType(arg2.type) + String::ToString(size2) + (sign & 2 ? "" : "U");
							insts.Add(lbl);

							Instruction inst = Instruction(type);
							inst.arguments.Add(arg1);
							inst.arguments.Add(arg2);
							inst.sizes[0] = size1;
							inst.sizes[1] = size2;
							inst.signs[0] = sign & 1;
							inst.signs[1] = (sign & 2) >> 1;
							insts.Add(inst);

							sign++;

							if (sign > 3) sign = 0;
						}
					}
				}
			}
		}

		static void TestThreeArg(Boxx::List<Instruction>& insts, const Boxx::List<Boxx::UByte>& sizes, const Boxx::List<Argument>& args1, const Boxx::List<Argument>& args2, const Boxx::List<Argument>& args3, const InstructionType type) {
			using namespace Boxx;

			UByte sign = 0;

			for (const UByte size1 : sizes) {
				for (const UByte size2 : sizes) {
					for (const UByte size3 : sizes) {
						for (const Argument arg1 : args1) {
							for (const Argument arg2 : args2) {
								for (const Argument arg3 : args3) {
									Instruction lbl = Instruction(InstructionType::Label);
									lbl.instructionName = argType(arg1.type) + String::ToString(size1) + (sign & 1 ? "_" : "U_") + argType(arg2.type) + String::ToString(size2) + (sign & 2 ? "_" : "U_") + argType(arg3.type) + String::ToString(size3) + (sign & 4 ? "" : "U");
									insts.Add(lbl);

									Instruction inst = Instruction(type);
									inst.arguments.Add(arg1);
									inst.arguments.Add(arg2);
									inst.arguments.Add(arg3);
									inst.sizes[0] = size1;
									inst.sizes[1] = size2;
									inst.sizes[2] = size3;
									inst.signs[0] = sign & 1;
									inst.signs[1] = (sign & 2) >> 1;
									inst.signs[2] = (sign & 4) >> 2;
									insts.Add(inst);

									sign++;

									if (sign > 7) sign = 0;
								}
							}
						}
					}
				}
			}
		}

		static void TestInsts(const Boxx::List<Boxx::UByte>& sizes, const Boxx::List<Boxx::List<Argument>>& args, Boxx::Logger& logger, const InstructionType type, const Boxx::String& file) {
			using namespace Boxx;

			bool sign = true;

			List<Instruction> insts;
	
			if (args.Size() == 1) {
				TestOneArg(insts, sizes, args[0], type);
			}
			else if (args.Size() == 2) {
				TestTwoArg(insts, sizes, args[0], args[1], type);
			}
			else if (args.Size() == 3) {
				TestThreeArg(insts, sizes, args[0], args[1], args[2], type);
			}
		
			Pointer<Converter> kiwiConverter = new Converter(logger);
			Pointer<x86_64Converter> asmConverter = new x86_64Converter(x86_64Syntax::Intel, logger);
			Pointer<x86_64Converter> sConverter = new x86_64Converter(x86_64Syntax::ATnT, logger);

			KiwiLang::WriteToFile("tests/temp.kiwi", kiwiConverter, insts);
			KiwiLang::WriteToFile("tests/temp.asm", asmConverter, insts);
			KiwiLang::WriteToFile("tests/temp.s", sConverter, insts);

			CompareFiles("tests/temp.asm", file + ".asm", logger);
			CompareFiles("tests/temp.s", file + ".s", logger);
		}

		static void CompareFiles(const Boxx::String& filename1, const Boxx::String& filename2, Boxx::Logger& logger) {
			using namespace Boxx;
			static Regex label = Regex("(%v+%w*)%:");

			FileReader file1 = FileReader(filename1);
			FileReader file2 = FileReader(filename2);

			List<String> lines1, lines2;

			for (const String& str : file1) {
				if (str != "") lines1.Add(str);
			}

			for (const String& str : file2) {
				if (str != "") lines2.Add(str);
			}

			UInt i = 0, u = 0;

			while (i < lines1.Size() && u < lines2.Size()) {
				while (i < lines1.Size() && label.Match(lines1[i])) i++;
				while (u < lines2.Size() && label.Match(lines2[u])) u++;

				if (i >= lines1.Size()) break;

				String lbl = label.Match(lines1[i])->groups[0];

				i++;
				u++;

				while (i < lines1.Size() && i < lines2.Size() && label.Match(lines1[i])) {
					if (lines1[i] != lines2[u]) {
						logger.Error(filename2 + ": " + lbl + " mismatch");
						i++;
						u++;
						break;
					}

					i++;
					u++;
				}
			}
		}
	};
}


#include "Old/Kiwi.h"
#include "Old/x86_64Converter.h"
#include "Old/x86_64Tester.h"

#include "KiwiProgram.h"

#include "Boxx/Boxx/String.h"
#include "Boxx/Boxx/StringBuilder.h"
#include "Boxx/Boxx/Logger.h"
#include "Boxx/Boxx/List.h"
#include "Boxx/Boxx/Math.h"
#include "Boxx/Boxx/File.h"
#include "Boxx/Boxx/System.h"

using namespace Boxx;

using namespace Kiwi;

int main() {
	Ptr<KiwiProgram> program = new KiwiProgram();

	Ptr<InstructionBlock> block = new InstructionBlock();

	block->AddInstruction(new AssignInstruction("i32", "a", new Integer(34)));

	Ptr<CallExpression> call = new CallExpression("f");
	call->args.Add(new Variable("a"));

	block->AddInstruction(new AssignInstruction("i32", "b", call));
	block->AddInstruction(new DebugPrintInstruction(new Variable("b")));

	program->AddCodeBlock(block);

	Ptr<Function> func = new Function("f");
	func->AddArgument("i32", "i");
	func->AddReturnValue("i32", "r");
	func->AddInstruction(new AssignInstruction("r", new Variable("i")));

	program->AddFunction(func);

	StringBuilder builder;
	program->BuildString(builder);

	FileWriter writer = FileWriter("Local/temp.kiwi");
	writer.Write(builder.ToString());
	writer.Close();

	Interpreter::InterpreterData data;
	data.program = program;
	program->Interpret(data);

	System::Execute("pause");

    return 0;
}

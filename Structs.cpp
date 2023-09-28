
#include "Structs.h"

#include "KiwiProgram.h"

using namespace Boxx;

using namespace Kiwi;

UInt Type::SizeOf(Type type, Weak<KiwiProgram> program) {
	if (type.pointers > 0) return KiwiProgram::ptrSize;

	if (type.name == "u8"  || type.name == "i8" ) return 1 * type.len;
	if (type.name == "u16" || type.name == "i16") return 2 * type.len;
	if (type.name == "u32" || type.name == "i32") return 4 * type.len;
	if (type.name == "u64" || type.name == "i64") return 8 * type.len;

	if (program->structs.Contains(type.name)) {
		return program->structs[type.name]->Size(program) * type.len;
	}

	return 0;
}

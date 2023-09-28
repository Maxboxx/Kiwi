#pragma once

#include "Ptr.h"

#include "Boxx/Boxx/StringBuilder.h"
#include "Boxx/Boxx/Regex.h"

///N Kiwi

namespace Kiwi {
	class KiwiProgram;

	/// Utility class for kiwi names.
	class Name final {
	public:
		/// Converts the specified name to a valid kiwi string.
		static Boxx::String ToKiwi(const Boxx::String& name) {
			using namespace Boxx;

			static Regex wordPattern = Regex("^%w+$");
			static Regex numPattern  = Regex("^%d+$");

			if (wordPattern.Match(name)) {
				if (numPattern.Match(name)) {
					return '@' + name;
				}
				else {
					return name;
				}
			}
			else {
				return '`' + name + '`';
			}
		}
	};

	/// A Kiwi type.
	struct Type {
		/// The pointer depth.
		Boxx::UInt pointers;

		/// The type name.
		Boxx::String name;

		/// The array length.
		Boxx::UInt len;

		Type() : pointers(0), name(""), len(1) {}
		explicit Type(const Boxx::String& type) : pointers(0), name(type), len(1) {}
		Type(Boxx::UInt pointers, const Boxx::String& type) : pointers(pointers), name(type), len(1) {}
		Type(const Boxx::String& type, Boxx::UInt len) : pointers(0), name(type), len(len) {}
		Type(Boxx::UInt pointers, const Boxx::String& type, Boxx::UInt len) : pointers(pointers), name(type), len(len) {}
		~Type() {}

		/// Converts the type to kiwi.
		Boxx::String ToKiwi() const {
			Boxx::String str = Name::ToKiwi(name);
			
			if (len != 1) {
				str += '[' + Boxx::String::ToString(len) + ']';
			}

			if (pointers) {
				str += Boxx::String("*").Repeat(pointers);
			}

			return str;
		}

		bool operator==(const Type& type) const {
			return pointers == type.pointers && name == type.name && len == type.len;
		}

		bool operator!=(const Type& type) const {
			return pointers != type.pointers || name != type.name || len != type.len;
		}

		bool operator<(const Type& type) const {
			if (pointers != type.pointers) {
				return pointers < type.pointers;
			}

			if (len != type.len) {
				return len < type.len;
			}

			return name < type.name;
		}

		/// Gets the size of the specified type.
		static Boxx::UInt SizeOf(Type type, Weak<KiwiProgram> program);
	};
}

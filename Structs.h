#pragma once

#include "Ptr.h"

#include "Boxx/Boxx/StringBuilder.h"
#include "Boxx/Boxx/Regex.h"

///N Kiwi

namespace Kiwi {
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

		Type() : pointers(0), name("") {}
		explicit Type(const Boxx::String& type) : pointers(0), name(type) {}
		Type(Boxx::UInt pointers, const Boxx::String& type) : pointers(pointers), name(type) {}
		~Type() {}

		/// Converts the type to kiwi.
		Boxx::String ToKiwi() const {
			return Name::ToKiwi(name) + Boxx::String("*").Repeat(pointers);
		}

		bool operator==(const Type& type) const {
			return pointers == type.pointers && name == type.name;
		}

		bool operator!=(const Type& type) const {
			return pointers != type.pointers || name != type.name;
		}

		bool operator<(const Type& type) const {
			if (pointers != type.pointers) {
				return pointers < type.pointers;
			}

			return name < type.name;
		}
	};
}

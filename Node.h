#pragma once

#include "Ptr.h"

#include "Interpreter/Interpreter.h"

#include "Boxx/Boxx/StringBuilder.h"
#include "Boxx/Boxx/Regex.h"

///N Kiwi

namespace Kiwi {
	/// The base for all Kiwi nodes.
	class Node {
	public:
		virtual ~Node() {}

		/// Interprets the node.
		virtual void Interpret(Interpreter::InterpreterData& data) {}

		/// Builds a string from the node.
		virtual void BuildString(Boxx::StringBuilder& builder) = 0;
	};

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
}

#pragma once

#include "Ptr.h"

#include "Interpreter/Interpreter.h"

#include "Boxx/Boxx/StringBuilder.h"

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
}

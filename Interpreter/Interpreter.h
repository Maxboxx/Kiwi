#pragma once

#include "../Ptr.h"

#include "../Boxx/Boxx/String.h"
#include "../Boxx/Boxx/Map.h"
#include "../Boxx/Boxx/Stack.h"

namespace Kiwi {
	class KiwiProgram;

	namespace Interpreter {
		class Value {
		public:
			virtual ~Value() {}

			/// Copies the value.
			virtual Ptr<Value> Copy() const = 0;

			/// Converts the value to a string.
			virtual Boxx::String ToString() const = 0;
		};

		class Frame {
		public:
			Boxx::Map<Boxx::String, Ptr<Value>> variables;

			virtual ~Frame() {}

			Weak<Value> GetVarValue(const Boxx::String& var) {
				if (!variables.Contains(var)) return nullptr;
				return variables[var];
			}

			Ptr<Value> GetVarValueCopy(const Boxx::String& var) {
				if (!variables.Contains(var)) return nullptr;
				return variables[var]->Copy();
			}

			void SetVarValue(const Boxx::String& var, Ptr<Value> value) {
				if (!value) {
					variables.Remove(var);
				}
				else {
					variables.Set(var, value);
				}
			}
		};

		struct InterpreterData {
			Weak<KiwiProgram> program;
			Boxx::Stack<Ptr<Frame>> frames;
			Weak<Frame> frame;

			~InterpreterData() {}

			void PushFrame() {
				frames.Push(new Frame());
				frame = frames.Peek();
			}

			void PopFrame() {
				frames.Pop();

				if (!frames.IsEmpty()) {
					frame = frames.Peek();
				}
				else {
					frame = nullptr;
				}
			}
		};

		class Int : public Value {
		public:
			virtual Boxx::Long AsLong() const = 0;
		};

		class Int32 : public Int {
		public:
			Boxx::Int value;

			Int32(Boxx::Int value) {
				this->value = value;
			}

			Int32(Weak<Int> integer) {
				this->value = integer->AsLong();
			}

			virtual Boxx::Long AsLong() const override {
				return value;
			}

			virtual Ptr<Value> Copy() const override {
				return new Int32(value);
			}

			virtual Boxx::String ToString() const override {
				return Boxx::String::ToString(value);
			}
		};

		class Int64 : public Int {
		public:
			Boxx::Long value;

			Int64(Boxx::Long value) {
				this->value = value;
			}

			Int64(Weak<Int> integer) {
				this->value = integer->AsLong();
			}

			virtual Boxx::Long AsLong() const override {
				return value;
			}

			virtual Ptr<Value> Copy() const override {
				return new Int64(value);
			}

			virtual Boxx::String ToString() const override {
				return Boxx::String::ToString(value);
			}
		};
	}
}

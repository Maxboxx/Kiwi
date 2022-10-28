#pragma once

#include "../Ptr.h"

#include "../Boxx/Boxx/String.h"
#include "../Boxx/Boxx/Map.h"
#include "../Boxx/Boxx/Stack.h"
#include "../Boxx/Boxx/Error.h"

///N Kiwi::Interpreter

namespace Kiwi {
	class KiwiProgram;

	namespace Interpreter {
		/// Base for interpreter values.
		class Value {
		public:
			virtual ~Value() {}

			/// Copies the value.
			virtual Ptr<Value> Copy() const = 0;

			/// Converts the value to the specified type.
			virtual Ptr<Value> ConvertTo(const Boxx::String& type) const = 0;

			/// Gets the type of the value.
			virtual Boxx::String GetType() const = 0;

			/// Converts the value to a string.
			virtual Boxx::String ToString() const = 0;
		};

		/// A stack frame.
		class Frame {
		public:
			Boxx::Map<Boxx::String, Boxx::String> varTypes;
			Boxx::Map<Boxx::String, Ptr<Value>> variables;

			virtual ~Frame() {}

			/// Gets the value of the specified variable.
			Weak<Value> GetVarValue(const Boxx::String& var) {
				if (!variables.Contains(var)) return nullptr;
				return variables[var];
			}

			/// Gets a copy of the value for the specified variable.
			Ptr<Value> GetVarValueCopy(const Boxx::String& var) {
				if (!variables.Contains(var)) return nullptr;
				return variables[var]->Copy();
			}

			/// Sets the value of a variable.
			void SetVarValue(const Boxx::String& var, Ptr<Value> value) {
				if (!value) {
					variables.Remove(var);
				}
				else {
					variables.Set(var, value);
				}
			}

			/// Gets the type of a variable.
			Boxx::String GetVarType(const Boxx::String& var) {
				if (!varTypes.Contains(var)) return "";
				return varTypes[var];
			}

			/// Sets the type of a variable.
			void SetVarType(const Boxx::String& var, const Boxx::String& type) {
				varTypes.Set(var, type);
			}
		};

		/// Data used by the interpreter.
		struct InterpreterData {
			/// The current Kiwi program.
			Weak<KiwiProgram> program;

			/// All stack frames.
			Boxx::Stack<Ptr<Frame>> frames;

			/// The current stack frame.
			Weak<Frame> frame;

			~InterpreterData() {}

			/// Pushes a new stack frame.
			void PushFrame() {
				frames.Push(new Frame());
				frame = frames.Peek();
			}

			/// Pops a stack frame.
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

		/// An integer value.
		class Integer : public Value {
		public:
			virtual Boxx::Long ToLong() const = 0;
		};

		/// Template class for integer types.
		///M
		template <class T>
		requires std::is_same<T, Boxx::Byte>::value || std::is_same<T, Boxx::UByte>::value || 
			std::is_same<T, Boxx::Short>::value || std::is_same<T, Boxx::UShort>::value ||
			std::is_same<T, Boxx::Int>::value || std::is_same<T, Boxx::UInt>::value ||
			std::is_same<T, Boxx::Long>::value || std::is_same<T, Boxx::ULong>::value
		class Int : public Integer {
		///M
		public:
			/// The integer value.
			T value;

			Int(T value) {
				this->value = value;
			}

			virtual Ptr<Value> Copy() const override {
				return new Int<T>((T)value);
			}

			virtual Boxx::String ToString() const override {
				return Boxx::String::ToString(value);
			}

			virtual Boxx::Long ToLong() const override {
				return value;
			}

			virtual Ptr<Value> ConvertTo(const Boxx::String& type) const override {
				if (type == "i8")  return new Int<Boxx::Byte>(value);
				if (type == "u8")  return new Int<Boxx::UByte>(value);
				if (type == "i16") return new Int<Boxx::Short>(value);
				if (type == "u16") return new Int<Boxx::UShort>(value);
				if (type == "i32") return new Int<Boxx::Int>(value);
				if (type == "u32") return new Int<Boxx::UInt>(value);
				if (type == "i64") return new Int<Boxx::Long>(value);
				if (type == "u64") return new Int<Boxx::ULong>(value);

				return nullptr;
			}

			virtual Boxx::String GetType() const override {
				if constexpr (std::is_same<T, Boxx::Byte>::value)   return "i8";
				if constexpr (std::is_same<T, Boxx::UByte>::value)  return "u8";
				if constexpr (std::is_same<T, Boxx::Short>::value)  return "i16";
				if constexpr (std::is_same<T, Boxx::UShort>::value) return "u16";
				if constexpr (std::is_same<T, Boxx::Int>::value)    return "i32";
				if constexpr (std::is_same<T, Boxx::UInt>::value)   return "u32";
				if constexpr (std::is_same<T, Boxx::Long>::value)   return "i64";
				if constexpr (std::is_same<T, Boxx::ULong>::value)  return "u64";

				return "";
			}
		};

		/// An 8-bit integer value.
		typedef Int<Boxx::Byte>   Int8;

		/// An 8-bit unsigned integer value.
		typedef Int<Boxx::UByte>  UInt8;

		/// A 16-bit integer value.
		typedef Int<Boxx::Short>  Int16;

		/// A 16-bit unsigned integer value.
		typedef Int<Boxx::UShort> UInt16;

		/// A 32-bit integer value.
		typedef Int<Boxx::Int>    Int32;

		/// A 32-bit unsigned integer value.
		typedef Int<Boxx::UInt>   UInt32;

		/// A 64-bit integer value.
		typedef Int<Boxx::Long>   Int64;

		/// A 64-bit unsigned integer value.
		typedef Int<Boxx::ULong>  UInt64;

		/// An struct value.
		class StructValue : public Value {
		public:
			/// The current kiwi program.
			Weak<KiwiProgram> program;

			/// The type of the struct value.
			Boxx::String type;

			/// The struct data.
			Boxx::Map<Boxx::String, Ptr<Value>> data;

			StructValue(const Boxx::String& type, Weak<KiwiProgram> program) {
				this->program = program;
				this->type = type;
			}

			/// Sets the specified struct value.
			void SetValue(const Boxx::String& var, Ptr<Value> value);

			/// Gets the specified struct value.
			Weak<Value> GetValue(const Boxx::String& var) const {
				if (data.Contains(var)) {
					return data[var];
				}

				return nullptr;
			}

			virtual Ptr<Value> Copy() const {
				Ptr<StructValue> value = new StructValue(type, program);

				for (const Boxx::Pair<Boxx::String, Ptr<Value>>& pair : data) {
					value->SetValue(pair.key, pair.value->Copy());
				}

				return value;
			}

			virtual Ptr<Value> ConvertTo(const Boxx::String& type) const {
				if (this->type == type) {
					return Copy();
				}

				return nullptr;
			}

			virtual Boxx::String GetType() const {
				return type;
			}

			virtual Boxx::String ToString() const;
		};

		/// Error for interpreting.
		class KiwiInterpretError : public Boxx::Error {
		public:
			KiwiInterpretError() : Boxx::Error() {}
			KiwiInterpretError(const char* const msg) : Boxx::Error(msg) {}

			virtual Boxx::String Name() const override {
				return "KiwiInterpretError";
			}
		};
	}
}

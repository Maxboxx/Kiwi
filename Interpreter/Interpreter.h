#pragma once

#include "../Ptr.h"

#include "../Boxx/Boxx/String.h"
#include "../Boxx/Boxx/Map.h"
#include "../Boxx/Boxx/Stack.h"

///N Kiwi::Interpreter

namespace Kiwi {
	class KiwiProgram;

	namespace Interpreter {
		/// Enum for all the value types.
		enum class ValueType : Boxx::UByte {
			None,
			Int8,
			UInt8,
			Int16,
			UInt16,
			Int32,
			UInt32,
			Int64,
			UInt64
		};
		
		/// Base for interpreter values.
		class Value {
		public:
			virtual ~Value() {}

			/// Copies the value.
			virtual Ptr<Value> Copy() const = 0;

			/// Converts the value to the specified type.
			virtual Ptr<Value> ConvertTo(const ValueType type) const = 0;

			/// Converts the value to a string.
			virtual Boxx::String ToString() const = 0;

			/// Gets the type with the specified name.
			static ValueType GetType(const Boxx::String& type) {
				if (type == "i8")  return ValueType::Int8;
				if (type == "u8")  return ValueType::UInt8;
				if (type == "i16") return ValueType::Int16;
				if (type == "u16") return ValueType::UInt16;
				if (type == "i32") return ValueType::Int32;
				if (type == "u32") return ValueType::UInt32;
				if (type == "i64") return ValueType::Int64;
				if (type == "u64") return ValueType::UInt64;

				return ValueType::None;
			}
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

			virtual Ptr<Value> ConvertTo(const ValueType type) const override {
				switch (type) {
					case ValueType::Int8:   return new Int<Boxx::Byte>(value);
					case ValueType::UInt8:  return new Int<Boxx::UByte>(value);
					case ValueType::Int16:  return new Int<Boxx::Short>(value);
					case ValueType::UInt16: return new Int<Boxx::UShort>(value);
					case ValueType::Int32:  return new Int<Boxx::Int>(value);
					case ValueType::UInt32: return new Int<Boxx::UInt>(value);
					case ValueType::Int64:  return new Int<Boxx::Long>(value);
					case ValueType::UInt64: return new Int<Boxx::ULong>(value);

					default: return nullptr;
				}
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
	}
}

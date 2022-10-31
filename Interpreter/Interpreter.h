#pragma once

#include "../Ptr.h"

#include "../Boxx/Boxx/String.h"
#include "../Boxx/Boxx/Map.h"
#include "../Boxx/Boxx/Stack.h"
#include "../Boxx/Boxx/Error.h"

#include "../Structs.h"

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
			virtual Ptr<Value> ConvertTo(const Type& type) const = 0;

			/// Gets the type of the value.
			virtual Type GetType() const = 0;

			/// Converts the value to a string.
			virtual Boxx::String ToString() const = 0;
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

		/// A stack frame.
		class Frame {
		public:
			Boxx::Map<Boxx::String, Type> varTypes;
			Boxx::Map<Boxx::String, Ptr<Value>> variables;

			virtual ~Frame() {}

			/// Gets the value of the specified variable.
			Weak<Value> GetVarValue(const Boxx::String& var) {
				if (!variables.Contains(var)) {
					throw KiwiInterpretError("Variable '" + var + "' does not have a value");
				}

				return variables[var];
			}

			/// Gets a copy of the value for the specified variable.
			Ptr<Value> GetVarValueCopy(const Boxx::String& var) {
				if (!variables.Contains(var)) {
					throw KiwiInterpretError("Variable '" + var + "' does not have a value");
				}

				return variables[var]->Copy();
			}

			/// Sets the value of a variable.
			void SetVarValue(const Boxx::String& var, Ptr<Value> value) {
				if (!varTypes.Contains(var)) {
					throw KiwiInterpretError("Variable '" + var + "' does not exist");
				}

				if (!value) {
					throw KiwiInterpretError("Invalid value to assign to '" + var + "'");
				}

				Ptr<Value> converted = value->ConvertTo(varTypes[var]);
				
				if (!converted) {
					throw KiwiInterpretError("Could not assign value of type " + value->GetType().ToKiwi() + " to '" + var + "', " + varTypes[var].ToKiwi() + " expected");
				}

				variables.Set(var, converted);
			}

			/// Gets the type of a variable.
			Type GetVarType(const Boxx::String& var) {
				if (!varTypes.Contains(var)) {
					throw KiwiInterpretError("Variable '" + var + "' does not exist");
				}

				return varTypes[var];
			}

			/// Creates a variable.
			void CreateVariable(const Boxx::String& var, const Type& type) {
				if (varTypes.Contains(var)) {
					throw KiwiInterpretError("Variable '" + var + "' already exists");
				}

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
			virtual void SetLong(Boxx::Long value) = 0;
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

			virtual void SetLong(Boxx::Long value) override {
				this->value = value;
			}

			virtual Ptr<Value> ConvertTo(const Type& type) const override {
				if (type.pointers > 0) return nullptr;

				if (type.name == "i8")  return new Int<Boxx::Byte>(value);
				if (type.name == "u8")  return new Int<Boxx::UByte>(value);
				if (type.name == "i16") return new Int<Boxx::Short>(value);
				if (type.name == "u16") return new Int<Boxx::UShort>(value);
				if (type.name == "i32") return new Int<Boxx::Int>(value);
				if (type.name == "u32") return new Int<Boxx::UInt>(value);
				if (type.name == "i64") return new Int<Boxx::Long>(value);
				if (type.name == "u64") return new Int<Boxx::ULong>(value);

				return nullptr;
			}

			virtual Type GetType() const override {
				if constexpr (std::is_same<T, Boxx::Byte>::value)   return Type("i8");
				if constexpr (std::is_same<T, Boxx::UByte>::value)  return Type("u8");
				if constexpr (std::is_same<T, Boxx::Short>::value)  return Type("i16");
				if constexpr (std::is_same<T, Boxx::UShort>::value) return Type("u16");
				if constexpr (std::is_same<T, Boxx::Int>::value)    return Type("i32");
				if constexpr (std::is_same<T, Boxx::UInt>::value)   return Type("u32");
				if constexpr (std::is_same<T, Boxx::Long>::value)   return Type("i64");
				if constexpr (std::is_same<T, Boxx::ULong>::value)  return Type("u64");

				return Type();
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
			Type type;

			/// The struct data.
			Boxx::Map<Boxx::String, Ptr<Value>> data;

			StructValue(const Type& type, Weak<KiwiProgram> program) {
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

			virtual Ptr<Value> ConvertTo(const Type& type) const {
				if (this->type == type) {
					return Copy();
				}

				return nullptr;
			}

			virtual Type GetType() const {
				return type;
			}

			virtual Boxx::String ToString() const;
		};

		/// A pointer value.
		class PtrValue : public Value {
		public:
			/// The type.
			Type type;

			/// The value pointed at.
			Weak<Value> value;

			PtrValue(const Type& type, Weak<Value> value) {
				this->type  = type;
				this->value = value;
			}

			virtual Ptr<Value> Copy() const {
				return new PtrValue(type, value);
			}

			virtual Ptr<Value> ConvertTo(const Type& type) const {
				if (type.pointers == 0) {
					return nullptr;
				}

				return Copy();
			}

			virtual Type GetType() const {
				return type;
			}

			virtual Boxx::String ToString() const;
		};
	}
}

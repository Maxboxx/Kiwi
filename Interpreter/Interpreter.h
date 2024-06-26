#pragma once

#include "../Ptr.h"

#include "../Boxx/Boxx/String.h"
#include "../Boxx/Boxx/Optional.h"
#include "../Boxx/Boxx/Map.h"
#include "../Boxx/Boxx/Stack.h"
#include "../Boxx/Boxx/Error.h"
#include "../Boxx/Boxx/Math.h"

#include "../Structs.h"

///N Kiwi::Interpreter

namespace Kiwi {
	class KiwiProgram;

	namespace Interpreter {
		using Byte    = Boxx::UByte;
		using DataPtr = Byte*;

		/// Interpreter data.
		struct Data {
		public:
			Data() {
				
			}

			/// Creates data of specified size.
			Data(Boxx::UInt size) {
				data = Boxx::Array<Byte>(size);
			}

			/// Create data from the specified pointer.
			Data(DataPtr ptr, Boxx::UInt size) {
				data = Boxx::Array<Byte>(size);
				std::memcpy((DataPtr)data, ptr, size * sizeof(Byte));
			}

			/// Create data from the specified data.
			Data(const Data& data, Boxx::UInt size) {
				this->data = Boxx::Array<Byte>(size);

				for (Boxx::UInt i = data.Size(); i < size; i++) {
					this->data[i] = 0;
				}

				std::memcpy((DataPtr)this->data, data.Ptr(), Boxx::Math::Min(size, data.Size()) * sizeof(Byte));
			}

			~Data() {
				
			}

			/// Copies the data.
			Data Copy() const {
				Data copy = Data(data.Length());
				std::memcpy((DataPtr)copy.data, (DataPtr)data, data.Length() * sizeof(Byte));
				return copy;
			}

			/// Gets the data pointer.
			DataPtr Ptr() const {
				return (DataPtr)data;
			}

			/// The size of the data.
			Boxx::UInt Size() const {
				return data.Length();
			}

			Boxx::ULong GetNumber(Boxx::UInt size) {
				switch (size) {
					case 1:  return Get<Boxx::Byte>();
					case 2:  return Get<Boxx::Short>();
					case 4:  return Get<Boxx::Int>();
					default: return Get<Boxx::Long>();
				}
			}

			static Data Number(Boxx::UInt size, Boxx::Long num) {
				Data data = Data(size);
				
				switch (size) {
					case 1:  data.Set<Boxx::Byte>((Boxx::Byte)num);   break;
					case 2:  data.Set<Boxx::Short>((Boxx::Short)num); break;
					case 4:  data.Set<Boxx::Int>((Boxx::Int)num);     break;
					default: data.Set<Boxx::Long>((Boxx::Long)num);   break;
				}

				return data;
			}

			template <class T>
			static void Set(DataPtr d, T t) {
				*(T*)d = t;
			}

			template <class T>
			void Set(T t) {
				Set(Ptr(), t);
			}

			static void Set(DataPtr ptr, Data data) {
				std::memcpy(ptr, data.Ptr(), data.data.Length() * sizeof(Interpreter::Byte));
			}

			template <class T>
			static T Get(DataPtr d) {
				return *(T*)d;
			}

			template <class T>
			T Get() {
				return Get<T>(Ptr());
			}

		private:
			Boxx::Array<Byte> data;
		};

		/// Base for interpreter values.
		class Value {
		public:
			virtual ~Value() {}

			/// Sets the value.
			///R success: {true} if the value was set successfully.
			virtual bool SetValue(Weak<Value> value) = 0;

			/// Copies the value.
			virtual Ptr<Value> Copy() const = 0;

			/// Converts the value to the specified type.
			virtual Ptr<Value> ConvertTo(const Type& type) const = 0;

			/// Gets the type of the value.
			virtual Type GetType() const = 0;

			/// Converts the value to a string.
			virtual Boxx::String ToString(Boxx::UInt indent = 1) const = 0;
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
			Boxx::Map<Boxx::String, Kiwi::Type> varTypes;
			Boxx::Map<Boxx::String, Data> variables;

			virtual ~Frame() {}

			/// Gets the value of the specified variable.
			Data GetVarValue(const Boxx::String& var) {
				if (!variables.Contains(var)) {
					throw KiwiInterpretError("Variable '" + var + "' does not have a value");
				}

				return variables[var];
			}

			/// Gets a copy of the value for the specified variable.
			Data GetVarValueCopy(const Boxx::String& var) {
				if (!variables.Contains(var)) {
					throw KiwiInterpretError("Variable '" + var + "' does not have a value");
				}

				return variables[var].Copy();
			}

			/// Sets the value of a variable.
			void SetVarValue(const Boxx::String& var, Data value) {
				if (!varTypes.Contains(var)) {
					throw KiwiInterpretError("Variable '" + var + "' does not exist");
				}

				variables.Set(var, value);
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
				Type varType;

				if (varTypes.Contains(var, varType)) {
					if (type != varType) {
						throw KiwiInterpretError("Variable '" + var + "' already exists with a different type");
					}
				}

				varTypes.Set(var, type);
			}
		};

		/// The memory heap.
		class Heap {
		public:
			Boxx::Map<DataPtr, Data> values;

			virtual ~Heap() {}

			/// Allocates data on the heap.
			Data Alloc(Boxx::UInt size) {
				Data data = Data(size == 0 ? 1 : size);
				values.Add(data.Ptr(), data);
				return data;
			}

			/// Frees up the given data.
			void Free(Data data) {
				Free(data.Ptr());
			}

			/// Frees up the value at the given address.
			void Free(DataPtr data) {
				if (!IsAllocated(data)) {
					throw KiwiInterpretError("Attempt to free memory that is not allocated");
				}

				values.Remove(data);
			}

			/// True if the specified pointer has been allocated.
			bool IsAllocated(DataPtr ptr) {
				return values.Contains(ptr);
			}

			/// Gets the size of the specified pointer.
			Boxx::UInt GetSize(DataPtr ptr) {
				if (!IsAllocated(ptr)) return 0;
				return values[ptr].Size();
			}
		};

		/// Data used by the interpreter.
		struct InterpreterData {
			/// The current Kiwi program.
			Weak<KiwiProgram> program;

			/// The next label to go to.
			Boxx::Optional<Boxx::String> gotoLabel;

			/// All stack frames.
			Boxx::Stack<Ptr<Frame>> frames;

			/// The current stack frame.
			Weak<Frame> frame;

			/// The heap.
			Ptr<Heap> heap = new Heap();

			/// The static data.
			Boxx::Map<Boxx::String, Data> staticData;

			/// A map of function data.
			Boxx::Map<Boxx::String, Boxx::UInt> funcIdMap;

			/// A map of function names.
			Boxx::Map<Boxx::UInt, Boxx::String> funcNameMap;

			/// Used by functions.
			bool ret = false;

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

			virtual bool SetValue(Weak<Value> value) override {
				if (Weak<Interpreter::Integer> intValue = value.As<Interpreter::Integer>()) {
					SetLong(intValue->ToLong());
					return true;
				}

				return false;
			}
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

			virtual Boxx::String ToString(Boxx::UInt indent) const override {
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

		/// A struct value.
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

			virtual bool SetValue(Weak<Value> value) override {
				if (Weak<StructValue> struct_ = value.As<StructValue>()) {
					if (GetType() == struct_->GetType()) {
						data = Boxx::Map<Boxx::String, Ptr<Value>>();

						for (Boxx::Pair<Boxx::String, Ptr<Value>>& pair : struct_->data) {
							data.Add(pair.key, pair.value->Copy());
						}

						return true;
					}
				}

				return false;
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

			virtual Boxx::String ToString(Boxx::UInt indent) const override;
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

			virtual bool SetValue(Weak<Value> value) override {
				return false;
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

			virtual Boxx::String ToString(Boxx::UInt indent) const override;
		};
	}
}

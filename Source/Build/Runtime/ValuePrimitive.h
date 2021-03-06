// <copyright file="ValuePrimitive.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "Value.h"
#include "ValueList.h"

namespace Soup::Build::Runtime
{
	/// <summary>
	/// Build State Extension interface
	/// </summary>
	template<typename T>
	class ValuePrimitive : public IValuePrimitive<T>
	{
	public:
		/// <summary>
		/// Initializes a new instance of the ValuePrimitive class
		/// </summary>
		ValuePrimitive() :
			_value()
		{
		}

		/// <summary>
		/// Initializes a new instance of the ValuePrimitive class
		/// </summary>
		ValuePrimitive(T value) :
			_value(value)
		{
		}

		/// <summary>
		/// Type checker methods
		/// </summary>
		OperationResult TryGetValue(T& value) const noexcept override final
		{
			value = _value;
			return 0;
		}

		OperationResult TrySetValue(T value) noexcept override final
		{
			_value = value;
			return 0;
		}

		/// <summary>
		/// Internal Accessor
		/// </summary>
		std::string ToString() const
		{
			return std::to_string(_value);
		}

		/// <summary>
		/// Equality operator
		/// </summary>
		bool operator ==(const ValuePrimitive<T>& rhs) const
		{
			return _value == rhs._value;
		}

		/// <summary>
		/// Inequality operator
		/// </summary>
		bool operator !=(const ValuePrimitive<T>& rhs) const
		{
			return !(*this == rhs);
		}

	private:
		T _value;
	};

	template<>
	class ValuePrimitive<const char*> : public IValuePrimitive<const char*>
	{
	public:
		/// <summary>
		/// Initializes a new instance of the ValuePrimitive class
		/// </summary>
		ValuePrimitive() :
			_value()
		{
		}
		
		/// <summary>
		/// Initializes a new instance of the ValuePrimitive class
		/// </summary>
		ValuePrimitive(std::string value) :
			_value(std::move(value))
		{
		}

		/// <summary>
		/// Type checker methods
		/// </summary>
		OperationResult TryGetValue(const char*& value) const noexcept override final
		{
			value = _value.c_str();
			return 0;
		}

		OperationResult TrySetValue(const char* value) noexcept override final
		{
			try
			{
				// String assign can fail
				_value = value;
				return 0;
			}
			catch(...)
			{
				// Unknown error
				return -1;
			}
		}

		/// <summary>
		/// Internal Accessor
		/// </summary>
		const std::string& ToString() const noexcept
		{
			return _value;
		}

		/// <summary>
		/// Equality operator
		/// </summary>
		bool operator ==(const ValuePrimitive<const char*>& rhs) const
		{
			return _value == rhs._value;
		}

		/// <summary>
		/// Inequality operator
		/// </summary>
		bool operator !=(const ValuePrimitive<const char*>& rhs) const
		{
			return !(*this == rhs);
		}

	private:
		std::string _value;
	};
}

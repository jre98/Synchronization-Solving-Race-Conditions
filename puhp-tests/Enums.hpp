#ifndef PUHP_TESTS_ENUMS_H
#define PUHP_TESTS_ENUMS_H

//
namespace PuhPTests
{
	/**
	 * Types of assertions
	 */
	enum class AssertionType
	{
		True,
		False,
		Equal,
		Exception,
		NoException
	};
	
	
	/**
	 * Types of log entries
	 */
	enum class LogEntryType
	{
		None,
		Any,
		Info,
		Pass,
		Fail,
		Warning,
		Error
	};
}

#endif


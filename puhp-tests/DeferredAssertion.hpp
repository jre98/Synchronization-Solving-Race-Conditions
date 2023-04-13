#ifndef PUHP_TESTS_DEFERRED_ASSERTION_H
#define PUHP_TESTS_DEFERRED_ASSERTION_H


//
#include "./Enums.hpp"
#include "./Assertions.hpp"


//
#include <functional>
#include <string>


//
namespace PuhPTests
{
	/**
	 * An assertion that won't run right away, but can be run at a later time by a Test
	 */
	class DeferredAssertion
	{
		//
		public:
			
			///	CTOR with one value
			template<class T1>
			DeferredAssertion(std::string label, int points, AssertionType assertionType, T1 value)
			{
				//
				this->label_ = label;
				this->points_ = points;
				this->assertionType_ = assertionType;
				
				//	Assert True
				if ( this->assertionType_ == AssertionType::True ) {
					this->lambda_ = [this, value](std::string& out_message) -> bool
					{
						return Assertions::assertTrue(value, out_message);
					};
				}
				
				//	Assert False
				else if ( this->assertionType_ == AssertionType::False ) {
					this->lambda_ = [this, value](std::string& out_message) -> bool
					{
						return Assertions::assertFalse(value, out_message);
					};
				}
				
				//	Unsupported
				else {
					throw std::runtime_error("DeferredAssertion() does not support type " + Assertions::typeToString(this->assertionType_) + " with one value");
				}
			}
			
			///	CTOR with two values
			template<class T1, class T2>
			DeferredAssertion(std::string label, int points, AssertionType assertionType, T1 value1, T2 value2)
			{
				//
				this->label_ = label;
				this->points_ = points;
				this->assertionType_ = assertionType;
				
				//	Assert Equal
				if ( this->assertionType_ == AssertionType::Equal ) {
					this->lambda_ = [this, value1, value2](std::string& out_message) -> bool
					{
						return Assertions::assertEqual(value1, value2, out_message);
					};
				}
				
				//	Unsupported
				else {
					throw std::runtime_error("DeferredAssertion() does not support type " + Assertions::typeToString(this->assertionType_) + " with two values");
				}
			}
			
			///	CTOR for Lambda
			DeferredAssertion(std::string label, int points, AssertionType assertionType, std::function<void()> func)
			{
				//
				this->label_ = label;
				this->points_ = points;
				this->assertionType_ = assertionType;
				
				//	Assert the Lambda throws an exception
				if ( this->assertionType_ == AssertionType::Exception ) {
					this->lambda_ = [func](std::string& out_message) -> bool
					{
						return Assertions::assertException(func, out_message);
					};
				}
				
				//	Assert the Lambda does not throw an exception
				else if ( this->assertionType_ == AssertionType::NoException ) {
					this->lambda_ = [func](std::string& out_message) -> bool
					{
						return Assertions::assertNoException(func, out_message);
					};
				}
				
				//	Unsupported
				else {
					throw std::runtime_error("DeferredAssertion() does not support type " + Assertions::typeToString(this->assertionType_) + " with a lambda");
				}
			}
			
			//	Getters
			std::string getLabel() { return this->label_; }
			int getPoints() { return this->points_; }
			AssertionType getType() { return this->assertionType_; }
			std::string getMessage() { return this->lambda_out_message_; }
			bool getReadyToRun() { return this->ready_to_run_; }
			bool getResult() { return this->lambda_result_; }
			
			void setReadyToRun(bool b = true)
			{
				this->ready_to_run_ = b;
			}
			
			///	Run the lambda and return its bool
			bool run()
			{
				//
				bool returnValue = false;
				
				try
				{
					returnValue = this->lambda_(this->lambda_out_message_);
				}
				catch( ... )
				{
					returnValue = false;
					this->lambda_out_message_ += "Encountered exception; Assertion failed";
				}
				
				//
				this->lambda_result_ = returnValue;
				this->ready_to_run_ = false;
				
				return returnValue;
			}
		
		//
		private:
			
			//
			std::string label_;
			int points_;
			AssertionType assertionType_;
			
			/// Our internal lambda, which we will use to wrap the assertion test
			std::function<bool(std::string& out_message)> lambda_;
			
			/// Message output from our internal lambda, which should help explain why the test succeeded or failed
			std::string lambda_out_message_;
			
			///	Result of running the lambda (true for success, false for fail)
			bool lambda_result_ = false;
			
			/// Keep track of whether this particular assertion has run
			bool ready_to_run_ = true;
	};
}


#endif

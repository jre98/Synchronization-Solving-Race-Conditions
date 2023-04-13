#ifndef PUHP_TESTS_TEST_H
#define PUHP_TESTS_TEST_H


//
#include "./deps/json.hpp"


//
#include "./ToString.hpp"
#include "./LogEntry.hpp"
#include "./Logs.hpp"
#include "./DeferredAssertion.hpp"


//
#include <memory>
#include <string>
#include <vector>


//
namespace PuhPTests
{
	/**
	 * This class represents one test case.
	 * A test case can contain multiple assertions and point adjustments; It's just a way to logically group things.
	 */
	class Test
	{
		//
		public:
			
			//
			static constexpr const char * DEFAULT_LABEL = "NOLABEL";
			
			//
			Test(std::string label, int points = 0, int points_possible = 0)
				: label_(label), points_(points), points_possible_(points_possible)
			{
				//
				this->logs = std::unique_ptr<Logs>(new Logs(this->label_));
			}
			Test(const Test& other) {
				this->copyOther(other);
			}
			
			//
			std::string getLabel() const { return this->label_; }
			
			/**
			 * Grants points if the passed boolean evaluates to true
			 */
			void assertTrue(bool b, int points = 0, std::string label = Test::DEFAULT_LABEL) {
				
				//
				this->adjustPointsPossible(points, label);
				
				//
				this->deferredAssertions_.push_back(
					DeferredAssertion(label, points, AssertionType::True, b)
				);
			}
			
			/**
			 * Grants points if the passed boolean evaluates to false
			 */
			void assertFalse(bool b, int points = 0, std::string label = Test::DEFAULT_LABEL) {
				
				this->adjustPointsPossible(points, label);
				
				this->deferredAssertions_.push_back(
					DeferredAssertion(label, points, AssertionType::False, b)
				);
			}
			
			/**
			 * Grant points is the two items are "equal"
			 * 
			 * Equality is determined by the existing comparison operator between the two types,
			 * 	which you'll want to make sure are properly overloaded/defined.
			 * 
			 * Works with any two classes that have properly implemented comparison operators.
			 * 
			 * Will also attempt to print out the two items using to_string.
			 * Above, std::to_string is brought into the current namespace,
			 * 	which means you can overload simply "to_string" in this namespace to add support
			 * 	for additional types. See the overload for std::vector near the top of this source file.
			 */
			template<class T1, class T2>
			void assertEqual(T1 a, T2 b, int points = 0, std::string label = Test::DEFAULT_LABEL)
			{
				this->adjustPointsPossible(points, label);
				
				this->deferredAssertions_.push_back(
					DeferredAssertion(label, points, AssertionType::Equal, a, b)
				);
			}
			
			/**
			 * Attempts to run the passed lambda "func",
			 * and grants points if an exception DOES OCCUR.
			 */
			void assertException(std::function<void()> func, int points = 0, std::string label = Test::DEFAULT_LABEL) {
				
				this->adjustPointsPossible(points, label);
				
				this->deferredAssertions_.push_back(
					DeferredAssertion(label, points, AssertionType::Exception, func)
				);
			}
			
			/**
			 * Attempts to run the passed lambda "func",
			 * and grants points if an exception does not occur.
			 */
			void assertNoException(std::function<void()> func, int points = 0, std::string label = Test::DEFAULT_LABEL) {
				
				this->adjustPointsPossible(points, label);
				
				this->deferredAssertions_.push_back(
					DeferredAssertion(label, points, AssertionType::NoException, func)
				);
			}
			
			//
			bool getFailFast() { return this->fail_fast_; }
			void setFailFast(bool b) { this->fail_fast_ = b; }
			
			//
			bool run()
			{
				//
				bool any_failed = false;
				
				//
				for ( size_t i = 0; i < this->deferredAssertions_.size(); i++ ) {
					
					//
					DeferredAssertion& ass = this->deferredAssertions_[i];
					
					//	Don't run anything twice
					if ( ass.getReadyToRun() == false ) {
						continue;
					}
					
					//
					bool passed = ass.run();
					any_failed |= !passed;
					
					//
					std::string assert_message = ass.getMessage();
					std::string log_message = ass.getLabel() + " :: ";
					
					//
					if ( passed == true ) {
						log_message += "Award";
					}
					else {
						log_message += "Unable to award";
					}
					log_message += " " + std::to_string(ass.getPoints()) + " points";
					
					//
					if ( !assert_message.empty() ) {
						log_message += " :: " + assert_message;
					}
					
					//
					if ( passed == true ) {
						this->adjustPoints(
							ass.getPoints(),
							log_message,
							LogEntryType::Pass
						);
					}
					else {
						this->log(LogEntry(log_message, LogEntryType::Fail));
					}
					
					//	Fail fast?
					if ( this->fail_fast_ == true && passed == false ) {
						this->log(LogEntry(
							"Test failed and fail_fast_ is true; Aborting remaining tests.",
							LogEntryType::Warning
						));
						break;
					}
				}
				
				//	Make sure all assertions are unmarked as ready to run
				for ( DeferredAssertion& d : this->deferredAssertions_ ) {
					d.setReadyToRun(false);
				}
				
				return !any_failed;
			}
			
			/// Write a log entry
			void log(const LogEntry& entry) {
				this->logs->log(entry);
			}
			/// Write a log entry
			void log(const std::string& s) {
				this->logs->log(s);
			}
			
			/**
			 * Adjusts points for this test, with a given reason and log entry type.
			 * Pass LogEntryType::None to let the method decide on Pass/Fail, based on the points.
			 * 
			 * You typically won't need to call this; It will be called by assertion methods above.
			 */
			int adjustPoints(int adjustment, std::string reason, LogEntryType log_type = LogEntryType::None) {
				
				//
				int points_old = this->points_;
				this->points_ += adjustment;
				
				//
				if ( log_type == LogEntryType::None) {
					if (adjustment > 0) {
						log_type = LogEntryType::Pass;
					}
					else if (adjustment < 0) {
						log_type = LogEntryType::Fail;
					}
				}
				this->log(
					LogEntry(
						"Adjusting points " + std::to_string(points_old) + " ==> " + std::to_string(this->points_) + " (" + std::to_string(adjustment) + "): " + reason,
						log_type
					)
				);
				
				return this->points_;
			}
			
			/**
			 * Adjust the total points possible.
			 * 
			 * Typically called by an assertion above, and does not need to be called by user code.
			 */
			int adjustPointsPossible(int points, std::string label) {
				
				//
				if (points > 0) {
					this->points_possible_ += points;
					this->log(label + " :: Adjusting points possible to " + std::to_string(this->points_possible_));
				}
				
				return this->points_possible_;
			}
			
			/// Return the number of points earned
			int getPoints() const { return this->points_; }
			
			/// Return the number of points possible
			int getPointsPossible() const { return this->points_possible_; }
			
			///	Wrapper
			std::string getPassFailLogsAsString(size_t label_room = 0) { return this->logs->getPassFailEntriesAsString(label_room); }
			
			/// Render this entire test case to a JSON object
			nlohmann::json toJson() {
				return nlohmann::json {
					{"label", this->label_},
					{"logs", this->logs->getEntriesAsString() },
					{"points", this->points_},
					{"points_possible", this->points_possible_}
				};
			}
			
			/// Render this entire test case to a JSON object, compatible with Gradescope
			nlohmann::json toGradescopeJson() {
				return nlohmann::json {
					{"score", this->points_},
					{"max_score", this->points_possible_},
					{"output", this->logs->getEntriesAsString()},
					{"name", this->label_}
				};
			}
			
			/// Assignment operator overload
			Test& operator=(const Test& rhs) {
				return this->copyOther(rhs);
			}
			
		private:
			
			//
			std::string label_;
			int points_;
			int points_possible_;
			
			/// Whether to fail the whole test on the first failed assertion
			bool fail_fast_ = true;
			
			/// Vector of assertions to run, when the test is run
			std::vector<DeferredAssertion> deferredAssertions_;
			
			///	Logs class
			std::unique_ptr<Logs> logs;
			
			/// Helper function to make this test case into a copy of another.
			Test& copyOther(const Test& other)
			{
				this->label_ = other.label_;
				this->points_ = other.points_;
				this->points_possible_ = other.points_possible_;
				this->fail_fast_ = other.fail_fast_;
				
				this->deferredAssertions_ = other.deferredAssertions_;
				this->logs = std::unique_ptr<Logs>(new Logs(*other.logs));
				
				return *this;
			}
	};
}













#endif

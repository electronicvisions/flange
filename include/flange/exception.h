#pragma once
#include "hate/visibility.h"
#include <exception>
#include <string>

namespace flange {

/**
 * Exception describing a timeout in function calls to the simulator.
 */
class Timeout : public virtual std::exception
{
public:
	/**
	 * Construct timeout from cause message.
	 * @param message Exception cause
	 */
	explicit Timeout(std::string const& message) SYMBOL_VISIBLE;

	/** Destructor. */
	virtual ~Timeout() noexcept override = default;

	/**
	 * Get exception cause.
	 * @return String describing cause of exception
	 */
	virtual const char* what() const noexcept override SYMBOL_VISIBLE;

private:
	std::string const m_message;
};

} // namespace flange

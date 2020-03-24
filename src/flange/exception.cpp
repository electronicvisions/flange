#include "flange/exception.h"

namespace flange {

Timeout::Timeout(std::string const& message) : m_message(message) {}

const char* Timeout::what() const noexcept
{
	return m_message.c_str();
}

} // namespace flange

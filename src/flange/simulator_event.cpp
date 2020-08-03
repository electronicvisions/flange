#include "flange/simulator_event.h"

#include <stdexcept>

#include <SF/utility.hpp>
#include <SF/vector.hpp>

namespace flange {

SimulatorEvent::SimulatorEvent() : event_type(), timestamp(), data() {}

SimulatorEvent::SimulatorEvent(event_types event_type, clk_t timestamp, al_data_t opt_data) :
    event_type(event_type), timestamp(timestamp), data()
{
	if (opt_data.size()) {
		if (event_type != AL_DATA) {
			throw std::runtime_error("Trying to construct non-data event with data.");
		}
		data = opt_data;
	}
}

void SimulatorEvent::serialize(SF::Archive& ar)
{
	// clang-format off
	ar & event_type & timestamp & data;
	// clang-format on
}

bool SimulatorEvent::operator==(SimulatorEvent const& other) const
{
	return (
	    (event_type == other.event_type) && (timestamp == other.timestamp) && (data == other.data));
}

bool SimulatorEvent::operator!=(SimulatorEvent const& other) const
{
	return !(*this == other);
}

SimulatorEvent::clk_t const SimulatorEvent::timestamp_asap = 0;

} // namespace flange

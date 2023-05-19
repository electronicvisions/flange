#pragma once
#include "RCF/RCF.hpp"
#include "flange/simulator_dpi.h"
#include "flange/simulator_event.h"
#include "hate/visibility.h"
#include <deque>

namespace flange {

/** @brief SimulatorControl is an I/O-queue interface class.
 *
 *  @note All functions use mutexes (or similar means) to
 *  support multi-threaded access to the class.
 */
class SimulatorControl
{
public:
	SimulatorControl() SYMBOL_VISIBLE;

	/// Return true if class is in runnable state
	bool get_runnable() const SYMBOL_VISIBLE;

	/// Set runnable state to target value
	void set_runnable(bool state) SYMBOL_VISIBLE;

	/// Set terminate state
	void issue_terminate() SYMBOL_VISIBLE;

	/// Set reset state for count cycle(s).
	void issue_reset(SimulatorEvent::clk_t count) SYMBOL_VISIBLE;

	/// Add SimulatorEvent to "to_sim" queue
	void push_event(SimulatorEvent const& se) SYMBOL_VISIBLE;

	/// Check for events in "from_sim" queue
	bool received_data_available() const SYMBOL_VISIBLE;

	/// Pop and return first event in "from_sim" queue
	SimulatorEvent pop_front() SYMBOL_VISIBLE;

	/// Pop and return all events in "from_sim" queue
	std::deque<SimulatorEvent> pop_events() SYMBOL_VISIBLE;

	/// Return current system time in "simulator clocks"
	SimulatorEvent::clk_t current_clk() const SYMBOL_VISIBLE;

private:
	/// C-API access functions
	friend dpi_handle_t ::dpi_comm_init();
	friend void ::dpi_comm_shutdown(dpi_handle_t handle);
	friend void ::dpi_comm_tx(dpi_handle_t handle, FlangeFrame* tx_data);
	friend bool ::dpi_comm_rx(
	    dpi_handle_t handle, bool rx_ready, bool* terminate, bool* reset, FlangeFrame* rx_data);

	mutable RCF::Mutex m_service_lock;
	std::deque<SimulatorEvent> m_to_sim;
	std::deque<SimulatorEvent> m_from_sim;
	SimulatorEvent::clk_t m_current_clk;
	bool m_runnable;
	bool m_pause_after_next_event_from_sim;
	bool m_terminate_asap;
	SimulatorEvent::clk_t m_reset;
};

} // namespace flange

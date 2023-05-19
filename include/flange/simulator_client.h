#pragma once
#include "flange/genpybind.h"
#include "flange/simulator_event.h"
#include "hate/visibility.h"
#include <memory>
#include <stdbool.h>
#include <stdint.h>
#include <string>

namespace flange GENPYBIND_TAG_FLANGE {

/**
 * Simulation client connecting via RCF to simulation controlled by SimulatorControl.
 * Establish and hold Simulation connection.
 * Provide convenience functions for sending and receiving packets.
 */
class GENPYBIND(visible) SimulatorClient
{
public:
	typedef uint16_t port_t;
	typedef std::string ip_t;

	typedef SimulatorEvent::al_data_t data_type;

	/**
	 * Create and start connection to simulation server.
	 * @param ip IP-address of simulation server
	 * @param port Port of simulation server
	 */
	SimulatorClient(ip_t ip, port_t port) SYMBOL_VISIBLE;

	/**
	 * Create and start connection to simulation server.
	 * Automatically extract RCF port from environment, the simulation server is expected to run on
	 * the same host.
	 * @throws std::runtime_error On no port to simulator found in environment
	 */
	SimulatorClient() SYMBOL_VISIBLE;

	/**
	 * Copy constructor.
	 */
	SimulatorClient(SimulatorClient const& other) = delete;

	/**
	 * Close connection to simulator.
	 */
	~SimulatorClient() SYMBOL_VISIBLE;

	/**
	 * Send a packet containing a vector of words.
	 * @param words Words to send
	 */
	void send(data_type const& words) SYMBOL_VISIBLE;

	/**
	 * Receive a packet containing a vector of words.
	 * @return Received vector of words
	 * @throws std::runtime_error On timeout of 1s waiting for response
	 */
	data_type receive() SYMBOL_VISIBLE;

	/**
	 * Set runnable state of the simulation.
	 * On true start / continue with execution.
	 * @param value Value to set runnable state to
	 */
	void set_runnable(bool value) SYMBOL_VISIBLE;

	/**
	 * Get runnable state of the simulation.
	 * @return Runnable state
	 */
	bool get_runnable() SYMBOL_VISIBLE;

	/**
	 * Check whether the simulation has event(s) that can be received.
	 * @return Boolean value
	 */
	bool receive_data_available() SYMBOL_VISIBLE;

	/**
	 * Get current time of simulation in clock cycles.
	 * @return Current clock value
	 */
	SimulatorEvent::clk_t get_current_time() SYMBOL_VISIBLE;

	/**
	 * Set RCF remote call timeout in milliseconds.
	 * @param timeout Timeout in milliseconds
	 */
	void set_remote_timeout(int timeout) SYMBOL_VISIBLE;

	/**
	 * Send terminate signal to simulator.
	 */
	void issue_terminate() SYMBOL_VISIBLE;

	/**
	 * Send reset signal to simulator.
	 * @param count Number of clock cycles to pull reset
	 */
	void issue_reset(SimulatorEvent::clk_t count = 1) SYMBOL_VISIBLE;

private:
	struct Impl;
	std::unique_ptr<Impl> m_impl;
};

} // namespace flange

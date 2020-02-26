#pragma once

#include <memory>
#include <stdbool.h>
#include <stdint.h>
#include <string>

#include "flange/genpybind.h"
#include "flange/simulator_event.h"

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

	typedef SimulatorEvent::al_data_t word_type;

	/**
	 * Create and start connection to simulation server.
	 * @param ip IP-address of simulation server
	 * @param port Port of simulation server
	 */
	SimulatorClient(ip_t ip, port_t port);

	/**
	 * Create and start connection to simulation server.
	 * Automatically extract RCF port from environment, the simulation server is expected to run on
	 * the same host.
	 * @throws std::runtime_error On no port to simulator found in environment
	 */
	SimulatorClient();

	/**
	 * Copy constructor.
	 */
	SimulatorClient(SimulatorClient const& other) = delete;

	/**
	 * Close connection to simulator.
	 */
	~SimulatorClient();

	/**
	 * Send a packet containing a single word.
	 * @param word Word to send
	 */
	void send(word_type word);

	/**
	 * Receive a packet containing a single word.
	 * @return Received word
	 * @throws std::runtime_error On timeout of 1s waiting for response
	 */
	word_type receive();

	/**
	 * Set runnable state of the simulation.
	 * On true start / continue with execution.
	 * @param value Value to set runnable state to
	 */
	void set_runnable(bool value);

	/**
	 * Get runnable state of the simulation.
	 * @return Runnable state
	 */
	bool get_runnable();

	/**
	 * Check whether the simulation has event(s) that can be received.
	 * @return Boolean value
	 */
	bool receive_data_available();

	/**
	 * Get current time of simulation in clock cycles.
	 * @return Current clock value
	 */
	SimulatorEvent::clk_t get_current_time();

	/**
	 * Set RCF remote call timeout in milliseconds.
	 * @param timeout Timeout in milliseconds
	 */
	void set_remote_timeout(int timeout);

	/**
	 * Send terminate signal to simulator.
	 */
	void issue_terminate();

	/**
	 * Send reset signal to simulator.
	 * @param count Number of clock cycles to pull reset
	 */
	void issue_reset(SimulatorEvent::clk_t count = 1);

private:
	struct Impl;
	std::unique_ptr<Impl> m_impl;
};

} // namespace flange

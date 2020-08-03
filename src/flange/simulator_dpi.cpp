#include "flange/simulator_dpi.h"

extern "C"
{
#include <unistd.h>
}

#include <cassert>

#include "flange/simulator_control.h"
#include "flange/simulator_control_if.h"

// the C-API uses global variables to track the service and RCF server instances
static std::vector<flange::SimulatorControl*> g_service;
static std::vector<RCF::RcfServer*> g_server;

static constexpr uint32_t rx_runnable_state_sleep_time = 1000000;

// FIXME: support multiple servers
dpi_handle_t dpi_comm_init()
{
	RCF::init();
	if (!g_server.empty() || !g_service.empty()) {
		throw std::runtime_error("Pre-existing services found; this is not supported for now.");
	}

	dpi_handle_t ret = static_cast<dpi_handle_t>(g_server.size());

	// Spawn flange::SimulatorControl
	flange::SimulatorControl* service = new flange::SimulatorControl();
	service->m_current_clk = 0;
	g_service.push_back(service);

	// Spawn matching
	char* env_port = std::getenv("FLANGE_SIMULATION_RCF_PORT");
	uint16_t port = static_cast<uint16_t>(
	    env_port ? std::strtoul(env_port, static_cast<char**>(nullptr), 10)
	             : flange::default_rcf_port);
	if (port == 0) {
		throw std::runtime_error(
		    "Could not convert FLANGE_SIMULATION_RCF_PORT environment variable to a port.");
	}
	RCF::RcfServer* server = new RCF::RcfServer(RCF::TcpEndpoint("0.0.0.0", port));

	server->bind<flange::I_SimulatorControl>(*service);
	server->start();
	g_server.push_back(server);

	// return index of new instance
	return ret;
}

void dpi_comm_shutdown(dpi_handle_t /*handle*/)
{
	while (!g_server.empty()) {
		RCF::RcfServer* server = g_server.back();
		delete server;
		g_server.pop_back();
	}

	while (!g_service.empty()) {
		flange::SimulatorControl* service = g_service.back();
		delete service;
		g_service.pop_back();
	}
	RCF::deinit();
}

void dpi_comm_tx(dpi_handle_t handle, FlangeFrame* data)
{
	RCF::Lock lock(g_service[handle]->m_service_lock);

	assert(data != nullptr);
	uint64_t* elem = data->data;
	assert(elem != nullptr);

	if (data->size > 0) {
		flange::SimulatorEvent se(
		    flange::SimulatorEvent::AL_DATA, g_service[handle]->m_current_clk,
		    {elem, elem + data->size});

		g_service[handle]->m_from_sim.push_back(se);
	}

	// unset runnable if we were instructed to do so after the next event from sim
	bool& tx_pause = g_service[handle]->m_pause_after_next_event_from_sim;
	if (tx_pause) {
		tx_pause = false;
		g_service[handle]->m_runnable = false;
	}
}

bool dpi_comm_rx(
    dpi_handle_t handle, bool rx_ready, bool* terminate, bool* reset, FlangeFrame* rx_data)
{
	*reset = false;
	*terminate = false;
	rx_data->size = 0;
	flange::SimulatorEvent::clk_t now = 0;

	{
		RCF::Lock lock(g_service[handle]->m_service_lock);
		now = g_service[handle]->m_current_clk;
	}

	while (true) {
		{
			RCF::Lock lock(g_service[handle]->m_service_lock);
			if (g_service[handle]->m_runnable) {
				break;
			}
		}

		// wait for runnable state
		usleep(rx_runnable_state_sleep_time);
	}

	RCF::Lock lock(g_service[handle]->m_service_lock);

	// update clock counter in software for next function call
	g_service[handle]->m_current_clk++;

	// special case: TERMINATE ASAP!
	if (g_service[handle]->m_terminate_asap) {
		*terminate = true;
		return false;
	}

	// special case: reset!
	if (g_service[handle]->m_reset > 0) {
		*reset = true;
		g_service[handle]->m_reset -= 1;
		return false;
	}

	// simulator ready to take data?
	if (!rx_ready) {
		return false;
	}

	// event queue empty?
	if (g_service[handle]->m_to_sim.empty()) {
		return false;
	}

	// next event
	flange::SimulatorEvent se = g_service[handle]->m_to_sim.front();

	// next event ready?
	if (se.timestamp > now) {
		return false;
	}

	if ((se.timestamp > flange::SimulatorEvent::timestamp_asap) && (se.timestamp < now)) {
		throw std::runtime_error("Illegal (early) timestamp found");
	}

	bool valid_data = false;
	// PAUSE events will stop the simulation at the next invocation of dpi_comm_rx
	switch (se.event_type) {
		case flange::SimulatorEvent::PAUSE: {
			g_service[handle]->m_runnable = false;
			break;
		}

		case flange::SimulatorEvent::PAUSE_AFTER_RECEIVE: {
			g_service[handle]->m_pause_after_next_event_from_sim = true;
			break;
		}

		case flange::SimulatorEvent::TERMINATE: {
			*terminate = true;
			break;
		}

		case flange::SimulatorEvent::AL_DATA: {
			rx_data->size = 1;
			rx_data->data[0] =
			    (se.event_type == flange::SimulatorEvent::AL_DATA) ? se.data.at(0) : 0;
			valid_data = (se.event_type == flange::SimulatorEvent::AL_DATA);
			break;
		}

		default: {
			throw std::runtime_error("Illegal SimulatorEvent found!");
		}
	}

	// remove event from queue
	g_service[handle]->m_to_sim.pop_front();

	return valid_data;
}

#pragma once

// clang-format off
extern "C"
{
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Dynamic-sized array exchange object between flange and SV.
 * @note This has to match the definition in SystemVerilog.
 */
struct FlangeFrame {
	/// size of the array (valid range from 0...size-1)
	uint64_t size;
	/// data vector
	uint64_t data[1024]; // TODO: should be compile-time configurable, cf. #3638
};

typedef int dpi_handle_t;

/**
 * @brief Simulator startup wants to initialize the software interface service.
 * @return Handle identifying the service and server instance
 */
dpi_handle_t dpi_comm_init();

/**
 * @brief Simulator wants to cleanly terminate.
 * @note called by simulation before $finish
 */
void dpi_comm_shutdown(dpi_handle_t handle);

/**
 * @brief Simulator provides new data word.
 */
void dpi_comm_tx(dpi_handle_t handle, FlangeFrame* tx_data);

/**
 * @brief Simulator requests new data word.
 * @return true on new data stored in rx_data/terminate
 */
bool dpi_comm_rx(dpi_handle_t handle, bool rx_ready, bool* terminate, bool* reset, FlangeFrame* rx_data);

} // extern "C"
// clang-format on

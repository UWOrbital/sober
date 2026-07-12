#pragma once

#include <cstdint>
#include <driver_ifaces/thermal_datatypes.hpp>

class IThermal {
protected:
	IThermal() = default;

public:
    virtual ~IThermal() = default;

	/// @brief Initialize the thermal sensor
	/// @return 0 on success, negative value on failure
    virtual int init() = 0;

	/// @brief Stream frames from the thermal sensor
	/// @return Raw thermal frame batch
    virtual ThermalImageFrame_t streamFrames() = 0;

    virtual Spotmeter_Values_t get_scene_stats(uint16_t *min, uint16_t *max, uint16_t *avg) = 0;

    //timeestamp of the last frame received from the sensor
    virtual uint32_t get_last_frame_timestamp() = 0;

    // set camera mode?

    // get camera mode (radiometry, tlinear, agc, video format)
    virtual Camera_Mode_t get_camera_mode() = 0;

    // get calibration data (emissivity, window transmission, window temp)
    virtual Calibration_t get_calibration() = 0;

    virtual void set_calibration(const Calibration_t &cal) = 0;

};
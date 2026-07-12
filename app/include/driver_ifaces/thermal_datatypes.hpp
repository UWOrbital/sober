#pragma once

#include <cstdint>

// unscaled, NED frame
typedef struct {
    int16_t raw_frame[38340]; // 160x120
    float frame_temps[38340]; // 160x120, converted
    uint32_t timestamp_ms;                            // Time counter (ms since boot), from telemetry
    uint32_t frame_count;                             // Frame sequence number, from telemetry
} ThermalImageFrame_t;

// can be set by user for offset and gain correction? check
typedef struct {
    float emissivity;                              // Scene emissivity (user-settable correction)
    float window_transmission;                     // Window transmission factor (user-settable)
    float window_temp_K;                           // Window temperature in Kelvin (user-settable)
} Calibration_t;

// user configurable camera modes
typedef struct {
    bool radiometry_enabled;                       // Radiometry mode on/off
    bool tlinear_enabled;                          // TLinear mode on/off
    bool agc_enabled;                              // Automatic Gain Control status (affects data interpretation)
    uint8_t video_format;                          // Raw14, RGB888 etc.
} Camera_Mode_t;



// // ROI for spotmeter / AGC (optional)
// uint16_t agc_roi_top;
// uint16_t agc_roi_left;
// uint16_t agc_roi_bottom;
// uint16_t agc_roi_right;

// // Spotmeter values (mean, max, min temperature of ROI)
typedef struct{
    float    spotmeter_mean_temp_K;
    float    spotmeter_max_temp_K;
    float    spotmeter_min_temp_K;
} Spotmeter_Values_t;
#pragma once
// Window Settings
#define V_SYNC 0
#define FULLSCREEN_MODE 0 // If set, window resolution is maximized to your screen resolution


// AA Settings
#define MSAA_SAMPLE_AMOUNT 4 // Only used in forward rendering & for water

// Camera Settings
#define CAMERA_SPEED 20.0f
#define CAMERA_ROTATION_SENSITIVITY 0.15f
#define CAMERA_FOV 60.0f
#define NEAR_PLANE 0.3f
#define FAR_PLANE 5000.0f


// Shadowmap Options
#define SHADOWMAP_RESOLUTION_X 2048
#define SHADOWMAP_RESOLUTION_Y 2048
#define SHADOWMAP_NEAR_PLANE 0.0f
#define SHADOWMAP_FAR_PLANE 250.0f
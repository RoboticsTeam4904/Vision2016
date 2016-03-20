// CONSTANTS
const int maxThresh = 255;
const int maxBlobSize = 20;

// Distances are in milimeters, angles are in degrees
const float mountAngleX = 0.0;
const float mountAngleY = 45.0 * M_PI / 180;
const int nativeResX = 2592;
const int nativeResY = 1944;
const float nativeAngleX = 53.5 * M_PI / 180;
const float nativeAngleY = 41.41 * M_PI / 180;
const float shiftX = 336.55; // 13.25 inches
const float shiftY = 57.15; // 2.5 inches
const float goalHeight = 2292.35; // 7.5 feet
const float cameraHeight = 296.0; // 296 milimeters
const float millimetersPerInch = 25.4;

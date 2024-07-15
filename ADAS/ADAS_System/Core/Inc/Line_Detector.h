/*
 * Line_Detector.h
 *
 *  Created on: May 1, 2024
 *      Author: eqw18
 */

#ifndef INC_LINE_DETECTOR_H_
#define INC_LINE_DETECTOR_H_

#include <stdint.h>

/*
 * Function: LineDetector_u8Read
 * ------------------------------
 * Reads the state of the line detector sensor.
 * Returns 1 if the sensor detects a line, otherwise returns 0.
 */
void LineDetector_voidRead(uint8_t *IR1, uint8_t *IR2);



#endif /* INC_LINE_DETECTOR_H_ */

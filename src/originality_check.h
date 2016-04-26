/*
 * originality_check.h
 *
 *  Created on: 26.04.2014.
 *      Author: d-logic
 */

#ifndef ORIGINALITY_CHECK_H_
#define ORIGINALITY_CHECK_H_

#define APP_LOOP_CYCLE_PERIOD	100000L // [us]

int app_loop(void);
UFR_STATUS NewCardInField(uint8_t sak, uint8_t *uid, uint8_t uid_size);
chr_ptr GetDlTypeName(uint8_t dl_type_code);

#endif /* ORIGINALITY_CHECK_H_ */

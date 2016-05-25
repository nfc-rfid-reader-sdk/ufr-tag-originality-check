/*
 ============================================================================
 Name        : originality_check.c
 Author      : d-logic
 Version     : 1.1
 Description : NXP Originality Check Demo in C, (using gcc cross compiler)
 Dependencies: libuFCoder.dylib from v3.9.14, uFR firmware from v3.9.8
 Windows (MinGW) compiler options: -std=c99
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "uFCoder.h"
#include "originality_check.h"
#include <unistd.h>

//==============================================================================
int main(int argc, char *argv[]) {

	int result = app_loop();

	return result;
}

//==============================================================================
int app_loop(void) {
	UFR_STATUS status;
	bool card_in_field = false;
	uint8_t old_sak = 0, old_uid_size = 0, old_uid[10];
	uint8_t sak, uid_size, uid[10];

	printf( "-----------------------------------------\n"
			"      NXP  Originality Check Example     \n"
			"Please wait while opening uFR NFC reader.\n"
			"-----------------------------------------\n");
	status = ReaderOpen();
	if (status != UFR_OK) {
		printf("Error while opening device, status is: 0x%08X\n", status);
		return EXIT_FAILURE;
	}
	printf("uFR NFC reader successfully opened.\n");

	do {
		status = GetCardIdEx(&sak, uid, &uid_size);
		switch(status)
		{
		case UFR_OK:
			if (card_in_field) {
				if (old_sak != sak || old_uid_size != uid_size ||
						memcmp(old_uid, uid, uid_size)) {
					old_sak = sak;
					old_uid_size = uid_size;
					memcpy(old_uid, uid, uid_size);
					NewCardInField(sak, uid, uid_size);
				}
			} else {
				old_sak = sak;
				old_uid_size = uid_size;
				memcpy(old_uid, uid, uid_size);
				NewCardInField(sak, uid, uid_size);
				card_in_field = true;
			}
			break;
		case UFR_NO_CARD:
			card_in_field = false;
			status = UFR_OK;
			break;
		default:
			printf("Fatal error while trying to read card, status is: 0x%08X\n", status);
			return EXIT_FAILURE;
		}
		usleep(APP_LOOP_CYCLE_PERIOD);
	} while (status == UFR_OK);

	ReaderClose();
	return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------
UFR_STATUS NewCardInField(uint8_t sak, uint8_t *uid, uint8_t uid_size) {
	UFR_STATUS status;
	uint8_t curr_uid_len;
	uint8_t dl_card_type;
	uint8_t ecc_signature[ECC_SIG_LEN];
	uint8_t curr_uid[MAX_UID_LEN];

	status = ReadECCSignature(ecc_signature, curr_uid, &curr_uid_len, &dl_card_type);
	if ((status != UFR_OK) && (status != UFR_UNSUPPORTED_CARD_TYPE))
		return status;

	if (status == UFR_UNSUPPORTED_CARD_TYPE) {
		status = GetDlogicCardType(&dl_card_type);
		if (status != UFR_OK)
			return status;
		printf("\a-----------------------------------------------------\n");
		printf("Card type: %s, uid[%d] = ", GetDlTypeName(dl_card_type), uid_size);
		for (int i = 0; i < uid_size; i++) {
			printf("%02X", uid[i]);
			if (i < (uid_size - 1)) printf(":");
		}
		printf("\n");
		printf("Tag does not support ECC signature checking.\n");
	} else {
		printf("\a-----------------------------------------------------\n");
		printf("Card type: %s, uid[%d] = ", GetDlTypeName(dl_card_type), curr_uid_len);
		for (int i = 0; i < curr_uid_len; i++) {
			printf("%02X", curr_uid[i]);
			if (i < (curr_uid_len - 1)) printf(":");
		}
		printf("\n");
		printf("Card ECC signature = ");
		for (int i = 0; i < ECC_SIG_LEN; i++) {
			printf("%02X", ecc_signature[i]);
			if (i < (ECC_SIG_LEN - 1)) printf(":");
		}
		printf("\n");

		status = OriginalityCheck(ecc_signature, curr_uid, curr_uid_len, dl_card_type);
		if (status != UFR_OK) {
			switch (status) {
			case UFR_NOT_NXP_GENUINE:
				printf("Tag is not genuine NXP product.\n");
				break;
			case UFR_OPEN_SSL_DYNAMIC_LIB_FAILED:
				printf("Error in OpenSSL library. Is libeay32.dll version 1.0.1 or above?\n");
				break;
			case UFR_OPEN_SSL_DYNAMIC_LIB_NOT_FOUND:
				printf("Can't find OpenSSL library. Is libeay32.dll in current folder or in path?\n");
				break;
			default:
				printf("Unknown error while conducting originality checking.\n");
				break;
			}
		} else {
			printf("Tag has passed NXP originality check.\n");
		}
	}

	printf("-----------------------------------------------------\n");
	return UFR_OK;
}

//------------------------------------------------------------------------------
chr_ptr GetDlTypeName(uint8_t dl_type_code) {
	static char s[50];

	switch (dl_type_code) {
	case DL_MIFARE_ULTRALIGHT:
		strcpy(s, "DL_MIFARE_ULTRALIGHT");
		break;
	case DL_MIFARE_ULTRALIGHT_EV1_11:
		strcpy(s, "DL_MIFARE_ULTRALIGHT_EV1_11");
		break;
	case DL_MIFARE_ULTRALIGHT_EV1_21:
		strcpy(s, "DL_MIFARE_ULTRALIGHT_EV1_21");
		break;
	case DL_MIFARE_ULTRALIGHT_C:
		strcpy(s, "DL_MIFARE_ULTRALIGHT_C");
		break;
	case DL_NTAG_203:
		strcpy(s, "DL_NTAG_203");
		break;
	case DL_NTAG_210:
		strcpy(s, "DL_NTAG_210");
		break;
	case DL_NTAG_212:
		strcpy(s, "DL_NTAG_212");
		break;
	case DL_NTAG_213:
		strcpy(s, "DL_NTAG_213");
		break;
	case DL_NTAG_215:
		strcpy(s, "DL_NTAG_215");
		break;
	case DL_NTAG_216:
		strcpy(s, "DL_NTAG_216");
		break;
	case DL_MIKRON_MIK640D:
		strcpy(s, "DL_MIKRON_MIK640D");
		break;
	case DL_MIFARE_MINI:
		strcpy(s, "DL_MIFARE_MINI");
		break;
	case DL_MIFARE_CLASSIC_1K:
		strcpy(s, "DL_MIFARE_CLASSIC_1K");
		break;
	case DL_MIFARE_CLASSIC_4K:
		strcpy(s, "DL_MIFARE_CLASSIC_4K");
		break;
	case DL_MIFARE_PLUS_S_2K:
		strcpy(s, "DL_MIFARE_PLUS_S_2K");
		break;
	case DL_MIFARE_PLUS_S_4K:
		strcpy(s, "DL_MIFARE_PLUS_S_4K");
		break;
	case DL_MIFARE_PLUS_X_2K:
		strcpy(s, "DL_MIFARE_PLUS_X_2K");
		break;
	case DL_MIFARE_PLUS_X_4K:
		strcpy(s, "DL_MIFARE_PLUS_X_4K");
		break;
	case DL_MIFARE_DESFIRE:
		strcpy(s, "DL_MIFARE_DESFIRE");
		break;
	case DL_MIFARE_DESFIRE_EV1_2K:
		strcpy(s, "DL_MIFARE_DESFIRE_EV1_2K");
		break;
	case DL_MIFARE_DESFIRE_EV1_4K:
		strcpy(s, "DL_MIFARE_DESFIRE_EV1_4K");
		break;
	case DL_MIFARE_DESFIRE_EV1_8K:
		strcpy(s, "DL_MIFARE_DESFIRE_EV1_8K");
		break;
	case DL_IMEI_UID:
		strcpy(s, "DL_IMEI_UID");
		break;
	default:
		strcpy(s, "UNSUPPORTED CARD");
	}

	return s;
}

//==============================================================================

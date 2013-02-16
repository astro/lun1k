#include "main.h"


#include "hal_file.h"

static void init(void) {
	

	lun1k_fopen();
}

static void deinit(void) {
	lun1k_fclose();
}

static uint8_t tick(void) {
	lun1k_fread();
	
	return 0;
}

static void constructor(void) CONSTRUCTOR_ATTRIBUTES
    void constructor(void) {
        registerAnimation("file_example", init,tick,deinit, 10, 2000);
}

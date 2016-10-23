#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "structures.h"
#include "recordData.h"

int main( void ){
	//	create toilet object
	toilet_t toilet;

	//	packet toilet with dummy data to test
	toilet.modules.refractometer->data.currentSlotTemperature = 23.103;
	toilet.modules.refractometer->data.heater = 0x01;
	toilet.modules.refractometer->data.light = 0x01;
	toilet.modules.refractometer->data.pwm = 0x55;
	toilet.modules.refractometer->data.setPointTemperature = 23.5;
	toilet.modules.refractometer->data.specificGravity = 10.003;

	//	record the data
	/* If DISPLAY_DATA is defined in globals.h the data will print to screen */
	RecordData( &toilet );

	puts( "\nPress <enter> to quit." );
	getchar();

	return 0;
}

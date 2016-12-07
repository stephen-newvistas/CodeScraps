#define _XOPEN_SOURCE 1 /* for ptsname */
#define _GNU_SOURCE   1 /* "" */




int main(int argc, char **argv){

    /*  setup  */
    setup();

	/* main loop */
	while (1) {

        loop();

	} /* while(1) */

	return 0;
}

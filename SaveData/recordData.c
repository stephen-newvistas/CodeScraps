#include "recordData.h"

static bool read_bytes(void *data, size_t sz, FILE *fh) {
    return fread(data, sizeof(uint8_t), sz, fh) == (sz * sizeof(uint8_t));
}

static bool file_reader(cmp_ctx_t *ctx, void *data, size_t limit) {
    return read_bytes(data, limit, (FILE *)ctx->buf);
}

static size_t file_writer(cmp_ctx_t *ctx, const void *data, size_t count) {
    return fwrite(data, sizeof(uint8_t), count, (FILE *)ctx->buf);
}

void error_and_exit(const char *msg) {
    fprintf(stderr, "%s\n\n", msg);
    exit(EXIT_FAILURE);
}

static time_t *GetTime(){
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	return &rawtime;
}

int RecordRefractometer( toilet_t *_toilet ){

	FILE *fh;
	cmp_ctx_t cmp;

	//	open the file for the refractometer
    fh = fopen("/opt/SpectrAuto/Data/refractometer.dat", "w+b");

    if (fh == NULL)
        error_and_exit("Error opening data.dat");

    cmp_init(&cmp, fh, file_reader, file_writer);

	if (!cmp_write_str(&cmp, "recordedTimestamp", 17))
		error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_u32(&cmp, *GetTime() ))
        error_and_exit(cmp_strerror(&cmp));

	if (!cmp_write_str(&cmp, "currentSlotTemp", 15))
		error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_float(&cmp, _toilet->modules.refractometer->data.currentSlotTemperature ))
        error_and_exit(cmp_strerror(&cmp));

	if (!cmp_write_str(&cmp, "setPointTemp", 12))
		error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_float(&cmp, _toilet->modules.refractometer->data.setPointTemperature ))
        error_and_exit(cmp_strerror(&cmp));

	if (!cmp_write_str(&cmp, "specificGravity", 15))
		error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_float(&cmp, _toilet->modules.refractometer->data.specificGravity ))
        error_and_exit(cmp_strerror(&cmp));

	if (!cmp_write_str(&cmp, "pwm", 3))
		error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_u8(&cmp, _toilet->modules.refractometer->data.pwm ))
        error_and_exit(cmp_strerror(&cmp));

	if (!cmp_write_str(&cmp, "lightSource", 11))
		error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_u8(&cmp, _toilet->modules.refractometer->data.light ))
        error_and_exit(cmp_strerror(&cmp));

	if (!cmp_write_str(&cmp, "heater", 6))
		error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_u8(&cmp, _toilet->modules.refractometer->data.heater ))
        error_and_exit(cmp_strerror(&cmp));


	ReadData( fh , &cmp );

	fclose(fh);

	return 0;

}

int ReadData( FILE *fh , cmp_ctx_t *cmp ){

    cmp_init(cmp, fh, file_reader, file_writer);


	rewind( fh );

    uint16_t year = 1983;
    uint8_t month = 5;
    uint8_t day = 28;
    int64_t sint = 0;
    uint64_t uint = 0;
    float flt = 0.0f;
    double dbl = 0.0;
    bool boolean = false;
    uint8_t fake_bool = 0;
    uint32_t string_size = 0;
    uint32_t array_size = 0;
    uint32_t binary_size = 0;
    uint32_t map_size = 0;
    int8_t ext_type = 0;
    uint32_t ext_size = 0;
    char sbuf[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


    /* Alternately, you can read objects until the stream is empty */
    while (1) {
        cmp_object_t obj;

        if (!cmp_read_object(cmp, &obj)) {
            if (feof(fh))
                break;

            error_and_exit(cmp_strerror(cmp));
        }

        switch (obj.type) {
            case CMP_TYPE_POSITIVE_FIXNUM:
            case CMP_TYPE_UINT8:
                printf("Unsigned Integer: %u\n", obj.as.u8);
                break;
            case CMP_TYPE_FIXMAP:
            case CMP_TYPE_MAP16:
            case CMP_TYPE_MAP32:
                printf("Map: %u\n", obj.as.map_size);
                break;
            case CMP_TYPE_FIXARRAY:
            case CMP_TYPE_ARRAY16:
            case CMP_TYPE_ARRAY32:
                printf("Array: %u\n", obj.as.array_size);
                break;
            case CMP_TYPE_FIXSTR:
            case CMP_TYPE_STR8:
            case CMP_TYPE_STR16:
            case CMP_TYPE_STR32:
                if (!read_bytes(sbuf, obj.as.str_size, fh))
                    error_and_exit(strerror(errno));
                sbuf[obj.as.str_size] = 0;
                printf("String: %s\n", sbuf);
                break;
            case CMP_TYPE_BIN8:
            case CMP_TYPE_BIN16:
            case CMP_TYPE_BIN32:
                memset(sbuf, 0, sizeof(sbuf));
                if (!read_bytes(sbuf, obj.as.bin_size, fh))
                    error_and_exit(strerror(errno));
                printf("Binary: %s\n", sbuf);
                break;
            case CMP_TYPE_NIL:
                printf("NULL\n");
                break;
            case CMP_TYPE_BOOLEAN:
                if (obj.as.boolean)
                    printf("Boolean: true\n");
                else
                    printf("Boolean: false\n");
                break;
            case CMP_TYPE_EXT8:
            case CMP_TYPE_EXT16:
            case CMP_TYPE_EXT32:
            case CMP_TYPE_FIXEXT1:
            case CMP_TYPE_FIXEXT2:
            case CMP_TYPE_FIXEXT4:
            case CMP_TYPE_FIXEXT8:
            case CMP_TYPE_FIXEXT16:
                if (obj.as.ext.type == 1) { /* Date object */
                    if (!read_bytes(&year, sizeof(uint16_t), fh))
                        error_and_exit(strerror(errno));

                    if (!read_bytes(&month, sizeof(uint8_t), fh))
                        error_and_exit(strerror(errno));

                    if (!read_bytes(&day, sizeof(uint8_t), fh))
                        error_and_exit(strerror(errno));

                    printf("Date: %u/%u/%u\n", year, month, day);
                }
                else {
                    printf("Extended type {%d, %u}: ",
                        obj.as.ext.type, obj.as.ext.size
                    );
                    while (obj.as.ext.size--) {
                        read_bytes(sbuf, sizeof(uint8_t), fh);
                        printf("%02x ", sbuf[0]);
                    }
                    printf("\n");
                }
                break;
            case CMP_TYPE_FLOAT:
                printf("Float: %f\n", obj.as.flt);
                break;
            case CMP_TYPE_DOUBLE:
                printf("Double: %f\n", obj.as.dbl);
                break;
            case CMP_TYPE_UINT16:
                printf("Unsigned Integer: %u\n", obj.as.u16);
                break;
            case CMP_TYPE_UINT32:
                printf("Unsigned Integer: %u\n", obj.as.u32);
                break;
            case CMP_TYPE_UINT64:
                printf("Unsigned Integer: %" PRIu64 "\n", obj.as.u64);
                break;
            case CMP_TYPE_NEGATIVE_FIXNUM:
            case CMP_TYPE_SINT8:
                printf("Signed Integer: %d\n", obj.as.s8);
                break;
            case CMP_TYPE_SINT16:
                printf("Signed Integer: %d\n", obj.as.s16);
                break;
            case CMP_TYPE_SINT32:
                printf("Signed Integer: %d\n", obj.as.s32);
                break;
            case CMP_TYPE_SINT64:
                printf("Signed Integer: %" PRId64 "\n", obj.as.s64);
                break;
            default:
                printf("Unrecognized object type %u\n", obj.type);
                break;
        }
    }

	return 0;
}

int RecordData( toilet_t *_toilet ){
	int ret = 0;
	ret += RecordRefractometer( _toilet );
	//	ret += RecordMox( _toilet );
	//	etc
	return ret;
}




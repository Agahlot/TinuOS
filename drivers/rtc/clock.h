#include <boot.h>

typedef struct {
	unsigned char second;
	unsigned char minute;
	unsigned char hour;
	unsigned char day_of_week;
	unsigned char day_of_month;
	unsigned char month;
	unsigned char year;
	unsigned char century;
} time_t;

extern void gettime(time_t *time);
extern void rtc_install(void);
extern void rtc_print_struct(time_t current_time);
extern time_t rtc_handler(void);
extern u32 time(void);
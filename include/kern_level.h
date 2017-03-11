#ifndef __KERN_LEVEL_H
#define __KERN_LEVEL_H

typedef enum {
		LOGLEVEL_DEFAULT=1,
		LOGLEVEL_EMERG,
		LOGLEVEL_ALERT,
		LOGLEVEL_ERR,
		LOGLEVEL_WARINIG,
		LOGLEVEL_NOTICE,
		LOGLEVEL_DEBUG,
} log_level;

log_level debug_level;
void debug_print(char *title, int line_no, log_level level, char *fmt, ...);


#define printk(level, ...)	debug_print(char *title, int line_no, log_level level, char *fmt, ...)

#endif /* __KERN_LEVEL_H */
#ifndef ERR_H_
#define ERR_H_

void error_strings(char *, ...);
void error_strings_exit(char *, ...);
void func_error(char *);
void func_file_error(const char *, const char *);
void func_file_error_exit(const char *, const char *);
void func_error_exit(char *);
void file_error(char *);
void file_error_exit(char *);
void name_errors(const char *, ...);
void name_errors_exit(char *, ...);
void name_error_exit(void);

#endif

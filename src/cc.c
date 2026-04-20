#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>

#include <jd297/lmap.h>
#include <jd297/vector.h>

#include "toolchain.h"
#include "cpp.h"
#include "comp.h"
#include "gen.h"

#define CC_TEMPLATE_PATH "%s/%s-XXXXXXXXXX%s"

const char *tmp_path = "/tmp";

char *cc_gentemplate(const char *tmp_path, const char *path, const char *suffix)
{
	char *file_name = strdup(path);

	if (file_name == NULL) {
		return NULL;
	}

	char *file_suffix = strrchr(file_name, '.');
	
	if (file_suffix != NULL) {
		*file_suffix = '\0';
	}
	
	char *file_basename = strrchr(file_name, '/');
	
	if (file_basename != NULL) {
		++file_basename;
	}

	char *template = malloc(sizeof(char) * PATH_MAX);

	if (template == NULL) {
		return NULL;
	}

	snprintf(template, PATH_MAX, CC_TEMPLATE_PATH, tmp_path, file_basename, suffix);

	free(file_name);

	return template;
}

FILE *cc_tmpfile(char*template)
{
	int fd;
	FILE *fp = NULL;
	char *suffix = strrchr(template, '.');

	int suffixlen = 0;
	
	if (suffix != NULL) {
		suffixlen = (int)strlen(suffix);
	}

	if ((fd = mkstemps(template, suffixlen)) == -1 || (fp = fdopen(fd, "w+")) == NULL) {
		if (fd != -1) {
			unlink(template);
			close(fd);
		}
	}
	
	return fp;
}

pid_t cc_fork_execvp(vector_t *args)
{
	pid_t pid;

	if ((pid = fork()) == -1) {
		err(EXIT_FAILURE, NULL);
	}

	if (pid == 0) {
		if (execvp(*vec_begin(args), (char **)vec_begin(args)) == -1) {
			err(EXIT_FAILURE, "%s", (char *)*vec_begin(args));
		}
	}
	
	return pid;
}

void print_usage(void)
{
	fprintf(stderr, "usage: cc [-c][-C][-e epsym] [-D name[=value]]... [-E][-f][-F][-g]...\n");
	fprintf(stderr, "          [-i directory]... [-L directory]... [-o outfile][-O][-p][-P]\n");
	fprintf(stderr, "          [-q][-r][-s][-S][-u symname]... [-U name]... [-W options]... operand...\n");
}

int cflag;
int Cflag;
char *epsym;
lmap_t defines;
int Eflag;
int fflag;
int Fflag;
int gflag;
vector_t include_dirs;
vector_t lib_dirs;
char *outfile;
char *optlevel;
int pflag;
int Pflag;
int qflag;
int rflag;
int sflag;
int Sflag;
lmap_t undefsyms;
lmap_t undefines;
vector_t phaseoptions;
vector_t operands;

void free_globals(void)
{
    vec_free(&include_dirs);
    vec_free(&lib_dirs);
    vec_free(&phaseoptions);
    lmap_free(&defines);
    lmap_free(&undefsyms);
    lmap_free(&undefines);
}

int validate_target_extension(char *input_file, const char *target_extension)
{
	char *extension = strrchr(input_file, '.');

	if (extension == NULL) {
		warnx("warning: %s: 'linker' input unused [-Wunused-command-line-argument]", input_file);
		return -1;
	}

	if (strcmp(target_extension, extension) == 0) {
		if (strcmp(".i", extension) == 0) {
			warnx("warning: %s: previously preprocessed input [-Wunused-command-line-argument]", input_file);
			return -1;
		}

		if (strcmp(".s", extension) == 0) {
			warnx("warning: %s: 'assembler' input unused [-Wunused-command-line-argument]", input_file);
			return -1;
		}

		warnx("warning: %s: 'linker' input unused [-Wunused-command-line-argument]", input_file);
		return -1;
	}

	if (strcmp(".i", target_extension) == 0 && strcmp(".c", extension) != 0) {
		if (strcmp(".s", extension) == 0) {
			warnx("warning: %s: 'assembler' input unused [-Wunused-command-line-argument]", input_file);
		} else {
			warnx("warning: %s: 'linker' input unused [-Wunused-command-line-argument]", input_file);
		}

		return -1;
	}

	if (strcmp(".s", target_extension) == 0 && strcmp(".c", extension) != 0 && strcmp(".i", extension) != 0) {
		warnx("warning: %s: 'linker' input unused [-Wunused-command-line-argument]", input_file);

		return -1;
	}

	struct stat sb;

	if (stat(input_file, &sb) == -1) {
		warnx("error: %s: '%s'", strerror(errno), input_file);
		return -1;
	}

	if ((sb.st_mode & S_IFMT) == S_IFDIR) {
		warnx("error: %s: '%s'", strerror(EISDIR), input_file);
		return -1;
	}

	if (strcmp(".c", extension) != 0 &&
	    strcmp(".i", extension) != 0 &&
	    strcmp(".a", extension) != 0 &&
	    strcmp(".so", extension) != 0 &&
	    strcmp(".o", extension) != 0
	) {
		warnx("warning: %s: 'linker' input unused [-Wunused-command-line-argument]", input_file);
		return -1;
	}

	return 0;
}

char *produce_target_extension(char *input_file, const char *target_extension)
{
	char *extension = strrchr(input_file, '.');

	FILE *preprocessor_output = NULL;

	if (strcmp(".c", extension) == 0) {
		char *preprocessor_template = cc_gentemplate(tmp_path, input_file, ".i");

		if ((preprocessor_output = cc_tmpfile(preprocessor_template)) == NULL) {
			warn(NULL);
			return NULL;
		}

		//unlink(preprocessor_template); // TODO add and free / unlink later
		//free(preprocessor_template);

		vector_t source_files = { 0 };
		vec_push_back(&source_files, input_file);
	    
	    Preprocessor_C preprocessor = {
			.include_dirs = &include_dirs,
			.source_files = &source_files,
			.defines = &defines,
			.output = preprocessor_output
		};

		if (preprocessor_c_run(&preprocessor) == -1) { // TODO free on errro source_f
	        return NULL;
	    }
	    
	    vec_free(&source_files);
	}

	char *compiler_outfile = NULL;

	if (strcmp(".i", extension) == 0 || preprocessor_output != NULL) {
		FILE *compiler_output = NULL;
		
		if (outfile != NULL) {
			compiler_outfile = outfile;
		} else if (strcmp(".s", target_extension) == 0) {
			compiler_outfile = strdup(input_file);
			
			char *compiler_outfile_extension = strrchr(compiler_outfile, '.') + 1;
			
			*compiler_outfile_extension = 's';
			
			compiler_output = fopen(compiler_outfile, "w+");
		} else {
			compiler_outfile = cc_gentemplate(tmp_path, input_file, ".s");
			
			compiler_output = cc_tmpfile(compiler_outfile);
		}

		if (compiler_output == NULL) {
			warn(NULL);
			return NULL;
		}

		if (preprocessor_output == NULL) {
			if ((preprocessor_output = fopen(input_file, "r")) == NULL) {
				warn(NULL);
				return NULL;
			}
		}

		Compiler_C compiler = {
	    	.input = preprocessor_output,
			.output = compiler_output
		};

		if (compiler_c_run(&compiler) == -1) {
			return NULL;
		}

		fclose(compiler_output);

		if (strcmp(".s", target_extension) == 0) {
			return compiler_outfile;
		}

		//unlink(compiler_outfile);
		//free(compiler_outfile); // TODO push to temp_files_vector and free / unlink all on exit
	}
	
	char *assemler_outfile = NULL;
	
	if (strcmp(".s", extension) == 0 || compiler_outfile != NULL) {
		if (outfile != NULL) {
			assemler_outfile = outfile;
		} else if (strcmp(".o", target_extension) == 0) {
			assemler_outfile = strdup(input_file); // TODO free // TODO basename
			
			char *assembler_outfile_extension = strrchr(assemler_outfile, '.') + 1;
			
			*assembler_outfile_extension = 'o';
		} else {
			assemler_outfile = cc_gentemplate(tmp_path, input_file, ".o");

			fclose(cc_tmpfile(assemler_outfile));
		}

		vector_t as_args = { 0 };
		vec_push_back(&as_args, "as");
		vec_push_back(&as_args, compiler_outfile);
		vec_push_back(&as_args, "-o");
		vec_push_back(&as_args, assemler_outfile);
		vec_push_back(&as_args, NULL);

		int status;

		// TODO check code and on error ret NULL
		if (waitpid(cc_fork_execvp(&as_args), &status, 0) == -1) {
			warn(NULL);
			status = 1;
		}

		vec_free(&as_args);
		
		if (status != 0) {
			return NULL;
		}
		
		return assemler_outfile;
	}
	
	if (assemler_outfile != NULL) {
		return assemler_outfile;
	}
	
	return input_file;
}

int main(int argc, char **argv)
{
	atexit(free_globals);

	int opt;

	while ((opt = getopt(argc, argv, "cCe:D:EfFgI:L:o:OpPqrsSu:U:W:")) != -1) {
		switch (opt) {
			case 'c': cflag = 1; break;
			case 'C': Cflag = 1; break;
			case 'e': epsym = optarg; break;
			case 'D': {
			    char *value = optarg;

			    while (*value != '\0') {
			        if (*value == '=') {
			            *value = '\0';
			            ++value;
			            break;
			        }

			        ++value;
			    }

			    lmap_add(&defines, optarg, (*value == '\0') ? "1" : value);
			} break;
			case 'E': Eflag = 1; break;
			case 'f': fflag = 1; break;
			case 'F': Fflag = 1; break;
			case 'g': gflag = 1; break;
			case 'i': vec_push_back(&include_dirs, optarg); break;
			case 'L': vec_push_back(&lib_dirs, optarg); break;
			case 'o': outfile = optarg; break;
			case 'O': optlevel = optarg; break;
			case 'p': pflag = 1; break;
			case 'P': Pflag = 1; break;
			case 'q': qflag = 1; break;
			case 'r': rflag = 1; break;
			case 's': sflag = 1; break;
			case 'S': Sflag = 1; break;
			case 'u': lmap_add(&undefsyms, optarg, NULL); break;
			case 'U': lmap_add(&undefines, optarg, NULL); break;
			case 'W': vec_push_back(&phaseoptions, optarg); break;
			default:
				print_usage();
				exit(EXIT_FAILURE);
		}
	}

	if (optind == argc) {
		errx(EXIT_FAILURE, "error: no input file");
	}

	if (optind + 1 != argc && outfile != NULL && (Eflag == 1 || Sflag == 1 || cflag == 1)) {
		errx(EXIT_FAILURE, "error: cannot specify -o when generating multiple output files");
	}

	// usual places:
	vec_push_back(&include_dirs, "/usr/local/include");
	vec_push_back(&include_dirs, "/usr/include");

	vec_push_back(&lib_dirs, "/usr/local/lib");
	vec_push_back(&lib_dirs, "/usr/lib");
	vec_push_back(&lib_dirs, "/lib");

    if (toolchain_create_lookups() == -1) {
        return EXIT_FAILURE;
    }

    // TODO atexit(toolchain_destroy_lookups);

    Toolchain toolchain;

	ToolchainArgs toolchain_args = {
		.lib_dirs = &lib_dirs,
		.outfile = outfile
	};

	if (toolchain_create(&toolchain, &toolchain_args) == -1) {
		return EXIT_FAILURE;
	}

	// TODO atexit(toolchain_destroy);

    if (Eflag == 1) {
    	FILE *output = stdout;

    	if (outfile != NULL) {
			output = fopen(outfile, "w+");
		}

    	if (output == NULL) {
			err(EXIT_FAILURE, NULL);
		}
		
		vector_t source_files = { 0 };
		
		for (int i = optind; i < argc; i++) {
			char *input_file = argv[i];

			if (validate_target_extension(input_file, ".i") == -1) {
				continue;
			}
			
			vec_push_back(&source_files, input_file);
		}

    	Preprocessor_C preprocessor = {
			.include_dirs = &include_dirs,
			.source_files = &source_files,
			.defines = &defines,
			.output = output
		};

		if (preprocessor_c_run(&preprocessor) == -1) { // TODO free on error source_f
	        exit(EXIT_FAILURE);
	    }
	    
	    vec_free(&source_files);
	    
	    exit(EXIT_SUCCESS);
	}

	if (Sflag == 1) {
		int code = EXIT_SUCCESS;
	
		for (int i = optind; i < argc; i++) {
			char *input_file = argv[i];

			if (validate_target_extension(input_file, ".s") == -1) {
				continue;
			}

			if (produce_target_extension(input_file, ".s") == NULL) {
				exit(EXIT_FAILURE);
			}
		}
		
		return code;
	}
	
	if (cflag == 1) {
		int code = EXIT_SUCCESS;
	
		for (int i = optind; i < argc; i++) {
			char *input_file = argv[i];

			if (validate_target_extension(input_file, ".o") == -1) {
				continue;
			}

			if (produce_target_extension(input_file, ".o") == NULL) {
				exit(EXIT_FAILURE);
			}
		}
		
		return code;
	}

	outfile = NULL; // TODO GLOVAL VAR HACK RESOLVE

	for (int i = optind; i < argc; i++) {
		char *input_file = argv[i];

		if (validate_target_extension(input_file, ".o") == -1) {
			continue;
		}

		char *linker_file;

		if ((linker_file = produce_target_extension(input_file, ".x")) == NULL) { // TODO HACK .x
			exit(EXIT_FAILURE);
		}

		// TODO maybe: toolchain_add_input_file(&toolchain, linker_file);
		// or
		// TODO maybe: toolchain_add_linker_input_file(&toolchain, linker_file);
		vec_push_back(&toolchain.input_files, linker_file);
	}
	
	toolchain.linker_setup_args_func(&toolchain);

	int wstatus;

	if (waitpid(cc_fork_execvp(&toolchain.ld_args), &wstatus, 0) == -1) {
		err(EXIT_FAILURE, NULL);
	}

	return WEXITSTATUS(wstatus);
}

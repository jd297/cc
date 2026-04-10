#include "toolchain.h"

void toolchain_linker_openbsd_setup_args(Toolchain *toolchain)
{
	vec_push_back(&toolchain->ld_args, "ld");

	// TODO only shard linking is supported
	vec_push_back(&toolchain->ld_args, "-dynamic-linker=/usr/libexec/ld.so");

	vec_push_back(&toolchain->ld_args, "-o");
	vec_push_back(&toolchain->ld_args, toolchain->args->outfile);

	// TODO check for flags with nostdlib etc.
	vec_push_back(&toolchain->ld_args, "/usr/lib/crt0.o");
	vec_push_back(&toolchain->ld_args, "/usr/lib/crtbegin.o");

	for (char **it = (char **)vec_begin(&toolchain->input_files); it < (char **)vec_end(&toolchain->input_files); it++) {
		vec_push_back(&toolchain->ld_args, *it);
	}

	for (char **it = (char **)vec_begin(toolchain->args->lib_dirs); it < (char **)vec_end(toolchain->args->lib_dirs); it++) {
		vec_push_back(&toolchain->ld_args, "-L");
		vec_push_back(&toolchain->ld_args, *it);
	}

	// TODO other libs ??
	vec_push_back(&toolchain->ld_args, "-lc");

	vec_push_back(&toolchain->ld_args, "/usr/lib/crtend.o");
	vec_push_back(&toolchain->ld_args, NULL);
}

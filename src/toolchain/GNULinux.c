#include "toolchain.h"

void toolchain_linker_gnu_linux_setup_args(Toolchain *toolchain)
{
	vec_push_back(&toolchain->ld_args, "ld");

	// TODO only shard linking is supported
	vec_push_back(&toolchain->ld_args, "-dynamic-linker=/lib64/ld-linux-x86-64.so.2");

	vec_push_back(&toolchain->ld_args, "-o");
	vec_push_back(&toolchain->ld_args, toolchain->args->outfile);

	// TODO check for flags with nostdlib etc.
	vec_push_back(&toolchain->ld_args, "/usr/lib/x86_64-linux-gnu/crti.o");     // TODO HARD PATH to
	vec_push_back(&toolchain->ld_args, "/usr/lib/x86_64-linux-gnu/Scrt1.o");    // TODO HARD PATH to

	for (char **it = (char **)vec_begin(&toolchain->input_files); it < (char **)vec_end(&toolchain->input_files); it++) {
		vec_push_back(&toolchain->ld_args, *it);
	}

	for (char **it = (char **)vec_begin(toolchain->args->lib_dirs); it < (char **)vec_end(toolchain->args->lib_dirs); it++) {
		vec_push_back(&toolchain->ld_args, "-L");
		vec_push_back(&toolchain->ld_args, *it);
	}

	// TODO other libs ??
	vec_push_back(&toolchain->ld_args, "-lc");

	vec_push_back(&toolchain->ld_args, "/usr/lib/x86_64-linux-gnu/crtn.o"); // TODO HARD PATH to
	vec_push_back(&toolchain->ld_args, NULL);
}

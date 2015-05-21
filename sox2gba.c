/* sox2gba.c
 *
 * Copyright (C) 2015 - Blake Lowe
 *
 * This file is part of sox2gba.
 *
 * sox2gba is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * sox2gba is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sox2gba. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include <unistd.h>

#define NAME_LEN 50

int main(int argc, char **argv) {
	FILE *header = NULL; // header file to write, NULL for none
	char name[NAME_LEN+1] = "output"; // default object name
	int freq = 16000; // frequency of output audio, default is 16000Hz

	int option;
	while ((option = getopt(argc, argv, "h:n:f:")) != -1) {
		switch (option) {
		case 'h': // header file path
			header = fopen(optarg, "w");
			break;

		case 'n': // object name
			strncpy(name, optarg, NAME_LEN);
			name[NAME_LEN] = '\0'; // ensure null-termination
			break;

		case 'f': // audio frequency
			freq = atoi(optarg);
			break;

		default: /* '?' */
			printf("Usage: %s [-h headerfile] [-n objname] "
					"[-f freq] inputfile outputfile\n",
					argv[0]);
			return 1;
		}
	}

	// quick and very dirty string build, fix this later
	/* Explanation of sox options:
	 * -S: show progress during conversion
	 * -V3: print verbose info
	 * -t raw: don't output description header, as in wav or au
	 * -r freq: encode to requested frequency
	 * NOTE: gba audio requires signed 8-bit PCM format
	 * -e signed: use signed integers
	 * -b 8: use 8-bit integers
	 * -c 1: downmix to mono; maybe change this later
	 */
	char command[1024];
	sprintf(command, "sox '%s' -S -V3 -t raw -r %i -e signed -b 8 -c 1 -",
			argv[optind], freq);

	// start sox and open our output file
	FILE *sox = popen(command, "r");
	FILE *out = fopen(argv[optind+1], "w");

	/* Why don't we use the WaveData declaration from <gba_sound.h>?
	 *
	 * That declaration uses "s8 data[1];" for the data parameter of
	 * the sound. This is an archaic technique for creating a variable-
	 * length structure, where the size for the structure itself is
	 * allocated to the actual required size. The declaration that appears
	 * here is allocated the same way, but the omission of the array size
	 * is a C99 standards-conforming way of achieving the same thing.
	 *
	 * Doing it this way is not only standard-conforming, but also makes
	 * it much easier to write out the entire sound in a const literal
	 * in a single pass. With the "s8 data[1];" declaration, compilers
	 * are not likely to compile the structure defined in the output file.
	 * The gcc that ships with devkitpro, for example, (correctly) determines
	 * that all bytes past the first one in .data are in excess of the
	 * declared size of the array (1), and discards them. With our
	 * declaration, gcc understands that the structure is intended to
	 * be variable-length, and correctly puts all the bytes in the structure.
	 *
	 * This declaration of WaveData is 100% compatible with the one from
	 * <gba_sound.h>, and the header file created, if any, does in fact
	 * declare the created object as though it were a WaveData as that
	 * file defines it.
	 *
	 */
	fprintf(out, "#include <gba_base.h>\n"
				 "\n"
				 "typedef struct {\n"
				 	 "\tu16 type;\n"
				 	 "\tu16 stat;\n"
				 	 "\tu32 freq;\n"
				 	 "\tu32 loop;\n"
				 	 "\tu32 size;\n"
				 	 "\ts8 data[];\n"
				 "} ALIGN(4) WaveData;\n"
				 "\n"
				 "const WaveData %s = {\n"
				 "\t.type = 0,\n"
				 "\t.stat = 0,\n"
				 "\t.freq = %i,\n"
				 "\t.loop = 0,\n"
				 "\t.data = {\n\t\t",
				 name, freq
			);

	// now we read the output of sox and write it to our file
	// it's been running for some time now, so we should have
	// bytes to read
	uint32_t samples = 0;
	int printed = 0;
	int c;
	while ((c = fgetc(sox)) != EOF) {
		fprintf(out, "0x%X, ",c);
		++samples;
		if (++printed == 10) {
			fprintf(out, "\n\t\t");
			printed = 0;
		}
	}

	fprintf(out, "0x00},\n"
				 "\t.size = %" PRIu32 "\n"
				 "};\n",
				 samples
			);

	fclose(out);

	if (header) {
		// write header file if one was requested, then close it
		fprintf(header,
				"#ifndef %s_H\n"
				"#define %s_H\n"
				"\n"
				"#include <gba_sound.h>\n"
				"\n"
				"extern const WaveData %s;\n"
				"\n"
				"#endif",
				name, name, name
				);
		fclose(header);
	}

	pclose(sox);
}

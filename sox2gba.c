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
	FILE *header = NULL;
	char name[NAME_LEN+1] = "output";
	int freq = 16000;

	int option;
	while ((option = getopt(argc, argv, "h:n:f:")) != -1) {
		switch (option) {
		case 'h':
			header = fopen(optarg, "w");
			break;

		case 'n':
			strncpy(name, optarg, NAME_LEN);
			name[NAME_LEN] = '\0'; // ensure null-termination
			break;

		case 'f':
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
	char command[1024];
	sprintf(command, "sox '%s' -t raw -r %i -e signed -b 8 -c 1 -",
			argv[optind], freq);

	FILE *sox = popen(command, "r");
	FILE *out = fopen(argv[optind+1], "w");

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

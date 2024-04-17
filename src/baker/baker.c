#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

char *outdir = NULL;
int font_ptsize = -1;

void open_fonts(char *argv[], int start, int end)
{
	if (outdir == NULL)
		perror("no output directory\n"),  exit(EXIT_FAILURE);
	if (start == -1 || end == -1)
		perror("no fonts provided\n"), exit(EXIT_FAILURE);
	if (font_ptsize <= 0)
		perror("font point size is invalid\n"), exit(EXIT_FAILURE);
	for (int i = start; i < end; i++)
	{
		TTF_Font *font = TTF_OpenFont(argv[i], font_ptsize);
		char fontname[100], filename[PATH_MAX];

		int slash = strlen(argv[i]) - 1, dot;

		while (slash > 0 && argv[i][slash] != '/')
			slash--;
		dot = ++slash;

		while (argv[i][dot] && argv[i][dot] != '.')
			dot++;
		strncpy(fontname, argv[i] + slash, dot - slash);
		fontname[dot - slash] = '\0';

		for (char c = 1; c > 0; c++)
		{
			char character[2] = {c, '\0'};
			static const SDL_Color WHITE = { 255, 255, 255, 255 };

			SDL_Surface* character_surface =
				TTF_RenderText_Solid(font, character, WHITE);

			sprintf(filename, "%s/fonts/%s/%d.bmp", outdir, fontname, (int)c);

			// printf("Saving char %c to file %s\n", c, filename);

			SDL_SaveBMP(character_surface, filename);

			SDL_FreeSurface(character_surface);
		}

		TTF_CloseFont(font);
	}
}

void open_textures(char *argv[], int start, int end)
{
	if (outdir == NULL)
		perror("no output directory\n"), exit(EXIT_FAILURE);
	if (start == -1 || end == -1)
		perror("no textures provided\n"), exit(EXIT_FAILURE);
	for (int i = start; i < end; i++)
	{
		SDL_Surface *tex = IMG_Load(argv[i]);
		char texname[100], filename[PATH_MAX];
		int slash = strlen(argv[i]) - 1, dot;

		while (slash > 0 && argv[i][slash] != '/')
			slash--;
		dot = ++slash;

		while (argv[i][dot] && argv[i][dot] != '.')
			dot++;
		strncpy(texname, argv[i] + slash, dot - slash);

		sprintf(filename, "%s/images/%s.bmp", outdir, texname);

		// printf("Saving file %s to %s\n", texname, filename);

		SDL_SaveBMP(tex, filename);

		SDL_FreeSurface(tex);
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		perror("not enough arguments\n"), (void)argv, exit(EXIT_FAILURE);

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		perror("SDL_Init"), exit(EXIT_FAILURE);
	if (TTF_Init() != 0)
		perror("TTF_Init"), exit(EXIT_FAILURE);

	int texs_start = -1, texs_end = -1, fonts_start = -1, fonts_end = -1;

	for (int i = 1; i < argc; i++)
	{
		if (strncmp(argv[i], "-f", 2) == 0)
		{
			font_ptsize = atoi(argv[i] + 2);
			if (++i == argc)
				perror("not enough arguments\n"), exit(EXIT_FAILURE);
			fonts_start = i++;
			while (i < argc && argv[i][0] != '-')
				i++;
			fonts_end = i--;
		}
		else if (strcmp(argv[i], "-i") == 0)
		{
			if (++i == argc)
				perror("not enough arguments\n"), exit(EXIT_FAILURE);
			texs_start = i++;
			while (i < argc && argv[i][0] != '-')
				i++;
			texs_end = i--;
		}
		else if (strcmp(argv[i], "-o") == 0)
		{
			if (++i == argc)
				perror("not enough arguments\n"), exit(EXIT_FAILURE);
			outdir = argv[i];
		}
		else
			fprintf(stderr, "unrecognised option: %s\n", argv[i]);
	}


	open_textures(argv, texs_start, texs_end);
	open_fonts(argv, fonts_start, fonts_end);
	
	TTF_Quit();
	SDL_Quit();
	return EXIT_SUCCESS;
}

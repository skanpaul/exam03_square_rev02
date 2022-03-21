/* ************************************************************************** */
#define NO_ERROR	0
#define ERROR		1
/* ************************************************************************** */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
/* ************************************************************************** */
typedef struct s_zone
{
	int		width;
	int		height;
	char	back_char;
	int		total;
} t_zone;

/* ------------------------------------ */
typedef struct s_square
{
	char	type;
	float	origin_x;
	float	origin_y;
	float	width;
	float	height;
	char	drawn_char;
} t_square;

/* ************************************************************************** */
int ft_strlen(char *str)
{
	int i;

	i = 0;
	while (str[i] != '\0')
		i++;
	return (i);
}

/* ************************************************************************** */
void clean_all(char **map, FILE **ptr_file)
{
	if (*map)
		free(*map);
	*map = NULL;

	if (*ptr_file)
		if ((fclose(*ptr_file)) == 0)
			*ptr_file = NULL;
}

/* ************************************************************************** */
char *get_zone_information(FILE *ptr_file, t_zone *z)
{
	char	*map;
	int		ret;

	map = NULL;
	// get square information ---------------------------------
	ret = fscanf(ptr_file, "%d %d %c\n", &z->width, &z->height, &z->back_char);

	// check end of file --------------------------------------
	if (ret == EOF) 
		return (NULL);

	// check if fscanf error ----------------------------------
	if (ret != 3)
		return (NULL);

	// check square limit -------------------------------------
	if (z->width <= 0 || z->width > 300 || z->height <= 0 || z->height > 300)
		return (NULL);

	// calcul total char-pixel --------------------------------
	z->total = z->width * z->height;

	// malloc map ---------------------------------------------
	if ((map = (char *)malloc(z->total * sizeof(char))) == NULL)
		return (NULL);

	// init map background ------------------------------------
	memset(map, z->back_char, z->total);

	// --------------------------------------------------------
	return (map);	
}

/* ************************************************************************** */
bool is_in_rectangle(int x, int y, t_square *s)
{
	float Xtl;
	float Xbr;
	float Ytl;
	float Ybr;
	
	float xf;
	float yf;

	xf = (float)x;
	yf = (float)y;

	Xtl = s->origin_x;
	Ytl = s->origin_y;

	Xbr = s->origin_x + s->width;	
	Ybr = s->origin_y + s->height;

	if ( (Xtl <= xf) && (xf <= Xbr) && (Ytl <= yf) && (yf<= Ybr))
		return (true);

	return (false);
}

/* ************************************************************************** */
bool is_on_edge(int x, int y, t_square *s)
{
	float Xtl;
	float Xbr;
	float Ytl;
	float Ybr;
	
	float xf;
	float yf;

	xf = (float)x;
	yf = (float)y;

	Xtl = s->origin_x;
	Ytl = s->origin_y;

	Xbr = s->origin_x + s->width;	
	Ybr = s->origin_y + s->height;

	if ((xf - Xtl < 1) || (Xbr - xf< 1))
		return (true);

	if ((yf - Ytl < 1) || (Ybr - yf < 1))
		return (true);

	return (false);
}

/* ************************************************************************** */
void print_map(char *map, t_zone *z)
{
	int x;
	int y;

	y = 0;
	while (y < z->height)
	{
		write(1, &map[y * z->width], z->width);
		write(1, "\n", 1);
		y++;
	}	
}

/* ************************************************************************** */
void draw_square(char *map, t_square *s, t_zone *z)
{
	int x;
	int y;

	y = 0;
	while (y < z->height)
	{
		x = 0;
		while (x < z->width)
		{
			if (is_in_rectangle(x, y, s))
			{
				if ((s->type == 'r') && (is_on_edge(x, y, s)))
				{
					map[x + y * z->width] = s->drawn_char;
					//print_map(map, z);
				}		
				if (s->type == 'R')
				{
					map[x + y * z->width] = s->drawn_char;
					//print_map(map, z);
				}
			}			
			x++;
		}
		y++;
	}
	//print_map(map, z);
}

/* ************************************************************************** */
char *do_map(FILE *ptr_file, char *map, t_square *s, t_zone *z)
{
	int ret;

	while (1)
	{
		// read commande
		ret = fscanf(ptr_file, "%c %f %f %f %f %c\n",
			&s->type, &s->origin_x, &s->origin_y, &s->width, &s->height, &s->drawn_char);

		if (ret == EOF)
			break ;

		if (ret != 6)
			return (NULL);

		if (s->width <= 0 || s->height <= 0)
			return (NULL);

		if (!(s->type == 'r' || s->type == 'R'))
			return (NULL);

		draw_square(map, s, z);

	}
	return (map);
}

/* ************************************************************************** */
/* ************************************************************************** */
/* ************************************************************************** */
int main (int argc, char **argv)
{
	FILE		*ptr_file;
	t_zone 		z;
	t_square 	s;
	char		*map;	
	
	map = NULL;
	ptr_file = NULL;
	
	if (argc == 2)
	{
		// ouvrir fichier; -------------------------
		if ((ptr_file = fopen(argv[1], "r")) == NULL)
		{
			write(1, "Error: Operation file corrupted\n", ft_strlen("Error: Operation file corrupted\n"));
			goto is_error;
		}
		// read zone information; ------------------
		if ((map = get_zone_information(ptr_file, &z)) == NULL)
			goto is_error;		

		// do map ----------------------------------
		if (do_map(ptr_file, map, &s, &z) == NULL)
			goto is_error;

		// print map -------------------------------
		print_map(map, &z);
	}
	else
	{
		write(1, "Error: argument\n", ft_strlen("Error: argument\n"));
		goto is_error;		
	}

	clean_all(&map, &ptr_file);
	return (NO_ERROR);

is_error:
	clean_all(&map, &ptr_file);
	return (ERROR);
}


/* ************************************************************************** */

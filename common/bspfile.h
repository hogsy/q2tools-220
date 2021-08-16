/*
===========================================================================
Copyright (C) 1997-2006 Id Software, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
===========================================================================
*/

#include "qfiles.h"

//qb: consolidate some defines here from duplicate places

// if a brush just barely pokes onto the other side,
// let it slide by without chopping
#define	PLANESIDE_EPSILON	0.1 //qb: maybe can be smaller, but this small leaves gaps //0.001

#define MAX_POINTS_ON_WINDING	64
#define	MAX_POINTS_ON_FIXED_WINDING	12

// you can define on_epsilon in the makefile as tighter
#ifndef	ON_EPSILON
#define	ON_EPSILON	0.1 //qb: was 0.1
#endif

extern	int			nummodels;
extern	dmodel_t	dmodels[MAX_MAP_MODELS_QBSP];

extern	int			visdatasize;
extern	byte		dvisdata[MAX_MAP_VISIBILITY_QBSP];
extern	dvis_t		*dvis;

extern	int			lightdatasize;
extern	byte		dlightdata[MAX_MAP_LIGHTING_QBSP];

extern	int			entdatasize;
extern	char		dentdata[MAX_MAP_ENTSTRING_QBSP];

extern	int			numleafs;
extern	dleaf_t	    dleafs[MAX_MAP_LEAFS];
extern	dleaf_tx	dleafsX[MAX_MAP_LEAFS_QBSP];

extern	int			numplanes;
extern	dplane_t	dplanes[MAX_MAP_PLANES_QBSP];

extern	int			numvertexes;
extern	dvertex_t	dvertexes[MAX_MAP_VERTS_QBSP];

extern	int			numnodes;
extern	dnode_t	    dnodes[MAX_MAP_NODES];
extern	dnode_tx	dnodesX[MAX_MAP_NODES_QBSP];

extern	int			numtexinfo;
extern	texinfo_t	texinfo[MAX_MAP_TEXINFO_QBSP];

extern	int			numfaces;
extern	dface_t	    dfaces[MAX_MAP_FACES];
extern	dface_tx	dfacesX[MAX_MAP_FACES_QBSP];

extern	int			numedges;
extern	dedge_t	    dedges[MAX_MAP_EDGES];
extern	dedge_tx	dedgesX[MAX_MAP_EDGES_QBSP];

extern	int			    numleaffaces;
extern	unsigned short	dleaffaces[MAX_MAP_LEAFFACES];
extern	unsigned int	dleaffacesX[MAX_MAP_LEAFFACES_QBSP];

extern	int			    numleafbrushes;
extern	unsigned short	dleafbrushes[MAX_MAP_LEAFBRUSHES];
extern	unsigned int	dleafbrushesX[MAX_MAP_LEAFBRUSHES_QBSP];

extern	int			numsurfedges;
extern	int			dsurfedges[MAX_MAP_SURFEDGES_QBSP];

extern	int			numareas;
extern	darea_t		dareas[MAX_MAP_AREAS];

extern	int			    numareaportals;
extern	dareaportal_t	dareaportals[MAX_MAP_AREAPORTALS];

extern	int			numbrushes;
extern	dbrush_t	dbrushes[MAX_MAP_BRUSHES_QBSP];

extern	int			    numbrushsides;
extern	dbrushside_t	dbrushsides[MAX_MAP_BRUSHSIDES];
extern	dbrushside_tx	dbrushsidesX[MAX_MAP_BRUSHSIDES_QBSP];

extern	byte		dpop[256];

void DecompressVis (byte *in, byte *decompressed);
int CompressVis (byte *vis, byte *dest);

void	LoadBSPFile (char *filename);
void	LoadBSPFileTexinfo (char *filename);	// just for 4data
void	WriteBSPFile (char *filename);
void	PrintBSPFileSizes (void);

//===============

typedef struct epair_s
{
	struct epair_s	*next;
	char	*key;
	char	*value;
} epair_t;

typedef struct
{
	vec3_t		origin;
	int			firstbrush;
	int			numbrushes;
	epair_t		*epairs;

// only valid for func_areaportals
	int			areaportalnum;
	int			portalareas[2];
} entity_t;

extern	int			num_entities;
extern	entity_t	entities[MAX_MAP_ENTITIES_QBSP];

void	ParseEntities (void);
void	UnparseEntities (void);

void 	SetKeyValue (entity_t *ent, char *key, char *value);
char 	*ValueForKey (entity_t *ent, char *key);
// will return "" if not present

vec_t	FloatForKey (entity_t *ent, char *key);
void 	GetVectorForKey (entity_t *ent, char *key, vec3_t vec);

epair_t *ParseEpair (void);
void	RemoveLastEpair( entity_t *ent );

void PrintEntity (entity_t *ent);

